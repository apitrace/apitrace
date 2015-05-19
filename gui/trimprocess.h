#pragma once

#include "apitrace.h"

#include <QObject>
#include <QProcess>

class TrimProcess : public QObject
{
    Q_OBJECT
public:
    TrimProcess(QObject *parent=0);
    ~TrimProcess();

    void setTrimIndex(int trimIndex);
    int trimIndex();

    void setTracePath(const QString &str);
    QString tracePath() const;

private:
    void updateTrimPath();

public slots:
    void start();

signals:
    void trimmedFile(const QString &trimPath);
    void error(const QString &msg);

private slots:
    void trimFinished();
    void trimError(QProcess::ProcessError err);

private:
    QStringList m_args;
    QString m_tracePath;
    QString m_trimPath;
    int m_trimIndex;
    QProcess *m_process;
};
