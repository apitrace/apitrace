#include "traceloader.h"

#include "apitrace.h"
#include <QDebug>
#include <QFile>

#define FRAMES_TO_CACHE 100

static ApiTraceCall *
apiCallFromTraceCall(const trace::Call *call,
                     const QHash<QString, QUrl> &helpHash,
                     ApiTraceFrame *frame,
                     ApiTraceCall *parentCall,
                     TraceLoader *loader)
{
    ApiTraceCall *apiCall;

    if (parentCall)
        apiCall = new ApiTraceCall(parentCall, loader, call);
    else
        apiCall = new ApiTraceCall(frame, loader, call);

    apiCall->setHelpUrl(helpHash.value(apiCall->name()));

    return apiCall;
}

TraceLoader::TraceLoader(QObject *parent)
    : QObject(parent)
{
}

TraceLoader::~TraceLoader()
{
    m_parser.close();
    qDeleteAll(m_signatures);
}

void TraceLoader::loadTrace(const QString &filename)
{
    if (m_helpHash.isEmpty()) {
        loadHelpFile();
    }

    if (!m_frameBookmarks.isEmpty()) {
        qDeleteAll(m_signatures);
        m_signatures.clear();
        m_frameBookmarks.clear();
        m_createdFrames.clear();
        m_parser.close();
    }

    if (!m_parser.open(filename.toLatin1())) {
        qDebug() << "error: failed to open " << filename;
        return;
    }

    if (!m_parser.supportsOffsets()) {
        emit parseProblem(
            "This trace in compressed in a format that does not allow random seeking.\n"
            "Please repack the trace with `apitrace repack`."
        );
        m_parser.close();
        return;
    }

    emit startedParsing();

    scanTrace();

    emit guessedApi(static_cast<int>(m_parser.api));
    emit finishedParsing();
}

void TraceLoader::loadFrame(ApiTraceFrame *currentFrame)
{
    fetchFrameContents(currentFrame);
}

int TraceLoader::numberOfFrames() const
{
    return m_frameBookmarks.size();
}

int TraceLoader::numberOfCallsInFrame(int frameIdx) const
{
    if (frameIdx >= m_frameBookmarks.size()) {
        return 0;
    }
    FrameBookmarks::const_iterator itr =
            m_frameBookmarks.find(frameIdx);
    return itr->numberOfCalls;
}

void TraceLoader::loadHelpFile()
{
    QFile file(":/resources/glreference.tsv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString line;
        while (!file.atEnd()) {
            line = file.readLine();
            QString function = line.section('\t', 0, 0).trimmed();
            QUrl url = QUrl(line.section('\t', 1, 1).trimmed());
            //qDebug()<<"function = "<<function<<", url = "<<url.toString();
            m_helpHash.insert(function, url);
        }
    } else {
        qWarning() << "Couldn't open reference file "
                   << file.fileName();
    }
    file.close();
}

void TraceLoader::scanTrace()
{
    QList<ApiTraceFrame*> frames;
    ApiTraceFrame *currentFrame = 0;

    trace::Call *call;
    trace::ParseBookmark startBookmark;
    int numOfFrames = 0;
    int numOfCalls = 0;
    int lastPercentReport = 0;

    m_parser.getBookmark(startBookmark);

    while ((call = m_parser.scan_call())) {
        ++numOfCalls;

        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            FrameBookmark frameBookmark(startBookmark);
            frameBookmark.numberOfCalls = numOfCalls;

            currentFrame = new ApiTraceFrame();
            currentFrame->number = numOfFrames;
            currentFrame->setNumChildren(numOfCalls);
            currentFrame->setLastCallIndex(call->no);
            frames.append(currentFrame);

            m_createdFrames.append(currentFrame);
            m_frameBookmarks[numOfFrames] = frameBookmark;
            ++numOfFrames;

            if (m_parser.percentRead() - lastPercentReport >= 5) {
                emit parsed(m_parser.percentRead());
                lastPercentReport = m_parser.percentRead();
            }
            m_parser.getBookmark(startBookmark);
            numOfCalls = 0;
        }
        delete call;
    }

    if (numOfCalls) {
        //trace::File::Bookmark endBookmark = m_parser.currentBookmark();
        FrameBookmark frameBookmark(startBookmark);
        frameBookmark.numberOfCalls = numOfCalls;

        currentFrame = new ApiTraceFrame();
        currentFrame->number = numOfFrames;
        currentFrame->setNumChildren(numOfCalls);
        frames.append(currentFrame);

        m_createdFrames.append(currentFrame);
        m_frameBookmarks[numOfFrames] = frameBookmark;
        ++numOfFrames;
    }

    emit parsed(100);

    emit framesLoaded(frames);
}


ApiTraceCallSignature * TraceLoader::signature(unsigned id)
{
    if (id >= m_signatures.count()) {
        m_signatures.resize(id + 1);
        return NULL;
    } else {
        return m_signatures[id];
    }
}

