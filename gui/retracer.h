#ifndef RETRACER_H
#define RETRACER_H

#include "trace_api.hpp"
#include "apitracecall.h"

#include <QThread>
#include <QProcess>

class ApiTraceState;

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

    bool captureThumbnails() const;
    void setCaptureThumbnails(bool enable);

signals:
    void finished(const QString &output);
    void foundState(ApiTraceState *state);
    void foundThumbnails(const QList<QImage> &thumbnails);
    void error(const QString &msg);
    void retraceErrors(const QList<ApiTraceError> &errors);

protected:
    virtual void run();

private:
    QString m_fileName;
    trace::API m_api;
    bool m_benchmarking;
    bool m_doubleBuffered;
    bool m_captureState;
    bool m_captureThumbnails;
    qlonglong m_captureCall;

    QProcessEnvironment m_processEnvironment;
};

#endif
