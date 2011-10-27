#ifndef TRACELOADER_H
#define TRACELOADER_H


#include "apitrace.h"
#include "trace_file.hpp"
#include "trace_parser.hpp"

#include <QObject>
#include <QList>
#include <QMap>

class TraceLoader : public QObject
{
    Q_OBJECT
public:
    TraceLoader(QObject *parent=0);
    ~TraceLoader();


    ApiTraceCallSignature *signature(unsigned id);
    void addSignature(unsigned id, ApiTraceCallSignature *signature);

    ApiTraceEnumSignature *enumSignature(unsigned id);
    void addEnumSignature(unsigned id, ApiTraceEnumSignature *signature);

public slots:
    void loadTrace(const QString &filename);
    void loadFrame(ApiTraceFrame *frame);
    void setFrameMarker(ApiTrace::FrameMarker marker);
    void findFrameStart(ApiTraceFrame *frame);
    void findFrameEnd(ApiTraceFrame *frame);
    void findCallIndex(int index);
    void search(const ApiTrace::SearchRequest &request);

signals:
    void startedParsing();
    void parsed(int percent);
    void finishedParsing();

    void framesLoaded(const QList<ApiTraceFrame*> &frames);
    void frameContentsLoaded(ApiTraceFrame *frame,
                             const QVector<ApiTraceCall*> &calls,
                             quint64 binaryDataSize);

    void searchResult(const ApiTrace::SearchRequest &request,
                      ApiTrace::SearchResult result,
                      ApiTraceCall *call);
    void foundFrameStart(ApiTraceFrame *frame);
    void foundFrameEnd(ApiTraceFrame *frame);
    void foundCallIndex(ApiTraceCall *call);
private:
    struct FrameBookmark {
        FrameBookmark()
            : numberOfCalls(0)
        {}
        FrameBookmark(const trace::ParseBookmark &s)
            : start(s),
              numberOfCalls(0)
        {}

        trace::ParseBookmark start;
        int numberOfCalls;
    };
    bool isCallAFrameMarker(const trace::Call *call) const;
    int numberOfFrames() const;
    int numberOfCallsInFrame(int frameIdx) const;

    void loadHelpFile();
    void scanTrace();
    void parseTrace();

    void searchNext(const ApiTrace::SearchRequest &request);
    void searchPrev(const ApiTrace::SearchRequest &request);

    int callInFrame(int callIdx) const;
    bool callContains(trace::Call *call,
                      const QString &str,
                      Qt::CaseSensitivity sensitivity);
     QVector<ApiTraceCall*> fetchFrameContents(ApiTraceFrame *frame);
     bool searchCallsBackwards(const QList<trace::Call*> &calls,
                               int frameIdx,
                               const ApiTrace::SearchRequest &request);

private:
    trace::Parser m_parser;
    ApiTrace::FrameMarker m_frameMarker;

    typedef QMap<int, FrameBookmark> FrameBookmarks;
    FrameBookmarks m_frameBookmarks;
    QList<ApiTraceFrame*> m_createdFrames;

    QHash<QString, QUrl> m_helpHash;

    QVector<ApiTraceCallSignature*> m_signatures;
    QVector<ApiTraceEnumSignature*> m_enumSignatures;
};

#endif
