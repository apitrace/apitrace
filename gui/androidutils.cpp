#include "androidutils.h"
#include "androiddevicedialog.h"

#include <QCoreApplication>
#include <QByteArray>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>
#include <QStringList>

typedef QLatin1String _;

static QString s_androidSerialNumber;

QStringList AndroidDeviceInfo::adbSelector(const QString &serialNumber)
{
    if (serialNumber.startsWith(_("????")))
        return QStringList() << _("-d");
    return QStringList() << _("-s") << serialNumber;
}

QString AndroidDeviceInfo::adbPath()
{
    QSettings s;
    s.beginGroup(_("android"));
    return s.value(_("adbPath"), _("adb")).toString();
}

AndroidUtils::FileInfo::FileInfo()
{
    type = File;
    size = 0;
}

AndroidUtils::AndroidUtils()
{
    m_wait = 30000;
    reloadAdb();
}

void AndroidUtils::reloadAdb()
{
    m_adbPath = AndroidDeviceInfo::adbPath();
}

QString AndroidUtils::serialNumber() const
{
    if (m_serialNumber.isEmpty())
        m_serialNumber = s_androidSerialNumber;

    if (m_serialNumber.isEmpty()) {
        AndroidDeviceDialog ad;
        if (ad.exec() != QDialog::Accepted)
            return QString();

        m_serialNumber = ad.device().serialNumber;
        if (ad.saveDeviceSelection())
            s_androidSerialNumber = m_serialNumber;
    }
    return m_serialNumber;
}

bool AndroidUtils::runAdb(const QStringList &params, QByteArray *output) const
{
    QProcess adbProcess;
    adbProcess.start(m_adbPath, AndroidDeviceInfo::adbSelector(serialNumber()) << params);
    if (!output)
        connect(&adbProcess, SIGNAL(readyReadStandardError()), SLOT(processReadyRead()));

    if (!adbProcess.waitForStarted())
        return false;

    if (!adbProcess.waitForFinished(m_wait > INT_MAX ? -1 : m_wait))
        return false;

    if (output)
        *output = adbProcess.readAll();
    return true;
}

AndroidUtils::FileInfoList AndroidUtils::ls(const QString &path) const
{
    m_wait = 10000;
    FileInfoList ret;
    QByteArray lsOutput;
    runAdb(QStringList(_("shell")) << _("ls") << _("-l") << path, &lsOutput);
    foreach (const QByteArray &line, lsOutput.split('\n')) {
        if (line.isEmpty() || line[10] != ' ')
            continue;

        int pos = line.indexOf(':');
        if (pos == -1)
            continue;
        pos += 3;
        if (pos >= line.size())
            continue;

        FileInfo f;
        // Assume all links are directories
        f.type = (line[0] == 'd' || line[0] == 'l') ? FileInfo::Directory : FileInfo::File;
        f.name = line.mid(pos).trimmed();
        if (f.type == FileInfo::File) {
            pos = line.lastIndexOf('-', pos);
            if (pos < 4)
                continue;
            pos = line.lastIndexOf('-', pos -1);
            if (pos < 3)
                continue;
            pos = line.lastIndexOf(' ', pos -1);
            if (pos < 2)
                continue;
            int startPos = line.lastIndexOf(' ', pos -1);
            if (startPos == -1)
                continue;
            f.size = line.mid(startPos, pos - startPos).trimmed().toLongLong();
        } else if (line[0] == 'l') {
            // Fix links
            pos = f.name.indexOf(_("->"));
            if (pos < 2)
                continue;
            f.name = f.name.left(pos - 1).trimmed();
        }
        ret << f;
    }
    return ret;
}

bool AndroidUtils::sameFile(const QString &localPath, const QString &devicePath)
{
    QFileInfo fi(localPath);
    QList<FileInfo> deviceFiles = ls(devicePath);
    return fi.exists() && (deviceFiles.size() == 1) && (deviceFiles.first().size == fi.size());
}

bool AndroidUtils::updateFile(const QString &localPath, const QString &devicePath, UpdateType updateType)
{
    if (sameFile(localPath, devicePath))
        return true;

    QFileInfo fi(localPath);
    QList<FileInfo> deviceFiles = ls(devicePath);

    switch (updateType) {
    case PushLocalToDevice:
        if (!fi.exists())
            return false;

        m_wait = std::min(fi.size() / 1024, qint64(30000));
        if (!runAdb(QStringList()<< _("push") << _("-p") << localPath << devicePath))
            return false;

        deviceFiles = ls(devicePath);

        if (deviceFiles.size() != 1 || deviceFiles.first().size != fi.size())
            return false; // pushing to device failed
        break;

    case PullFromDeviceToLocal:
        if (deviceFiles.size() != 1)
            return false;

        m_wait = std::min(deviceFiles.at(0).size / 1024, qint64(30000));
        if (!runAdb(QStringList()<< _("pull") << _("-p") << devicePath << localPath))
            return false;

        fi = QFileInfo(localPath);
        if (!fi.exists() || deviceFiles.first().size != fi.size())
            return false; // pulling from device failed
        break;
    }
    return true;
}

void AndroidUtils::processReadyRead()
{
    QProcess *proc = qobject_cast<QProcess *>(QObject::sender());
    if (proc) {
        QByteArray stdOutErr = proc->readAllStandardError();
        foreach (const QByteArray &line,  stdOutErr.split('\r')) {
            QString msg(line.trimmed());
            if (!msg.isEmpty())
                emit statusMessage(msg);
        }
    }
}
