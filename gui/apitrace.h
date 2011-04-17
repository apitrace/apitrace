#ifndef APITRACE_H
#define APITRACE_H

#include "apitracecall.h"

#include <QObject>
#include <QSet>

class LoaderThread;

class ApiTrace : public QObject
{
    Q_OBJECT
public:
    enum FrameMarker {
        FrameMarker_SwapBuffers,
        FrameMarker_Flush,
        FrameMarker_Finish,
        FrameMarker_Clear
    };
    static bool isCallAFrameMarker(const ApiTraceCall *call,
                                   FrameMarker marker);
public:
    ApiTrace();
    ~ApiTrace();

    bool isEmpty() const;

    QString fileName() const;

    FrameMarker frameMarker() const;

    ApiTraceState defaultState() const;

    QList<ApiTraceCall*> calls() const;
    ApiTraceCall *callAt(int idx) const;
    ApiTraceCall *callWithIndex(int idx) const;
    int numCalls() const;

    QList<ApiTraceFrame*> frames() const;
    ApiTraceFrame *frameAt(int idx) const;
    int numFrames() const;
    int numCallsInFrame(int idx) const;

    void callEdited(ApiTraceCall *call);
    void callReverted(ApiTraceCall *call);

    bool isEdited() const;

public slots:
    void setFileName(const QString &name);
    void setFrameMarker(FrameMarker marker);

signals:
    void startedLoadingTrace();
    void finishedLoadingTrace();
    void invalidated();
    void framesInvalidated();
    void changed(ApiTraceCall *call);

    void framesAdded(int oldCount, int numAdded);
    void callsAdded(int oldCount, int numAdded);

private slots:
    void addFrames(const QList<ApiTraceFrame*> &frames);
private:
    void detectFrames();
private:
    QString m_fileName;

    QList<ApiTraceFrame*> m_frames;
    QList<ApiTraceCall*> m_calls;

    FrameMarker m_frameMarker;

    LoaderThread *m_loader;

    QSet<ApiTraceCall*> m_editedCalls;
};

#endif
