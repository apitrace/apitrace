#include "retracer.h"

#include "apitracecall.h"
#include "thumbnail.h"

#include "image.hpp"

#include <QDebug>
#include <QVariant>
#include <QList>
#include <QImage>

#include <qjson/parser.h>

/**
 * Wrapper around a QProcess which enforces IO to block .
 *
 * Several QIODevice users (notably QJSON) expect blocking semantics, e.g.,
 * they expect that QIODevice::read() will blocked until the requested ammount
 * of bytes is read or end of file is reached. But by default QProcess, does
 * not block.  And passing QIODevice::Unbuffered mitigates but does not fully
 * address the problem either.
 *
 * This class wraps around QProcess, providing QIODevice interface, while
 * ensuring that all reads block.
 *
 * This class also works around a bug in QProcess::atEnd() implementation.
 *
 * See also:
 * - http://qt-project.org/wiki/Simple_Crypt_IO_Device
 * - http://qt-project.org/wiki/Custom_IO_Device
 */
class BlockingIODevice : public QIODevice
{
    /* We don't use the Q_OBJECT in this class given we don't declare any
     * signals and slots or use any other services provided by Qt's meta-object
     * system. */
public:
    BlockingIODevice(QProcess * io);
    bool isSequential() const;
    bool atEnd() const;
    bool waitForReadyRead(int msecs = -1);

protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);

private:
    QProcess *m_device;
};

BlockingIODevice::BlockingIODevice(QProcess * io) :
    m_device(io)
{
    /*
     * We pass QIODevice::Unbuffered to prevent the base QIODevice class to do
     * its own buffering on top of the overridden readData() method.
     *
     * The only buffering used will be to satisfy QIODevice::peek() and
     * QIODevice::ungetChar().
     */
    setOpenMode(ReadOnly | Unbuffered);
}

bool BlockingIODevice::isSequential() const
{
    return true;
}

bool BlockingIODevice::atEnd() const
{
    /*
     * XXX: QProcess::atEnd() documentation is wrong -- it will return true
     * even when the process is running --, so we try to workaround that here.
     */
    if (m_device->atEnd()) {
        if (m_device->state() == QProcess::Running) {
            if (!m_device->waitForReadyRead(-1)) {
                return true;
            }
        }
    }
    return false;
}

bool BlockingIODevice::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return true;
}

qint64 BlockingIODevice::readData(char * data, qint64 maxSize)
{
    qint64 bytesToRead = maxSize;
    qint64 readSoFar = 0;
    do {
        qint64 chunkSize = m_device->read(data + readSoFar, bytesToRead);
        if (chunkSize < 0) {
            if (readSoFar) {
                return readSoFar;
            } else {
                return chunkSize;
            }
        }
        Q_ASSERT(chunkSize <= bytesToRead);
        bytesToRead -= chunkSize;
        readSoFar += chunkSize;
        if (bytesToRead) {
            if (!m_device->waitForReadyRead(-1)) {
                qDebug() << "waitForReadyRead failed\n";
                break;
            }
        }
    } while(bytesToRead);

    return readSoFar;
}

qint64 BlockingIODevice::writeData(const char * data, qint64 maxSize)
{
    Q_ASSERT(false);
    return -1;
}

Q_DECLARE_METATYPE(QList<ApiTraceError>);

Retracer::Retracer(QObject *parent)
    : QThread(parent),
      m_benchmarking(false),
      m_doubleBuffered(true),
      m_captureState(false),
      m_captureCall(0)
{
    qRegisterMetaType<QList<ApiTraceError> >();

#ifdef Q_OS_WIN
    QString format = QLatin1String("%1;");
#else
    QString format = QLatin1String("%1:");
#endif
    QString buildPath = format.arg(APITRACE_BINARY_DIR);
    m_processEnvironment = QProcessEnvironment::systemEnvironment();
    m_processEnvironment.insert("PATH", buildPath +
                                m_processEnvironment.value("PATH"));

    qputenv("PATH",
            m_processEnvironment.value("PATH").toLatin1());
}

QString Retracer::fileName() const
{
    return m_fileName;
}

void Retracer::setFileName(const QString &name)
{
    m_fileName = name;
}

void Retracer::setAPI(trace::API api)
{
    m_api = api;
}

bool Retracer::isBenchmarking() const
{
    return m_benchmarking;
}

void Retracer::setBenchmarking(bool bench)
{
    m_benchmarking = bench;
}

bool Retracer::isDoubleBuffered() const
{
    return m_doubleBuffered;
}

void Retracer::setDoubleBuffered(bool db)
{
    m_doubleBuffered = db;
}

void Retracer::setCaptureAtCallNumber(qlonglong num)
{
    m_captureCall = num;
}

qlonglong Retracer::captureAtCallNumber() const
{
    return m_captureCall;
}

bool Retracer::captureState() const
{
    return m_captureState;
}

void Retracer::setCaptureState(bool enable)
{
    m_captureState = enable;
}

bool Retracer::captureThumbnails() const
{
    return m_captureThumbnails;
}

void Retracer::setCaptureThumbnails(bool enable)
{
    m_captureThumbnails = enable;
}


/**
 * Starting point for the retracing thread.
 *
 * Overrides QThread::run().
 */
