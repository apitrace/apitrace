#include "traceloader.h"

#include "apitrace.h"
#include <QDebug>
#include <QFile>

#define FRAMES_TO_CACHE 100

static ApiTraceCall *
apiCallFromTraceCall(const Trace::Call *call,
                     const QHash<QString, QUrl> &helpHash,
                     ApiTraceFrame *frame,
                     TraceLoader *loader)
{
    ApiTraceCall *apiCall = new ApiTraceCall(frame, loader, call);

    apiCall->setHelpUrl(helpHash.value(apiCall->name()));

    return apiCall;
}

TraceLoader::TraceLoader(QObject *parent)
    : QObject(parent),
      m_frameMarker(ApiTrace::FrameMarker_SwapBuffers)
{
}

TraceLoader::~TraceLoader()
{
    m_parser.close();
}

void TraceLoader::loadTrace(const QString &filename)
{
    if (m_helpHash.isEmpty()) {
        loadHelpFile();
    }

    if (!m_parser.open(filename.toLatin1())) {
        qDebug() << "error: failed to open " << filename;
        return;
    }
    qDebug()<<"load trace with "<<filename;
    emit startedParsing();

    qDebug() <<"\t support offsets = "<<m_parser.supportsOffsets();
    if (m_parser.supportsOffsets()) {
        scanTrace();
    } else {
        //Load the entire file into memory
        parseTrace();
    }

    emit finishedParsing();
}

void TraceLoader::loadFrame(ApiTraceFrame *currentFrame)
{
    fetchFrameContents(currentFrame);
}

void TraceLoader::setFrameMarker(ApiTrace::FrameMarker marker)
{
    m_frameMarker = marker;
}

bool TraceLoader::isCallAFrameMarker(const Trace::Call *call) const
{
    std::string name = call->name();

    switch (m_frameMarker) {
    case ApiTrace::FrameMarker_SwapBuffers:
        return  name.find("SwapBuffers") != std::string::npos ||
                name == "CGLFlushDrawable" ||
                name == "glFrameTerminatorGREMEDY";
        break;
    case ApiTrace::FrameMarker_Flush:
        return name == "glFlush";
        break;
    case ApiTrace::FrameMarker_Finish:
        return name == "glFinish";
        break;
    case ApiTrace::FrameMarker_Clear:
        return name == "glClear";
        break;
    }
    return false;
}

int TraceLoader::numberOfFrames() const
{
    return m_frameBookmarks.size();
}

