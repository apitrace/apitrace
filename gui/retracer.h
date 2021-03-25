#pragma once

#include "trace_api.hpp"
#include "apitrace.h"
#include "apitracecall.h"

#include <QThread>
#include <QProcess>

class ApiTraceState;

namespace trace { struct Profile; }

struct RetracerCallRange
{
    qlonglong m_callStartNo{0};
    qlonglong m_callEndNo{0};
};

class Retracer : public QThread
{
    Q_OBJECT
public:
    Retracer(QObject *parent=0);

    QString fileName() const;
    void setFileName(const QString &name);

    QString remoteTarget() const;
    void setRemoteTarget(const QString &host);

    void setAPI(trace::API api);

    bool isBenchmarking() const;
    void setBenchmarking(bool bench);

    bool isDoubleBuffered() const;
    void setDoubleBuffered(bool db);

    bool isSinglethread() const;
    void setSinglethread(bool singlethread);

    bool isCoreProfile() const;
    void setCoreProfile(bool coreprofile);

    bool isProfilingGpu() const;
    bool isProfilingCpu() const;
    bool isProfilingPixels() const;
    bool isProfiling() const;
    void setProfiling(bool gpu, bool cpu, bool pixels);

    bool isMsaaResolve() const;
    void setMsaaResolve(bool resolve);



    void setCaptureAtCallNumber(qlonglong num);
    qlonglong captureAtCallNumber() const;

    void setCallsToIgnore(const QList<RetracerCallRange>& callsToIgnore);

    bool captureState() const;
    void setCaptureState(bool enable);

    bool captureThumbnails() const;
    void setCaptureThumbnails(bool enable);

    void addThumbnailToCapture(qlonglong num);
    void resetThumbnailsToCapture();

    QString thumbnailCallSet();

    int queryHandling();
    void setQueryHandling(int handling);

    int queryCheckReportThreshold();
    void setQueryCheckReportThreshold(int value);

signals:
    void finished(const QString &output);
    void foundState(ApiTraceState *state);
    void foundProfile(trace::Profile *profile);
    void foundThumbnails(const QList<QImage> &thumbnails);
    void foundThumbnails(const ImageHash &thumbnails);
    void error(const QString &msg);
    void retraceErrors(const QList<ApiTraceError> &errors);

protected:
    virtual void run() override;

private:
    QString m_fileName;
    QString m_remoteTarget;
    trace::API m_api;
    bool m_benchmarking;
    bool m_doubleBuffered;
    bool m_singlethread;
    bool m_useCoreProfile;
    bool m_msaaResolve;
    bool m_captureState;
    bool m_captureThumbnails;
    qlonglong m_captureCall;
    bool m_profileGpu;
    bool m_profileCpu;
    bool m_profilePixels;
    int m_queryHandling;
    int m_queryCheckThreshold;

    QProcessEnvironment m_processEnvironment;

    QList<qlonglong> m_thumbnailsToCapture;

    QList<RetracerCallRange> m_callsToIgnore;
};