void TraceLoader::addSignature(unsigned id, ApiTraceCallSignature *signature)
{
    m_signatures[id] = signature;
}

void TraceLoader::searchNext(const ApiTrace::SearchRequest &request)
{
    Q_ASSERT(m_parser.supportsOffsets());
    int startFrame = m_createdFrames.indexOf(request.frame);
    const FrameBookmark &frameBookmark = m_frameBookmarks[startFrame];
    m_parser.setBookmark(frameBookmark.start);
    trace::Call *call = 0;
    while ((call = m_parser.parse_call())) {

        if (callContains(call, request.text, request.cs)) {
            unsigned frameIdx = callInFrame(call->no);
            ApiTraceFrame *frame = m_createdFrames[frameIdx];
            const QVector<ApiTraceCall*> calls =
                    fetchFrameContents(frame);
            for (int i = 0; i < calls.count(); ++i) {
                if (calls[i]->index() == call->no) {
                    emit searchResult(request, ApiTrace::SearchResult_Found,
                                      calls[i]);
                    break;
                }
            }
            delete call;
            return;
        }

        delete call;
    }
    emit searchResult(request, ApiTrace::SearchResult_NotFound, 0);
}

void TraceLoader::searchPrev(const ApiTrace::SearchRequest &request)
{
    Q_ASSERT(m_parser.supportsOffsets());
    int startFrame = m_createdFrames.indexOf(request.frame);
    trace::Call *call = 0;
    QList<trace::Call*> frameCalls;
    int frameIdx = startFrame;

    const FrameBookmark &frameBookmark = m_frameBookmarks[frameIdx];
    int numCallsToParse = frameBookmark.numberOfCalls;
    m_parser.setBookmark(frameBookmark.start);

    while ((call = m_parser.parse_call())) {

        frameCalls.append(call);
        --numCallsToParse;

        if (numCallsToParse == 0) {
            bool foundCall = searchCallsBackwards(frameCalls,
                                                  frameIdx,
                                                  request);

            qDeleteAll(frameCalls);
            frameCalls.clear();
            if (foundCall) {
                return;
            }

            --frameIdx;

            if (frameIdx >= 0) {
                const FrameBookmark &frameBookmark =
                        m_frameBookmarks[frameIdx];
                m_parser.setBookmark(frameBookmark.start);
                numCallsToParse = frameBookmark.numberOfCalls;
            }
        }
    }
    emit searchResult(request, ApiTrace::SearchResult_NotFound, 0);
}

