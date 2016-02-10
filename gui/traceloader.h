#pragma once


#include "apitrace.h"
#include "trace_file.hpp"
#include "trace_parser.hpp"

#include <QObject>
#include <QList>
#include <QMap>
#include <QStack>

class TraceLoader : public QObject
{
    Q_OBJECT
public:
    TraceLoader(QObject *parent=0);
    ~TraceLoader();


    ApiTraceCallSignature *signature(unsigned id);
    void addSignature(unsigned id, ApiTraceCallSignature *signature);

    trace::EnumSig *enumSignature(unsigned id);

private:
    class FrameContents
    {
    public:
        FrameContents(int numOfCalls=0);

        bool load(TraceLoader *loader, ApiTraceFrame* frame,
                  QHash<QString, QUrl> helpHash, trace::Parser &parser);
        void reset();
        int  topLevelCount()      const;
        int  allCallsCount()      const;
        QVector<ApiTraceCall*> topLevelCalls() const;
        QVector<ApiTraceCall*> allCalls()      const;
        quint64 binaryDataSize()  const;
        bool isEmpty();

    private:
        QStack <ApiTraceCall*> m_groups;
        QVector<ApiTraceCall*> m_topLevelItems;
        QVector<ApiTraceCall*> m_allCalls;
        quint64 m_binaryDataSize;
        int     m_parsedCalls;
    };

public slots:
    void loadTrace(const QString &filename);
    void loadFrame(ApiTraceFrame *frame);
    void findFrameStart(ApiTraceFrame *frame);
    void findFrameEnd(ApiTraceFrame *frame);
    void findCallIndex(int index);
    void search(const ApiTrace::SearchRequest &request);

signals:
    void parseProblem(const QString &message);
    void startedParsing();
    void parsed(int percent);
    void guessedApi(int api);
    void finishedParsing();

    void framesLoaded(const QList<ApiTraceFrame*> &frames);
    void frameContentsLoaded(ApiTraceFrame *frame,
                             const QVector<ApiTraceCall*> &topLevelItems,
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
    int numberOfFrames() const;
    int numberOfCallsInFrame(int frameIdx) const;

    void loadHelpFile();
    void guessApi(const trace::Call *call);
    void scanTrace();

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

    typedef QMap<int, FrameBookmark> FrameBookmarks;
    FrameBookmarks m_frameBookmarks;
    QList<ApiTraceFrame*> m_createdFrames;

    QHash<QString, QUrl> m_helpHash;

    QVector<ApiTraceCallSignature*> m_signatures;
};
