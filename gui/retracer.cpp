#include "retracer.h"
#include <iostream>

#include "apitracecall.h"

#include "image.hpp"

#include <QDebug>
#include <QVariant>
#include <QList>
#include <QImage>

#include <qjson/parser.h>

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
    QString msg;

    /*
     * Construct command line
     */

    QString prog;
    QStringList arguments;

    if (m_api == trace::API_GL) {
        prog = QLatin1String("glretrace");
    } else if (m_api == trace::API_EGL) {
        prog = QLatin1String("eglretrace");
    } else {
        Q_ASSERT(0);
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

    process.start(prog, arguments);
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
        if (m_captureState) {
            /*
             * Parse JSON from the output.
             *
             * XXX: QJSON does not wait for QIODevice::waitForReadyRead so we
             * need to buffer all stdout.
             *
             * XXX: QJSON's scanner is inneficient as it abuses single
             * character QIODevice::peek (not cheap), instead of maintaining a
             * lookahead character on its own.
             */

            if (!process.waitForFinished(-1)) {
                return;
            }

            bool ok = false;
            QJson::Parser jsonParser;
            parsedJson = jsonParser.parse(&process, &ok).toMap();
            if (!ok) {
                msg = QLatin1String("failed to parse JSON");
            }
        } else if (m_captureThumbnails) {
            /*
             * Parse concatenated PNM images from output.
             */

            while (true) {
                /*
                 * QProcess::atEnd() documentation is wrong -- it will return
                 * true even when the process is running --, so try to handle
                 * that here.
                 */
                if (process.atEnd()) {
                    if (process.state() == QProcess::Running) {
                        if (!process.waitForReadyRead(-1)) {
                            break;
                        }
                    }
                }

                unsigned channels = 0;
                unsigned width = 0;
                unsigned height = 0;

                char header[512];
                qint64 headerSize = 0;
                int headerLines = 3; // assume no optional comment line

                for (int headerLine = 0; headerLine < headerLines; ++headerLine) {
                    while (!process.canReadLine()) {
                        if (!process.waitForReadyRead(-1)) {
                            qDebug() << "QProcess::waitForReadyRead failed";
                            break;
                        }
                    }

                    qint64 headerRead = process.readLine(&header[headerSize], sizeof(header) - headerSize);

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

                    while (process.bytesAvailable() < rowBytes) {
                        if (!process.waitForReadyRead(-1)) {
                            qDebug() << "QProcess::waitForReadyRead failed";
                            break;
                        }
                    }

                    qint64 read = process.read((char *) scanLine, rowBytes);
                    Q_ASSERT(read == rowBytes);
                }

                QImage thumbnail = snapshot.scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation);
                thumbnails.append(thumbnail);
            }

            Q_ASSERT(process.state() != QProcess::Running);

        } else {
            QByteArray output;
            output = process.readAllStandardOutput();
            msg = QString::fromUtf8(output);
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