int TraceLoader::numberOfCallsInFrame(int frameIdx) const
{
    if (frameIdx > m_frameBookmarks.size()) {
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

    Trace::Call *call;
    Trace::ParseBookmark startBookmark;
    int numOfFrames = 0;
    int numOfCalls = 0;
    int lastPercentReport = 0;

    m_parser.getBookmark(startBookmark);

    while ((call = m_parser.scan_call())) {
        ++numOfCalls;

        if (isCallAFrameMarker(call)) {
            FrameBookmark frameBookmark(startBookmark);
            frameBookmark.numberOfCalls = numOfCalls;

            currentFrame = new ApiTraceFrame();
            currentFrame->number = numOfFrames;
            currentFrame->setNumChildren(numOfCalls);
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
        //Trace::File::Bookmark endBookmark = m_parser.currentBookmark();
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
    QVector<ApiTraceCall*> calls;
    quint64 binaryDataSize = 0;

    int lastPercentReport = 0;

    Trace::Call *call = m_parser.parse_call();
    while (call) {
        //std::cout << *call;
        if (!currentFrame) {
            currentFrame = new ApiTraceFrame();
            currentFrame->number = frameCount;
            ++frameCount;
        }
        ApiTraceCall *apiCall =
                apiCallFromTraceCall(call, m_helpHash, currentFrame, this);
        calls.append(apiCall);
        if (apiCall->hasBinaryData()) {
            QByteArray data =
                    apiCall->arguments()[apiCall->binaryDataIndex()].toByteArray();
            binaryDataSize += data.size();
        }
        if (ApiTrace::isCallAFrameMarker(apiCall,
                                         m_frameMarker)) {
            calls.squeeze();
            currentFrame->setCalls(calls, binaryDataSize);
            calls.clear();
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
        calls.squeeze();
        currentFrame->setCalls(calls, binaryDataSize);
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

void TraceLoader::searchNext(int startFrame,
                             const QString &str,
                             Qt::CaseSensitivity sensitivity)
{
    Q_ASSERT(m_parser.supportsOffsets());
    if (m_parser.supportsOffsets()) {
        const FrameBookmark &frameBookmark = m_frameBookmarks[startFrame];
        m_parser.setBookmark(frameBookmark.start);
        Trace::Call *call = 0;
        while ((call = m_parser.parse_call())) {

            if (callContains(call, str, sensitivity)) {
                unsigned frameIdx = callInFrame(call->no);
                ApiTraceFrame *frame = m_createdFrames[frameIdx];
                const QVector<ApiTraceCall*> calls =
                        fetchFrameContents(frame);
                for (int i = 0; i < calls.count(); ++i) {
                    if (calls[i]->index() == call->no) {
                        emit searchResult(ApiTrace::SearchFound, calls[i]);
                        break;
                    }
                }
                delete call;
                return;
            }

            delete call;
        }
    }
    emit searchResult(ApiTrace::SearchNotFound, 0);
}

void TraceLoader::searchPrev(int startFrame,
                             const QString &str,
                             Qt::CaseSensitivity sensitivity)
{
}

int TraceLoader::callInFrame(int callIdx) const
{
    unsigned numCalls = 0;

    for (int frameIdx = 0; frameIdx <= m_frameBookmarks.size(); ++frameIdx) {
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

bool TraceLoader::callContains(Trace::Call *call,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity)
{
    /*
     * FIXME: do string comparison directly on Trace::Call
     */
    ApiTraceCall *apiCall = apiCallFromTraceCall(call, m_helpHash,
                                                 0, this);
    bool result = apiCall->contains(str, sensitivity);
    delete apiCall;
    return result;
}

QVector<ApiTraceCall*>
TraceLoader::fetchFrameContents(ApiTraceFrame *currentFrame)
{
    Q_ASSERT(currentFrame);
    if (m_parser.supportsOffsets()) {
        unsigned frameIdx = currentFrame->number;
        int numOfCalls = numberOfCallsInFrame(frameIdx);

        if (numOfCalls) {
            quint64 binaryDataSize = 0;
            QVector<ApiTraceCall*> calls(numOfCalls);
            const FrameBookmark &frameBookmark = m_frameBookmarks[frameIdx];

            m_parser.setBookmark(frameBookmark.start);

            Trace::Call *call;
            int parsedCalls = 0;
            while ((call = m_parser.parse_call())) {
                ApiTraceCall *apiCall =
                    apiCallFromTraceCall(call, m_helpHash,
                                         currentFrame, this);
                calls[parsedCalls] = apiCall;
                Q_ASSERT(calls[parsedCalls]);
                if (apiCall->hasBinaryData()) {
                    QByteArray data =
                        apiCall->arguments()[
                            apiCall->binaryDataIndex()].toByteArray();
                    binaryDataSize += data.size();
                }

                ++parsedCalls;

                delete call;

                if (ApiTrace::isCallAFrameMarker(apiCall, m_frameMarker)) {
                    break;
                }

            }
            assert(parsedCalls == numOfCalls);
            Q_ASSERT(parsedCalls == calls.size());
            calls.squeeze();

            Q_ASSERT(parsedCalls == currentFrame->numChildrenToLoad());
            emit frameContentsLoaded(currentFrame,
                                     calls, binaryDataSize);
            return calls;
        }
    }
    return QVector<ApiTraceCall*>();
}

#include "traceloader.moc"
