#include "apitrace.h"

#include "traceloader.h"
#include "saverthread.h"

#include <QDebug>
#include <QDir>
#include <QThread>

ApiTrace::ApiTrace()
    : m_frameMarker(ApiTrace::FrameMarker_SwapBuffers),
      m_needsSaving(false)
{
    m_loader = new TraceLoader();

    connect(this, SIGNAL(loadTrace(QString)),
            m_loader, SLOT(loadTrace(QString)));
    connect(this, SIGNAL(requestFrame(ApiTraceFrame*)),
            m_loader, SLOT(loadFrame(ApiTraceFrame*)));
    connect(m_loader, SIGNAL(framesLoaded(const QList<ApiTraceFrame*>)),
            this, SLOT(addFrames(const QList<ApiTraceFrame*>)));
    connect(m_loader,
            SIGNAL(frameContentsLoaded(ApiTraceFrame*,QVector<ApiTraceCall*>,quint64)),
            this,
            SLOT(loaderFrameLoaded(ApiTraceFrame*,QVector<ApiTraceCall*>,quint64)));
    connect(m_loader, SIGNAL(finishedParsing()),
            this, SLOT(finishedParsing()));
    connect(this, SIGNAL(loaderSearch(ApiTrace::SearchRequest)),
            m_loader, SLOT(search(ApiTrace::SearchRequest)));
    connect(m_loader,
            SIGNAL(searchResult(ApiTrace::SearchRequest,ApiTrace::SearchResult,ApiTraceCall*)),
            this,
            SLOT(loaderSearchResult(ApiTrace::SearchRequest,ApiTrace::SearchResult,ApiTraceCall*)));
    connect(this, SIGNAL(loaderFindFrameStart(ApiTraceFrame*)),
            m_loader, SLOT(findFrameStart(ApiTraceFrame*)));
    connect(this, SIGNAL(loaderFindFrameEnd(ApiTraceFrame*)),
            m_loader, SLOT(findFrameEnd(ApiTraceFrame*)));
    connect(m_loader, SIGNAL(foundFrameStart(ApiTraceFrame*)),
            this, SIGNAL(foundFrameStart(ApiTraceFrame*)));
    connect(m_loader, SIGNAL(foundFrameEnd(ApiTraceFrame*)),
            this, SIGNAL(foundFrameEnd(ApiTraceFrame*)));
    connect(this, SIGNAL(loaderFindCallIndex(int)),
            m_loader, SLOT(findCallIndex(int)));
    connect(m_loader, SIGNAL(foundCallIndex(ApiTraceCall*)),
            this, SIGNAL(foundCallIndex(ApiTraceCall*)));


    connect(m_loader, SIGNAL(startedParsing()),
            this, SIGNAL(startedLoadingTrace()));
    connect(m_loader, SIGNAL(parsed(int)),
            this, SIGNAL(loaded(int)));
    connect(m_loader, SIGNAL(finishedParsing()),
            this, SIGNAL(finishedLoadingTrace()));


    m_saver = new SaverThread(this);
    connect(m_saver, SIGNAL(traceSaved()),
            this, SLOT(slotSaved()));
    connect(m_saver, SIGNAL(traceSaved()),
            this, SIGNAL(saved()));

    m_loaderThread = new QThread();
    m_loader->moveToThread(m_loaderThread);
    m_loaderThread->start();
}

ApiTrace::~ApiTrace()
{
    m_loaderThread->quit();
    m_loaderThread->deleteLater();
    qDeleteAll(m_frames);
    delete m_loader;
    delete m_saver;
}

bool ApiTrace::isCallAFrameMarker(const ApiTraceCall *call,
                                  ApiTrace::FrameMarker marker)
{
    if (!call) {
        return false;
    }

    switch (marker) {
    case FrameMarker_SwapBuffers:
        return call->flags() & trace::CALL_FLAG_END_FRAME;
    case FrameMarker_Flush:
        return call->name() == QLatin1String("glFlush");
    case FrameMarker_Finish:
        return call->name() == QLatin1String("glFinish");
    case FrameMarker_Clear:
        return call->name() == QLatin1String("glClear");
    }

    Q_ASSERT(!"unknown frame marker");

    return false;
}

bool ApiTrace::isEmpty() const
{
    return m_frames.isEmpty();
}

QString ApiTrace::fileName() const
{
    if (edited()) {
        return m_tempFileName;
    }

    return m_fileName;
}

ApiTrace::FrameMarker ApiTrace::frameMarker() const
{
    return m_frameMarker;
}

QList<ApiTraceFrame*> ApiTrace::frames() const
{
    return m_frames;
}

ApiTraceFrame * ApiTrace::frameAt(int idx) const
{
    return m_frames.value(idx);
}

int ApiTrace::numFrames() const
{
    return m_frames.count();
}

