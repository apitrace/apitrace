#ifndef RETRACER_H
#define RETRACER_H

#include <QObject>
#include <QProcess>

class Retracer : public QObject
{
    Q_OBJECT
public:
    Retracer(QObject *parent=0);

    QString fileName() const;
    void setFileName(const QString &name);

    bool isBenchmarking() const;
    void setBenchmarking(bool bench);

    bool isDoubleBuffered() const;
    void setDoubleBuffered(bool db);

    void setCaptureAtCallNumber(qlonglong num);
    qlonglong captureAtCallNumber() const;

    bool captureState() const;
    void setCaptureState(bool enable);

public slots:
    void start();
    void terminate();

signals:
    void finished(const QByteArray &output);
    void error(const QString &msg);

private slots:
    void replayFinished();
    void replayError(QProcess::ProcessError err);
private:
    QString m_fileName;
    bool m_benchmarking;
    bool m_doubleBuffered;
    bool m_captureState;
    qlonglong m_captureCall;

    QProcess *m_process;
};

#endif
