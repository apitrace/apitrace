#ifndef APITRACE_H
#define APITRACE_H

#include "apitracecall.h"

#include <QObject>
#include <QSet>

class LoaderThread;
class SaverThread;

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
    void callError(ApiTraceCall *call);

    bool edited() const;
    bool needsSaving() const;

    bool isSaving() const;

    bool hasErrors() const;

public slots:
    void setFileName(const QString &name);
    void setFrameMarker(FrameMarker marker);
    void save();

signals:
    void startedLoadingTrace();
    void finishedLoadingTrace();
    void invalidated();
    void framesInvalidated();
    void changed(ApiTraceCall *call);
    void startedSaving();
    void saved();

    void beginAddingFrames(int oldCount, int numAdded);
    void endAddingFrames();
    void callsAdded(int oldCount, int numAdded);

private slots:
    void addFrames(const QList<ApiTraceFrame*> &frames);
    void slotSaved();
private:
    void detectFrames();
private:
    QString m_fileName;
    QString m_tempFileName;

    QList<ApiTraceFrame*> m_frames;
    QList<ApiTraceCall*> m_calls;

    FrameMarker m_frameMarker;

    LoaderThread *m_loader;
    SaverThread  *m_saver;

    QSet<ApiTraceCall*> m_editedCalls;

    bool m_needsSaving;

    QSet<ApiTraceCall*> m_errors;
};

#endif
