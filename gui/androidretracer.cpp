#include "androidretracer.h"

#include "androiddevicedialog.h"

#include "thumbnail.h"

#include "image.hpp"
#include "trace_profiler.hpp"

#include <QHostAddress>
#include <QSettings>
#include <QTime>
#include <QBuffer>

#include "qubjson.h"

typedef QLatin1String _;

static QLatin1String packageName("apitrace.github.io.eglretrace");
static QLatin1String activityName("/.RetraceActivity");

AndroidRetracer::AndroidRetracer(QObject *parent)
    : Retracer(parent),
      m_stdoutPort(52341),
      m_stderrPort(52342)
{
    connect(&m_androidUtils, SIGNAL(statusMessage(QString)),
            SIGNAL(statusMessage(QString)));
}

void AndroidRetracer::setAndroidFileName(const QString &fileName)
{
    m_androdiFileName = fileName;
}

static int extractPidFromChunk(const QByteArray &chunk, int from)
{
    int pos1 = chunk.indexOf(' ', from);
    if (pos1 == -1)
        return -1;
    while (chunk[pos1] == ' ')
        ++pos1;
    int pos3 = chunk.indexOf(' ', pos1);
    int pid = chunk.mid(pos1, pos3 - pos1).toInt();
    return pid;
}

static int extractPid(const QByteArray &psOutput)
{
    static const QByteArray needle("apitrace.github.io.eglretrace\r");
    const int to = psOutput.indexOf(needle);
    if (to == -1)
        return -1;
    const int from = psOutput.lastIndexOf('\n', to);
    if (from == -1)
        return -1;
    return extractPidFromChunk(psOutput, from);
}

static QByteArray readLine(QTcpSocket &sock)
{
    while (!sock.canReadLine() && sock.waitForReadyRead());
    if (sock.state() != QAbstractSocket::ConnectedState)
        return QByteArray();
    return sock.readLine();
}

static bool read(QTcpSocket &sock, char *ptr, qint64 sz)
{
    do {
        qint64 readBytes = sock.read(ptr, sz);
        ptr += readBytes;
        sz -= readBytes;
    } while(sz && sock.waitForReadyRead());
    return sz == 0;
}