int ApiTrace::numCallsInFrame(int idx) const
{
    const ApiTraceFrame *frame = frameAt(idx);
    if (frame) {
        return frame->numChildren();
    } else {
        return 0;
    }
}

void ApiTrace::setFileName(const QString &name)
{
    if (m_fileName != name) {
        m_fileName = name;
        m_tempFileName = QString();

        m_frames.clear();
        m_errors.clear();
        m_editedCalls.clear();
        m_queuedErrors.clear();
        m_needsSaving = false;
        emit invalidated();

        emit loadTrace(m_fileName);
    }
}

void ApiTrace::addFrames(const QList<ApiTraceFrame*> &frames)
{
    int currentFrames = m_frames.count();
    int numNewFrames = frames.count();

    emit beginAddingFrames(currentFrames, numNewFrames);

    m_frames += frames;

    foreach(ApiTraceFrame *frame, frames) {
        frame->setParentTrace(this);
    }

    emit endAddingFrames();
}

ApiTraceCall * ApiTrace::callWithIndex(int idx) const
{
    for (int i = 0; i < m_frames.count(); ++i) {
        ApiTraceCall *call = m_frames[i]->callWithIndex(idx);
        if (call) {
            return call;
        }
    }
    return NULL;
}

ApiTraceState ApiTrace::defaultState() const
{
    ApiTraceFrame *frame = frameAt(0);
    if (!frame || !frame->isLoaded() || frame->isEmpty()) {
        return ApiTraceState();
    }

    ApiTraceCall *firstCall = frame->calls().first();
    if (!firstCall->hasState()) {
        return ApiTraceState();
    }

    return *firstCall->state();
}

void ApiTrace::callEdited(ApiTraceCall *call)
{
    if (!m_editedCalls.contains(call)) {
        //lets generate a temp filename
        QString tempPath = QDir::tempPath();
        m_tempFileName = QString::fromLatin1("%1/%2.edited")
                         .arg(tempPath)
                         .arg(m_fileName);
    }
    m_editedCalls.insert(call);
    m_needsSaving = true;

    emit changed(call);
}

void ApiTrace::callReverted(ApiTraceCall *call)
{
    m_editedCalls.remove(call);

    if (m_editedCalls.isEmpty()) {
        m_needsSaving = false;
    }
    emit changed(call);
}

bool ApiTrace::edited() const
{
    return !m_editedCalls.isEmpty();
}

bool ApiTrace::needsSaving() const
{
    return m_needsSaving;
}

void ApiTrace::save()
{
    QFileInfo fi(m_tempFileName);
    QDir dir;
    emit startedSaving();
    dir.mkpath(fi.absolutePath());
    m_saver->saveFile(m_tempFileName,
                      m_fileName,
                      m_editedCalls);
}

void ApiTrace::slotSaved()
{
    m_needsSaving = false;
}

bool ApiTrace::isSaving() const
{
    return m_saver->isRunning();
}

bool ApiTrace::hasErrors() const
{
    return !m_errors.isEmpty();
}

void ApiTrace::loadFrame(ApiTraceFrame *frame)
{
    if (!isFrameLoading(frame)) {
        Q_ASSERT(!frame->isLoaded());
        m_loadingFrames.insert(frame);
        emit requestFrame(frame);
    }
}

void ApiTrace::finishedParsing()
{
    ApiTraceFrame *firstFrame = m_frames[0];
    if (firstFrame && !firstFrame->isLoaded()) {
        loadFrame(firstFrame);
    }
}

void ApiTrace::loaderFrameLoaded(ApiTraceFrame *frame,
                                 const QVector<ApiTraceCall*> &calls,
                                 quint64 binaryDataSize)
{
    Q_ASSERT(frame->numChildrenToLoad() == calls.size());

    if (!frame->isLoaded()) {
        emit beginLoadingFrame(frame, calls.size());
        frame->setCalls(calls, binaryDataSize);
        emit endLoadingFrame(frame);
        m_loadingFrames.remove(frame);
    }

    if (!m_queuedErrors.isEmpty()) {
        QList< QPair<ApiTraceFrame*, ApiTraceError> >::iterator itr;
        for (itr = m_queuedErrors.begin(); itr != m_queuedErrors.end();
             ++itr) {
            const ApiTraceError &error = (*itr).second;
            if ((*itr).first == frame) {
                ApiTraceCall *call = frame->callWithIndex(error.callIndex);

                if (!call) {
                    continue;
                }

                call->setError(error.message);
                m_queuedErrors.erase(itr);

                if (call->hasError()) {
                    m_errors.insert(call);
                } else {
                    m_errors.remove(call);
                }
                emit changed(call);
            }
        }
    }
}