bool TraceLoader::searchCallsBackwards(const QList<trace::Call*> &calls,
                                       int frameIdx,
                                       const ApiTrace::SearchRequest &request)
{
    for (int i = calls.count() - 1; i >= 0; --i) {
        trace::Call *call = calls[i];
        if (callContains(call, request.text, request.cs)) {
            ApiTraceFrame *frame = m_createdFrames[frameIdx];
            const QVector<ApiTraceCall*> apiCalls =
                    fetchFrameContents(frame);
            for (int i = 0; i < apiCalls.count(); ++i) {
                if (apiCalls[i]->index() == call->no) {
                    emit searchResult(request,
                                      ApiTrace::SearchResult_Found,
                                      apiCalls[i]);
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

int TraceLoader::callInFrame(int callIdx) const
{
    unsigned numCalls = 0;

    for (int frameIdx = 0; frameIdx < m_frameBookmarks.size(); ++frameIdx) {
        const FrameBookmark &frameBookmark = m_frameBookmarks[frameIdx];
        unsigned firstCall = numCalls;
        unsigned endCall = numCalls + frameBookmark.numberOfCalls;
        if (firstCall <= callIdx && endCall > callIdx) {
            return frameIdx;
        }
        numCalls = endCall;
    }
    Q_ASSERT(!"call not in the trace");
    return 0;
}

bool TraceLoader::callContains(trace::Call *call,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity)
{
    /*
     * FIXME: do string comparison directly on trace::Call
     */
    ApiTraceCall *apiCall = apiCallFromTraceCall(call, m_helpHash,
                                                 0, 0, this);
    bool result = apiCall->contains(str, sensitivity);
    delete apiCall;
    return result;
}

QVector<ApiTraceCall*>
TraceLoader::fetchFrameContents(ApiTraceFrame *currentFrame)
{
    Q_ASSERT(currentFrame);

    if (currentFrame->isLoaded()) {
        return currentFrame->calls();
    }

    unsigned frameIdx = currentFrame->number;
    int numOfCalls = numberOfCallsInFrame(frameIdx);

    if (numOfCalls) {
        const FrameBookmark &frameBookmark = m_frameBookmarks[frameIdx];

        m_parser.setBookmark(frameBookmark.start);

        FrameContents frameCalls(numOfCalls);
        frameCalls.load(this, currentFrame, m_helpHash, m_parser);
        if (frameCalls.topLevelCount() == frameCalls.allCallsCount()) {
            emit frameContentsLoaded(currentFrame,
                                     frameCalls.allCalls(),
                                     frameCalls.allCalls(),
                                     frameCalls.binaryDataSize());
        } else {
            emit frameContentsLoaded(currentFrame,
                                     frameCalls.topLevelCalls(),
                                     frameCalls.allCalls(),
                                     frameCalls.binaryDataSize());
        }
        return frameCalls.allCalls();
    }
    return QVector<ApiTraceCall*>();
}

void TraceLoader::findFrameStart(ApiTraceFrame *frame)
{
    if (!frame->isLoaded()) {
        loadFrame(frame);
    }
    emit foundFrameStart(frame);
}

void TraceLoader::findFrameEnd(ApiTraceFrame *frame)
{
    if (!frame->isLoaded()) {
        loadFrame(frame);
    }
    emit foundFrameEnd(frame);
}

void TraceLoader::findCallIndex(int index)
{
    int frameIdx = callInFrame(index);
    ApiTraceFrame *frame = m_createdFrames[frameIdx];
    QVector<ApiTraceCall*> calls = fetchFrameContents(frame);
    QVector<ApiTraceCall*>::const_iterator itr;
    ApiTraceCall *call = 0;
    for (itr = calls.constBegin(); itr != calls.constEnd(); ++itr) {
        if ((*itr)->index() == index) {
            call = *itr;
            break;
        }
    }
    if (call) {
        emit foundCallIndex(call);
    }
}

void TraceLoader::search(const ApiTrace::SearchRequest &request)
{
    if (request.direction == ApiTrace::SearchRequest::Next) {
        searchNext(request);
    } else {
        searchPrev(request);
    }
}

TraceLoader::FrameContents::FrameContents(int numOfCalls)
    : m_allCalls(numOfCalls),
      m_binaryDataSize(0),
      m_parsedCalls(0)
{}


void
TraceLoader::FrameContents::reset()
{
    m_groups.clear();
    m_allCalls.clear();
    m_topLevelItems.clear();
    m_binaryDataSize = 0;
}

int
TraceLoader::FrameContents::topLevelCount() const
{
    return m_topLevelItems.count();
}

int
TraceLoader::FrameContents::allCallsCount() const
{
    return m_allCalls.count();
}

quint64
TraceLoader::FrameContents::binaryDataSize() const
{
    return m_binaryDataSize;
}
QVector<ApiTraceCall*>
TraceLoader::FrameContents::topLevelCalls() const
{
    return m_topLevelItems;
}

QVector<ApiTraceCall*>
TraceLoader::FrameContents::allCalls() const
{
    return m_allCalls;
}

bool
TraceLoader::FrameContents::isEmpty()
{
    return (m_allCalls.count() == 0);
}

bool
TraceLoader::FrameContents::load(TraceLoader   *loader,
                               ApiTraceFrame *currentFrame, 
                               QHash<QString, QUrl> helpHash,
                               trace::Parser &parser)
{
    bool bEndFrameReached = false;
    int initNumOfCalls = m_allCalls.count();
    trace::Call  *call;
    ApiTraceCall *apiCall = NULL;

    while ((call = parser.parse_call())) {

        apiCall = apiCallFromTraceCall(call, helpHash, currentFrame,
                                       m_groups.isEmpty() ? 0 : m_groups.top(),
                                       loader);
        Q_ASSERT(apiCall);
        if (initNumOfCalls) {
            Q_ASSERT(m_parsedCalls < m_allCalls.size());
            m_allCalls[m_parsedCalls++] = apiCall;
        } else {
            m_allCalls.append(apiCall);
        }
        if (m_groups.count() == 0) {
            m_topLevelItems.append(apiCall);
        } else {
            m_groups.top()->addChild(apiCall);
        }
        if (call->flags & trace::CALL_FLAG_MARKER_PUSH) {
            m_groups.push(apiCall);
        } else if (call->flags & trace::CALL_FLAG_MARKER_POP) {
            if (m_groups.count()) {
                m_groups.top()->finishedAddingChildren();
                m_groups.pop();
            }
        }
        if (apiCall->hasBinaryData()) {
            QByteArray data =
                apiCall->arguments()[apiCall->binaryDataIndex()].
                                                  toByteArray();
            m_binaryDataSize += data.size();
        }

        delete call;

        if (apiCall->flags() & trace::CALL_FLAG_END_FRAME) {
            bEndFrameReached = true;
            break;
        }
    }
    if (initNumOfCalls) {
        // There can be fewer parsed calls when call in different
        // threads cross the frame boundary
        Q_ASSERT(m_parsedCalls <= initNumOfCalls);
        Q_ASSERT(m_parsedCalls <= m_allCalls.size());
        m_allCalls.resize(m_parsedCalls);
        Q_ASSERT(m_parsedCalls <= currentFrame->numChildrenToLoad());
    }
    m_allCalls.squeeze();
    m_topLevelItems.squeeze();

    return bEndFrameReached;
}

#include "traceloader.moc"