void AndroidRetracer::run()
{
    m_androidUtils.reloadAdb();
    QString errorStr;
    bool setupRet;
    QMetaObject::invokeMethod(this, "setup", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(bool, setupRet),
                              Q_ARG(QString *, &errorStr));

    if (!setupRet) {
        emit finished(errorStr);
        return;
    }

    if (!m_androidUtils.runAdb(QStringList() << _("shell") << _("am") << _("start") << _("-n") << packageName + activityName)) {
        emit finished(tr("Can't start apitrace application"));
        return;
    }
    QByteArray which;
    if (!m_androidUtils.runAdb(QStringList() << _("shell") << _("readlink") << _("$(which ps)") , &which)) {
        emit finished(tr("Can't start adb"));
        return;
    }

    bool isBusyBox = which.startsWith("busybox");
    QStringList psArgs;
    psArgs << _("shell") << _("ps");
    if (isBusyBox)
        psArgs << _("-w");

    qint64 processPID;
    bool wasStarted = false;
    QTime startTime;
    startTime.start();

    QTcpSocket stdoutSocket;
    QTcpSocket stderrSocket;

    ImageHash thumbnails;

    QVariantMap parsedJson;
    trace::Profile* profile = isProfiling() ? new trace::Profile() : NULL;

    QList<ApiTraceError> errors;
    QRegExp regexp("(^\\d+): +(\\b\\w+\\b): ([^\\r\\n]+)[\\r\\n]*$");

    QString msg = QLatin1String("Replay finished!");
    QByteArray ubjsonBuffer;
    QByteArray outputBuffer;
    bool keepGoing = true;
    while(keepGoing) {
        if (!wasStarted || startTime.elapsed() > 1000) {
            QByteArray psOut;
            m_androidUtils.runAdb(psArgs, &psOut);
            processPID = extractPid(psOut);
            if (wasStarted)
                startTime.restart();
        }

        if (processPID == -1) {
            if (wasStarted) {
                break;
            } else {
                if (startTime.elapsed() > 3000) { // wait 3 seconds to start
                    emit finished(tr("Unable to start retrace on device."));
                    return;
                }
            }
            msleep(100);
            continue;
        }

        // we have a valid pid, it means the application started
        if (!wasStarted) {
            // connect the sockets
            int tries = 0;
            do {
                stdoutSocket.connectToHost(QHostAddress::LocalHost, m_stdoutPort);
            } while (!stdoutSocket.waitForConnected(100) && ++tries < 10);
            if (stdoutSocket.state() != QAbstractSocket::ConnectedState) {
                emit finished(tr("Can't connect to stdout socket."));
                return;
            }

            // Android doesn't suport GPU and PPD profiling (at leats not on my devices)
            //setProfiling(false, isProfilingCpu(), false);

            QString args = (retraceArguments() << m_androdiFileName).join(" ") + _("\n");
            stdoutSocket.write(args.toUtf8());
            if (!stdoutSocket.waitForBytesWritten()) {
                emit finished(tr("Can't send params."));
                return;
            }


            stderrSocket.connectToHost(QHostAddress::LocalHost, m_stderrPort);
            stderrSocket.waitForConnected(100);
            if (stderrSocket.state() != QAbstractSocket::ConnectedState) {
                emit finished(tr("Can't connect to stderr socket."));
                return;
            }
            wasStarted = true;
        }

        // We are going to read both channels at the same time

        // read stdout channel
        if (stdoutSocket.waitForReadyRead(100)) {
            if (captureState())
                ubjsonBuffer.append(stdoutSocket.readAll());
            else if (captureThumbnails()) {
                // read one image
                image::PNMInfo info;
                QByteArray header;
                int headerLines = 3; // assume no optional comment line
                for (int headerLine = 0; headerLine < headerLines; ++headerLine) {
                    QByteArray line = readLine(stdoutSocket);
                    if (line.isEmpty()) {
                        keepGoing = false;
                        break;
                    }
                    header += line;
                    // if header actually contains optional comment line, ...
                    if (headerLine == 1 && line[0] == '#') {
                        ++headerLines;
                    }
                }

                const char *headerEnd = image::readPNMHeader(header.constData(), header.size(), info);

                // if invalid PNM header was encountered, ...
                if (headerEnd == NULL ||
                    info.channelType != image::TYPE_UNORM8) {
                    qDebug() << "error: invalid snapshot stream encountered";
                    keepGoing = false;
                    break;
                }

                unsigned channels = info.channels;
                unsigned width = info.width;
                unsigned height = info.height;

                // qDebug() << "channels: " << channels << ", width: " << width << ", height: " << height";

                QImage snapshot = QImage(width, height, channels == 1 ? QImage::Format_Mono : QImage::Format_RGB888);

                int rowBytes = channels * width;
                for (int y = 0; y < height; ++y) {
                    unsigned char *scanLine = snapshot.scanLine(y);
                    if (!read(stdoutSocket, (char *) scanLine, rowBytes)) {
                        keepGoing = false;
                        break;
                    }
                }

                QImage thumb = thumbnail(snapshot);
                thumbnails.insert(info.commentNumber, thumb);
            } else if (isProfiling()) {
                QByteArray line = readLine(stdoutSocket);
                if (line.isEmpty()) {
                    keepGoing = false;
                    break;
                }
                line.append('\0');
                trace::Profiler::parseLine(line.constData(), profile);
            } else {
                outputBuffer.append(stdoutSocket.readAll());
            }
        }

        // read stderr channel
        if (stderrSocket.waitForReadyRead(5) && stderrSocket.canReadLine()) {
            QString line = stderrSocket.readLine();
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
    }

    if (outputBuffer.size() < 80)
        msg = outputBuffer;

    if (captureState()) {
        QBuffer io(&ubjsonBuffer);
        io.open(QIODevice::ReadOnly);

        parsedJson = decodeUBJSONObject(&io).toMap();
        ApiTraceState *state = new ApiTraceState(parsedJson);
        emit foundState(state);
    }

    if (captureThumbnails() && !thumbnails.isEmpty()) {
        emit foundThumbnails(thumbnails);
    }

    if (isProfiling() && profile) {
        emit foundProfile(profile);
    }

    if (!errors.isEmpty()) {
        emit retraceErrors(errors);
    }

    emit finished(msg);
}

bool AndroidRetracer::setup(QString *error)
{
    m_androidUtils.reloadAdb();
    if (m_androidUtils.serialNumber().isEmpty()) {
        *error = tr("No device selected");
        return false;
    }

    // forward adbPorts
    QSettings s;
    s.beginGroup(_("android"));
    m_stdoutPort = s.value(_("stdoutPort"), m_stdoutPort).toInt();
    m_stderrPort = s.value(_("stderrPort"), m_stderrPort).toInt();
    s.endGroup();

    if (!m_androidUtils.runAdb(QStringList(_("forward"))
              << QString::fromLatin1("tcp:%1").arg(m_stdoutPort)
              << _("localabstract:apitrace.github.io.eglretrace.stdout"))) {
        *error = tr("Can't forward ports");
        return false;
    }

    if (!m_androidUtils.runAdb(QStringList(_("forward"))
              << QString::fromLatin1("tcp:%1").arg(m_stderrPort)
              << _("localabstract:apitrace.github.io.eglretrace.stderr"))) {
        *error = tr("Can't forward ports");
        return false;
    }

    return true;
}