void ApiTrace::findNext(ApiTraceFrame *frame,
                        ApiTraceCall *from,
                        const QString &str,
                        Qt::CaseSensitivity sensitivity)
{
    ApiTraceCall *foundCall = 0;
    int frameIdx = m_frames.indexOf(frame);
    SearchRequest request(SearchRequest::Next,
                          frame, from, str, sensitivity);

    if (frame->isLoaded()) {
        foundCall = frame->findNextCall(from, str, sensitivity);
        if (foundCall) {
            emit findResult(request, SearchResult_Found, foundCall);
            return;
        }

        //if the frame is loaded we already searched it above
        // so skip it
        frameIdx += 1;
    }

    //for the rest of the frames we search from beginning
    request.from = 0;
    for (int i = frameIdx; i < m_frames.count(); ++i) {
        ApiTraceFrame *frame = m_frames[i];
        request.frame = frame;
        if (!frame->isLoaded()) {
            emit loaderSearch(request);
            return;
        } else {
            ApiTraceCall *call = frame->findNextCall(0, str, sensitivity);
            if (call) {
                emit findResult(request, SearchResult_Found, call);
                return;
            }
        }
    }
    emit findResult(request, SearchResult_Wrapped, 0);
}

void ApiTrace::findPrev(ApiTraceFrame *frame,
                        ApiTraceCall *from,
                        const QString &str,
                        Qt::CaseSensitivity sensitivity)
{
    ApiTraceCall *foundCall = 0;
    int frameIdx = m_frames.indexOf(frame);
    SearchRequest request(SearchRequest::Prev,
                          frame, from, str, sensitivity);

    if (frame->isLoaded()) {
        foundCall = frame->findPrevCall(from, str, sensitivity);
        if (foundCall) {
            emit findResult(request, SearchResult_Found, foundCall);
            return;
        }

        //if the frame is loaded we already searched it above
        // so skip it
        frameIdx -= 1;
    }

    request.from = 0;
    for (int i = frameIdx; i >= 0; --i) {
        ApiTraceFrame *frame = m_frames[i];
        request.frame = frame;
        if (!frame->isLoaded()) {
            emit loaderSearch(request);
            return;
        } else {
            ApiTraceCall *call = frame->findPrevCall(0, str, sensitivity);
            if (call) {
                emit findResult(request, SearchResult_Found, call);
                return;
            }
        }
    }
    emit findResult(request, SearchResult_Wrapped, 0);
}

void ApiTrace::loaderSearchResult(const ApiTrace::SearchRequest &request,
                                  ApiTrace::SearchResult result,
                                  ApiTraceCall *call)
{
    //qDebug()<<"Search result = "<<result
    //       <<", call is = "<<call;
    emit findResult(request, result, call);
}

void ApiTrace::findFrameStart(ApiTraceFrame *frame)
{
    if (frame->isLoaded()) {
        emit foundFrameStart(frame);
    } else {
        emit loaderFindFrameStart(frame);
    }
}

void ApiTrace::findFrameEnd(ApiTraceFrame *frame)
{
    if (frame->isLoaded()) {
        emit foundFrameEnd(frame);
    } else {
        emit loaderFindFrameEnd(frame);
    }
}

void ApiTrace::findCallIndex(int index)
{
    int frameIdx = callInFrame(index);
    ApiTraceFrame *frame = 0;

    if (frameIdx < 0) {
        emit foundCallIndex(0);
        return;
    }

    frame = m_frames[frameIdx];

    if (frame) {
        if (frame->isLoaded()) {
            ApiTraceCall *call = frame->callWithIndex(index);
            emit foundCallIndex(call);
        } else {
            emit loaderFindCallIndex(index);
        }
    }
}

int ApiTrace::callInFrame(int callIdx) const
{
    unsigned numCalls = 0;

    for (int frameIdx = 0; frameIdx <= m_frames.size(); ++frameIdx) {
        const ApiTraceFrame *frame = m_frames[frameIdx];
        unsigned numCallsInFrame =  frame->isLoaded()
                ? frame->numChildren()
                : frame->numChildrenToLoad();
        unsigned firstCall = numCalls;
        unsigned endCall = numCalls + numCallsInFrame;
        if (firstCall <= callIdx && endCall > callIdx) {
            return frameIdx;
        }
        numCalls = endCall;
    }

    return -1;
}

void ApiTrace::setCallError(const ApiTraceError &error)
{
    int frameIdx = callInFrame(error.callIndex);
    ApiTraceFrame *frame = 0;

    if (frameIdx < 0) {
        return;
    }
    frame = m_frames[frameIdx];

    if (frame->isLoaded()) {
        ApiTraceCall *call = frame->callWithIndex(error.callIndex);
        call->setError(error.message);
        if (call->hasError()) {
            m_errors.insert(call);
        } else {
            m_errors.remove(call);
        }
        emit changed(call);
    } else {
        loadFrame(frame);
        m_queuedErrors.append(qMakePair(frame, error));
    }
}

bool ApiTrace::isFrameLoading(ApiTraceFrame *frame) const
{
    return m_loadingFrames.contains(frame);
}

#include "apitrace.moc"
