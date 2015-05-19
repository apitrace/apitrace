#pragma once

#include <QList>
#include <QObject>
#include <QString>

class QByteArray;
class QStringList;


struct AndroidDeviceInfo
{
    QString serialNumber;
    enum State { OkState, UnAuthorizedState, OfflineState };
    State state;
    bool unauthorized;
    enum AndroidDeviceType { Hardware, Emulator };
    AndroidDeviceType type;

    static QStringList adbSelector(const QString &serialNumber);
    static QString adbPath();
};

class AndroidUtils : public QObject
{
    Q_OBJECT
public:
    struct FileInfo
    {
        FileInfo();

        enum Type {
            File,
            Directory
        };
        Type type;
        QString name;
        qint64 size;
    };

    typedef QList<FileInfo>  FileInfoList;

    enum UpdateType {
        PushLocalToDevice,
        PullFromDeviceToLocal
    };

public:
    AndroidUtils();

    void reloadAdb();

    QString serialNumber() const;

    bool runAdb(const QStringList &params, QByteArray *output = 0) const;
    FileInfoList ls(const QString &path) const;

    bool sameFile(const QString &localPath, const QString &devicePath);
    bool updateFile(const QString &localPath, const QString &devicePath, UpdateType updateType);

signals:
    void statusMessage(const QString &message);

private slots:
    void processReadyRead();

private:
    QString m_adbPath;
    mutable qint64 m_wait;
    mutable QString m_serialNumber;
};
