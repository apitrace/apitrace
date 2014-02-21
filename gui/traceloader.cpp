#include "traceloader.h"

#include "apitrace.h"
#include <QDebug>
#include <QFile>
#include <QStack>

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
    qDeleteAll(m_enumSignatures);
}

void TraceLoader::loadTrace(const QString &filename)
{
    if (m_helpHash.isEmpty()) {
        loadHelpFile();
    }

    if (!m_frameBookmarks.isEmpty()) {
        qDeleteAll(m_signatures);
        qDeleteAll(m_enumSignatures);
        m_signatures.clear();
        m_enumSignatures.clear();
        m_frameBookmarks.clear();
        m_createdFrames.clear();
        m_parser.close();
    }

    if (!m_parser.open(filename.toLatin1())) {
        qDebug() << "error: failed to open " << filename;
        return;
    }

    emit startedParsing();

    if (m_parser.supportsOffsets()) {
        scanTrace();
    } else {
        //Load the entire file into memory
        parseTrace();
    }
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

void TraceLoader::parseTrace()
{
    QList<ApiTraceFrame*> frames;
    ApiTraceFrame *currentFrame = 0;
    int frameCount = 0;
    QStack<ApiTraceCall*> groups;
    QVector<ApiTraceCall*> topLevelItems;
    QVector<ApiTraceCall*> allCalls;
    quint64 binaryDataSize = 0;

    int lastPercentReport = 0;

    trace::Call *call = m_parser.parse_call();
    while (call) {
        //std::cout << *call;
        if (!currentFrame) {
            currentFrame = new ApiTraceFrame();
            currentFrame->number = frameCount;
            ++frameCount;
        }
        ApiTraceCall *apiCall =
            apiCallFromTraceCall(call, m_helpHash, currentFrame, groups.isEmpty() ? 0 : groups.top(), this);
        allCalls.append(apiCall);
        if (groups.count() == 0) {
            topLevelItems.append(apiCall);
        }
        if (call->flags & trace::CALL_FLAG_MARKER_PUSH) {
            groups.push(apiCall);
        } else if (call->flags & trace::CALL_FLAG_MARKER_POP) {
            groups.top()->finishedAddingChildren();
            groups.pop();
        }
        if (!groups.isEmpty()) {
            groups.top()->addChild(apiCall);
        }
        if (apiCall->hasBinaryData()) {
            QByteArray data =
                apiCall->arguments()[apiCall->binaryDataIndex()].toByteArray();
            binaryDataSize += data.size();
        }
        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            allCalls.squeeze();
            topLevelItems.squeeze();
            if (topLevelItems.count() == allCalls.count()) {
                currentFrame->setCalls(allCalls, allCalls, binaryDataSize);
            } else {
                currentFrame->setCalls(topLevelItems, allCalls, binaryDataSize);
            }
            allCalls.clear();
            groups.clear();
            topLevelItems.clear();
            frames.append(currentFrame);
            currentFrame = 0;
            binaryDataSize = 0;
            if (frames.count() >= FRAMES_TO_CACHE) {
                emit framesLoaded(frames);
                frames.clear();
            }
            if (m_parser.percentRead() - lastPercentReport >= 5) {
                emit parsed(m_parser.percentRead());
                lastPercentReport = m_parser.percentRead();
            }
        }
        delete call;
        call = m_parser.parse_call();
    }

    //last frames won't have markers
    //  it's just a bunch of Delete calls for every object
    //  after the last SwapBuffers
    if (currentFrame) {
        allCalls.squeeze();
        if (topLevelItems.count() == allCalls.count()) {
            currentFrame->setCalls(allCalls, allCalls, binaryDataSize);
        } else {
            currentFrame->setCalls(topLevelItems, allCalls, binaryDataSize);
        }
        frames.append(currentFrame);
        currentFrame = 0;
    }
    if (frames.count()) {
        emit framesLoaded(frames);
    }
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

ApiTraceEnumSignature * TraceLoader::enumSignature(unsigned id)
{
    if (id >= m_enumSignatures.count()) {
        m_enumSignatures.resize(id + 1);
        return NULL;
    } else {
        return m_enumSignatures[id];
    }
}

void TraceLoader::addEnumSignature(unsigned id, ApiTraceEnumSignature *signature)
{
    m_enumSignatures[id] = signature;
}

void TraceLoader::searchNext(const ApiTrace::SearchRequest &request)
{
    Q_ASSERT(m_parser.supportsOffsets());
    if (m_parser.supportsOffsets()) {
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
    }
    emit searchResult(request, ApiTrace::SearchResult_NotFound, 0);
}

void TraceLoader::searchPrev(const ApiTrace::SearchRequest &request)
{
    Q_ASSERT(m_parser.supportsOffsets());
    if (m_parser.supportsOffsets()) {
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

    if (m_parser.supportsOffsets()) {
        unsigned frameIdx = currentFrame->number;
        int numOfCalls = numberOfCallsInFrame(frameIdx);

        if (numOfCalls) {
            quint64 binaryDataSize = 0;
            QStack<ApiTraceCall*> groups;
            QVector<ApiTraceCall*> topLevelItems;
            QVector<ApiTraceCall*> allCalls(numOfCalls);
            const FrameBookmark &frameBookmark = m_frameBookmarks[frameIdx];

            m_parser.setBookmark(frameBookmark.start);

            trace::Call *call;
            int parsedCalls = 0;
            while ((call = m_parser.parse_call())) {
                ApiTraceCall *apiCall =
                    apiCallFromTraceCall(call, m_helpHash,
                                         currentFrame, groups.isEmpty() ? 0 : groups.top(), this);
                Q_ASSERT(apiCall);
                Q_ASSERT(parsedCalls < allCalls.size());
                allCalls[parsedCalls++] = apiCall;
                if (groups.count() == 0) {
                    topLevelItems.append(apiCall);
                } else {
                    groups.top()->addChild(apiCall);
                }
                if (call->flags & trace::CALL_FLAG_MARKER_PUSH) {
                    groups.push(apiCall);
                } else if (call->flags & trace::CALL_FLAG_MARKER_POP) {
                    if (groups.count()) {
                        groups.top()->finishedAddingChildren();
                        groups.pop();
                    }
                }
                if (apiCall->hasBinaryData()) {
                    QByteArray data =
                        apiCall->arguments()[
                            apiCall->binaryDataIndex()].toByteArray();
                    binaryDataSize += data.size();
                }

                delete call;

                if (apiCall->flags() & trace::CALL_FLAG_END_FRAME) {
                    break;
                }

            }
            // There can be fewer parsed calls when call in different
            // threads cross the frame boundary
            Q_ASSERT(parsedCalls <= numOfCalls);
            Q_ASSERT(parsedCalls <= allCalls.size());
            allCalls.resize(parsedCalls);
            allCalls.squeeze();

            Q_ASSERT(parsedCalls <= currentFrame->numChildrenToLoad());
            if (topLevelItems.count() == allCalls.count()) {
                emit frameContentsLoaded(currentFrame, allCalls,
                                         allCalls, binaryDataSize);
            } else {
                emit frameContentsLoaded(currentFrame, topLevelItems,
                                         allCalls, binaryDataSize);
            }
            return allCalls;
        }
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

#include "traceloader.moc"
