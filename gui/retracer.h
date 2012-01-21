#ifndef RETRACER_H
#define RETRACER_H

#include "trace_api.hpp"
#include "apitracecall.h"

#include <QThread>
#include <QProcess>

class ApiTraceState;
namespace QJson {
    class Parser;
}

/* internal class used by the retracer to run
 * in the thread */
class RetraceProcess : public QObject
{
    Q_OBJECT
public:
    RetraceProcess(QObject *parent=0);
    ~RetraceProcess();

    QProcess *process() const;

    QString fileName() const;
    void setFileName(const QString &name);

    void setAPI(trace::API api);

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
    void finished(const QString &output);
    void error(const QString &msg);
    void foundState(ApiTraceState *state);
    void retraceErrors(const QList<ApiTraceError> &errors);

private slots:
    void replayFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void replayError(QProcess::ProcessError err);

private:
    QString m_fileName;
    trace::API m_api;
    bool m_benchmarking;
    bool m_doubleBuffered;
    bool m_captureState;
    qlonglong m_captureCall;

    QProcess *m_process;
    QJson::Parser *m_jsonParser;
};

class Retracer : public QThread
{
    Q_OBJECT
public:
    Retracer(QObject *parent=0);

    QString fileName() const;
    void setFileName(const QString &name);

    void setAPI(trace::API api);

    bool isBenchmarking() const;
    void setBenchmarking(bool bench);

    bool isDoubleBuffered() const;
    void setDoubleBuffered(bool db);

    void setCaptureAtCallNumber(qlonglong num);
    qlonglong captureAtCallNumber() const;

    bool captureState() const;
    void setCaptureState(bool enable);

signals:
    void finished(const QString &output);
    void foundState(ApiTraceState *state);
    void error(const QString &msg);
    void retraceErrors(const QList<ApiTraceError> &errors);

protected:
    virtual void run();

private slots:
    void cleanup();
private:
    QString m_fileName;
    trace::API m_api;
    bool m_benchmarking;
    bool m_doubleBuffered;
    bool m_captureState;
    qlonglong m_captureCall;

    QProcessEnvironment m_processEnvironment;
};

#endif
