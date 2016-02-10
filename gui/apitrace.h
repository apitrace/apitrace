#pragma once

#include "apitracecall.h"

#include "trace_api.hpp"

#include <QObject>
#include <QSet>

class TraceLoader;
class SaverThread;
class QThread;

typedef void (*ThumbnailCallback)(void *object, int thumbnailIdx);
typedef QHash<int, QImage> ImageHash;

class ApiTrace : public QObject
{
    Q_OBJECT
public:
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

public:
    ApiTrace();
    ~ApiTrace();

    bool isEmpty() const;

    QString fileName() const;

    ApiTraceState defaultState() const;

    ApiTraceCall *callWithIndex(int idx) const;

    const QList<ApiTraceFrame*> & frames() const;
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

    trace::API api() const;

    void missingThumbnail(ApiTraceFrame* frame);
    void missingThumbnail(ApiTraceCall* call);

    bool isMissingThumbnails() const;
    void resetMissingThumbnails();

    void iterateMissingThumbnails(void *object, ThumbnailCallback cb);

public slots:
    void setFileName(const QString &name);
    void save();
    void finishedParsing();
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

    void bindThumbnails(const ImageHash &thumbnails);

signals:
    void loadTrace(const QString &name);
    void requestFrame(ApiTraceFrame *frame);
    void problemLoadingTrace(const QString &message);
    void startedLoadingTrace();
    void loaded(int percent);
    void finishedLoadingTrace();
    void invalidated();
    void framesInvalidated();
    void changed(ApiTraceEvent *event);
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
    void guessedApi(int api);
    void loaderFrameLoaded(ApiTraceFrame *frame,
                           const QVector<ApiTraceCall*> &topLevelItems,
                           const QVector<ApiTraceCall*> &calls,
                           quint64 binaryDataSize);
    void loaderSearchResult(const ApiTrace::SearchRequest &request,
                            ApiTrace::SearchResult result,
                            ApiTraceCall *call);

private:
    int callInFrame(int callIdx) const;
    bool isFrameLoading(ApiTraceFrame *frame) const;

    void missingThumbnail(int callIdx);
private:
    QString m_fileName;
    QString m_tempFileName;

    QList<ApiTraceFrame*> m_frames;
    trace::API m_api;

    TraceLoader *m_loader;
    QThread     *m_loaderThread;
    SaverThread  *m_saver;

    QSet<ApiTraceCall*> m_editedCalls;

    bool m_needsSaving;

    QSet<ApiTraceCall*> m_errors;
    QList< QPair<ApiTraceFrame*, ApiTraceError> > m_queuedErrors;
    QSet<ApiTraceFrame*> m_loadingFrames;

    QSet<int> m_missingThumbnails;

    ImageHash m_thumbnails;
};