void Retracer::run()
{
    QString msg = QLatin1String("Replay finished!");

    /*
     * Construct command line
     */

    QString prog;
    QStringList arguments;

    switch (m_api) {
    case trace::API_GL:
        prog = QLatin1String("glretrace");
        break;
    case trace::API_EGL:
        prog = QLatin1String("eglretrace");
        break;
    case trace::API_DX:
    case trace::API_D3D7:
    case trace::API_D3D8:
    case trace::API_D3D9:
    case trace::API_D3D10:
    case trace::API_D3D10_1:
    case trace::API_D3D11:
#ifdef Q_OS_WIN
        prog = QLatin1String("d3dretrace");
#else
        prog = QLatin1String("wine");
        arguments << QLatin1String("d3dretrace.exe");
#endif
        break;
    default:
        emit finished(QLatin1String("Unsupported API"));
        return;
    }

    if (m_doubleBuffered) {
        arguments << QLatin1String("-db");
    } else {
        arguments << QLatin1String("-sb");
    }

    if (m_captureState) {
        arguments << QLatin1String("-D");
        arguments << QString::number(m_captureCall);
    } else if (m_captureThumbnails) {
        arguments << QLatin1String("-s"); // emit snapshots
        arguments << QLatin1String("-"); // emit to stdout
    } else if (m_benchmarking) {
        arguments << QLatin1String("-b");
    }

    arguments << m_fileName;

    /*
     * Start the process.
     */

    QProcess process;

    process.start(prog, arguments, QIODevice::ReadOnly);
    if (!process.waitForStarted(-1)) {
        emit finished(QLatin1String("Could not start process"));
        return;
    }

    /*
     * Process standard output
     */

    QList<QImage> thumbnails;
    QVariantMap parsedJson;

    process.setReadChannel(QProcess::StandardOutput);
    if (process.waitForReadyRead(-1)) {
        BlockingIODevice io(&process);

        if (m_captureState) {
            /*
             * Parse JSON from the output.
             *
             * XXX: QJSON's scanner is inneficient as it abuses single
             * character QIODevice::peek (not cheap), instead of maintaining a
             * lookahead character on its own.
             */

            bool ok = false;
            QJson::Parser jsonParser;
#if 0
            parsedJson = jsonParser.parse(&io, &ok).toMap();
#else
            /*
             * XXX: QJSON expects blocking IO, and it looks like
             * BlockingIODevice does not work reliably in all cases.
             */
            process.waitForFinished(-1);
            parsedJson = jsonParser.parse(&process, &ok).toMap();
#endif
            if (!ok) {
                msg = QLatin1String("failed to parse JSON");
            }
        } else if (m_captureThumbnails) {
            /*
             * Parse concatenated PNM images from output.
             */

            while (!io.atEnd()) {
                unsigned channels = 0;
                unsigned width = 0;
                unsigned height = 0;

                char header[512];
                qint64 headerSize = 0;
                int headerLines = 3; // assume no optional comment line

                for (int headerLine = 0; headerLine < headerLines; ++headerLine) {
                    qint64 headerRead = io.readLine(&header[headerSize], sizeof(header) - headerSize);

                    // if header actually contains optional comment line, ...
                    if (headerLine == 1 && header[headerSize] == '#') {
                        ++headerLines;
                    }

                    headerSize += headerRead;
                }

                const char *headerEnd = image::readPNMHeader(header, headerSize, &channels, &width, &height);

                // if invalid PNM header was encountered, ...
                if (header == headerEnd) {
                    qDebug() << "error: invalid snapshot stream encountered";
                    break;
                }

                // qDebug() << "channels: " << channels << ", width: " << width << ", height: " << height";

                QImage snapshot = QImage(width, height, channels == 1 ? QImage::Format_Mono : QImage::Format_RGB888);

                int rowBytes = channels * width;
                for (int y = 0; y < height; ++y) {
                    unsigned char *scanLine = snapshot.scanLine(y);
                    qint64 readBytes = io.read((char *) scanLine, rowBytes);
                    Q_ASSERT(readBytes == rowBytes);
                }

                QImage thumb = thumbnail(snapshot);
                thumbnails.append(thumb);
            }

            Q_ASSERT(process.state() != QProcess::Running);

        } else {
            QByteArray output;
            output = process.readAllStandardOutput();
            if (output.length() < 80) {
                msg = QString::fromUtf8(output);
            }
        }
    }

    /*
     * Wait for process termination
     */

    process.waitForFinished(-1);

    if (process.exitStatus() != QProcess::NormalExit) {
        msg = QLatin1String("Process crashed");
    } else if (process.exitCode() != 0) {
        msg = QLatin1String("Process exited with non zero exit code");
    }

    /*
     * Parse errors.
     */

    QList<ApiTraceError> errors;
    process.setReadChannel(QProcess::StandardError);
    QRegExp regexp("(^\\d+): +(\\b\\w+\\b): ([^\\r\\n]+)[\\r\\n]*$");
    while (!process.atEnd()) {
        QString line = process.readLine();
        if (regexp.indexIn(line) != -1) {
            ApiTraceError error;
            error.callIndex = regexp.cap(1).toInt();
            error.type = regexp.cap(2);
            error.message = regexp.cap(3);
            errors.append(error);
        } else if (!errors.isEmpty()) {
            // Probably a multiligne message
            ApiTraceError &previous = errors.last();
            if (line.endsWith("\n")) {
                line.chop(1);
            }
            previous.message.append('\n');
            previous.message.append(line);
        }
    }

    /*
     * Emit signals
     */

    if (m_captureState) {
        ApiTraceState *state = new ApiTraceState(parsedJson);
        emit foundState(state);
        msg = QLatin1String("State fetched.");
    }

    if (m_captureThumbnails && !thumbnails.isEmpty()) {
        emit foundThumbnails(thumbnails);
    }

    if (!errors.isEmpty()) {
        emit retraceErrors(errors);
    }

    emit finished(msg);
}

#include "retracer.moc"
