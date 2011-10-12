#ifndef APITRACE_H
#define APITRACE_H

#include "apitracecall.h"

#include <QObject>
#include <QSet>

class TraceLoader;
class SaverThread;
class QThread;

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
    enum SearchResult {
        SearchResult_NotFound,
        SearchResult_Found,
        SearchResult_Wrapped
    };
    struct SearchRequest {
        enum Direction {
            Next,
            Prev
        };
        SearchRequest()
            : direction(Next)
        {}
        SearchRequest(Direction dir,
                      ApiTraceFrame *f,
                      ApiTraceCall *call,
                      QString str,
                      Qt::CaseSensitivity caseSens)
            : direction(dir),
              frame(f),
              from(call),
              text(str),
              cs(caseSens)
        {}
        Direction direction;
        ApiTraceFrame *frame;
        ApiTraceCall *from;
        QString text;
        Qt::CaseSensitivity cs;
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

    ApiTraceCall *callWithIndex(int idx) const;

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
    void save();
    void loadFrame(ApiTraceFrame *frame);
    void findNext(ApiTraceFrame *frame,
                  ApiTraceCall *call,
                  const QString &str,
                  Qt::CaseSensitivity sensitivity);
    void findPrev(ApiTraceFrame *frame,
                  ApiTraceCall *call,
                  const QString &str,
                  Qt::CaseSensitivity sensitivity);
    void findFrameStart(ApiTraceFrame *frame);
    void findFrameEnd(ApiTraceFrame *frame);
    void findCallIndex(int index);
    void setCallError(const ApiTraceError &error);


signals:
    void loadTrace(const QString &name);
    void requestFrame(ApiTraceFrame *frame);
    void startedLoadingTrace();
    void loaded(int percent);
    void finishedLoadingTrace();
    void invalidated();
    void framesInvalidated();
    void changed(ApiTraceCall *call);
    void startedSaving();
    void saved();
    void findResult(const ApiTrace::SearchRequest &request,
                    ApiTrace::SearchResult result,
                    ApiTraceCall *call);

    void beginAddingFrames(int oldCount, int numAdded);
    void endAddingFrames();
    void beginLoadingFrame(ApiTraceFrame *frame, int numAdded);
    void endLoadingFrame(ApiTraceFrame *frame);
    void foundFrameStart(ApiTraceFrame *frame);
    void foundFrameEnd(ApiTraceFrame *frame);
    void foundCallIndex(ApiTraceCall *call);

signals:
    void loaderSearch(const ApiTrace::SearchRequest &request);
    void loaderFindFrameStart(ApiTraceFrame *frame);
    void loaderFindFrameEnd(ApiTraceFrame *frame);
    void loaderFindCallIndex(int index);

private slots:
    void addFrames(const QList<ApiTraceFrame*> &frames);
    void slotSaved();
    void finishedParsing();
    void loaderFrameLoaded(ApiTraceFrame *frame,
                           const QVector<ApiTraceCall*> &calls,
                           quint64 binaryDataSize);
    void loaderSearchResult(const ApiTrace::SearchRequest &request,
                            ApiTrace::SearchResult result,
                            ApiTraceCall *call);

private:
    int callInFrame(int callIdx) const;
    bool isFrameLoading(ApiTraceFrame *frame) const;
private:
    QString m_fileName;
    QString m_tempFileName;

    QList<ApiTraceFrame*> m_frames;

    FrameMarker m_frameMarker;

    TraceLoader *m_loader;
    QThread     *m_loaderThread;
    SaverThread  *m_saver;

    QSet<ApiTraceCall*> m_editedCalls;

    bool m_needsSaving;

    QSet<ApiTraceCall*> m_errors;
    QList< QPair<ApiTraceFrame*, ApiTraceError> > m_queuedErrors;
    QSet<ApiTraceFrame*> m_loadingFrames;
};

#endif
