#pragma once

#include <QObject>
#include <QProcess>

class TraceProcess : public QObject
{
    Q_OBJECT
public:
    TraceProcess(QObject *parent=0);
    ~TraceProcess();

    bool canTrace() const;

    void setApi(const QString &str);
    void setExecutablePathAndWorkingDir(const QString &execPath, const QString &workingDir);
    QString executablePath() const;
    QString workingDirectory() const;

    void setArguments(const QStringList &args);
    QStringList arguments() const;

public slots:
    void start();

signals:
    void tracedFile(const QString &tracePath);
    void error(const QString &msg);

private slots:
    void traceFinished();
    void traceError(QProcess::ProcessError err);

private:
    QString m_api;
    QString m_execPath;
    QString m_workingDir;
    QStringList m_args;
    QString m_tracePath;
    QProcess *m_process;

    bool m_canTrace;
};
