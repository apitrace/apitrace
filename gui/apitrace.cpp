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
    connect(this, SIGNAL(loaderSearchNext(int,QString,Qt::CaseSensitivity)),
            m_loader, SLOT(searchNext(int,QString,Qt::CaseSensitivity)));
    connect(this, SIGNAL(loaderSearchPrev(int,QString,Qt::CaseSensitivity)),
            m_loader, SLOT(searchPrev(int,QString,Qt::CaseSensitivity)));
    connect(m_loader,
            SIGNAL(searchResult(ApiTrace::SearchResult,ApiTraceCall*)),
            this,
            SLOT(loaderSearchResult(ApiTrace::SearchResult,ApiTraceCall*)));
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
    if (!call)
        return false;

    switch (marker) {
    case FrameMarker_SwapBuffers:
        return call->name().contains(QLatin1String("SwapBuffers")) ||
               call->name() == QLatin1String("CGLFlushDrawable") ||
               call->name() == QLatin1String("glFrameTerminatorGREMEDY");
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
    if (edited())
        return m_tempFileName;

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
    if (frame)
        return frame->numChildren();
    else
        return 0;
}

void ApiTrace::setFileName(const QString &name)
{
    if (m_fileName != name) {
        m_fileName = name;

        m_frames.clear();
        m_errors.clear();
        m_editedCalls.clear();
        m_needsSaving = false;
        emit invalidated();

//        m_loader->loadTrace(m_fileName);
        emit loadTrace(m_fileName);
    }
}

void ApiTrace::addFrames(const QList<ApiTraceFrame*> &frames)
{
    QVector<ApiTraceCall*> calls;
    int currentFrames = m_frames.count();
    int numNewFrames = frames.count();

    emit beginAddingFrames(currentFrames, numNewFrames);

    m_frames += frames;

    int numNewCalls = 0;
    foreach(ApiTraceFrame *frame, frames) {
        frame->setParentTrace(this);
        numNewCalls += frame->numChildren();
        calls += frame->calls();
    }

    emit endAddingFrames();
}

ApiTraceCall * ApiTrace::callWithIndex(int idx) const
{
    for (int i = 0; i < m_frames.count(); ++i) {
        ApiTraceCall *call = m_frames[i]->callWithIndex(idx);
        if (call)
            return call;
    }
    return NULL;
}

ApiTraceState ApiTrace::defaultState() const
{
    ApiTraceFrame *frame = frameAt(0);
    if (!frame || !frame->hasState())
        return ApiTraceState();

    return *frame->state();
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
    Q_ASSERT(!"saving not implemented");
    m_saver->saveFile(m_tempFileName, QVector<ApiTraceCall*>());
}

void ApiTrace::slotSaved()
{
    m_needsSaving = false;
}

bool ApiTrace::isSaving() const
{
    return m_saver->isRunning();
}

void ApiTrace::callError(ApiTraceCall *call)
{
    Q_ASSERT(call);

    if (call->hasError())
        m_errors.insert(call);
    else
        m_errors.remove(call);

    emit changed(call);
}

bool ApiTrace::hasErrors() const
{
    return !m_errors.isEmpty();
}

void ApiTrace::loadFrame(ApiTraceFrame *frame)
{
    Q_ASSERT(!frame->loaded());
    emit requestFrame(frame);
}

void ApiTrace::finishedParsing()
{
    ApiTraceFrame *firstFrame = m_frames[0];
    if (firstFrame && !firstFrame->loaded()) {
        loadFrame(firstFrame);
    }
}

void ApiTrace::loaderFrameLoaded(ApiTraceFrame *frame,
                                 const QVector<ApiTraceCall*> &calls,
                                 quint64 binaryDataSize)
{
    Q_ASSERT(frame->numChildrenToLoad() == calls.size());
    emit beginLoadingFrame(frame, calls.size());
    frame->setCalls(calls, binaryDataSize);
    emit endLoadingFrame(frame);
}

void ApiTrace::findNext(ApiTraceFrame *frame,
                        ApiTraceCall *from,
                        const QString &str,
                        Qt::CaseSensitivity sensitivity)
{
    ApiTraceCall *foundCall = 0;
    int frameIdx = m_frames.indexOf(frame);

    if (frame->loaded()) {
        foundCall = frame->findNextCall(from, str, sensitivity);
        if (foundCall) {
            emit findResult(SearchFound, foundCall);
            return;
        }

        //if the frame is loaded we already searched it above
        // so skip it
        frameIdx += 1;
    }

    for (int i = frameIdx; i < m_frames.count(); ++i) {
        ApiTraceFrame *frame = m_frames[i];
        if (!frame->loaded()) {
            emit loaderSearchNext(i, str, sensitivity);
            return;
        } else {
            ApiTraceCall *call = frame->findNextCall(0, str, sensitivity);
            if (call) {
                emit findResult(SearchFound, call);
                return;
            }
        }
    }
    emit findResult(SearchWrapped, 0);
}

void ApiTrace::findPrev(ApiTraceFrame *frame,
                        ApiTraceCall *from,
                        const QString &str,
                        Qt::CaseSensitivity sensitivity)
{
    ApiTraceCall *foundCall = 0;
    int frameIdx = m_frames.indexOf(frame);

    if (frame->loaded()) {
        foundCall = frame->findPrevCall(from, str, sensitivity);
        if (foundCall) {
            emit findResult(SearchFound, foundCall);
            return;
        }

        //if the frame is loaded we already searched it above
        // so skip it
        frameIdx -= 1;
    }

    for (int i = frameIdx; i >= 0; --i) {
        ApiTraceFrame *frame = m_frames[i];
        if (!frame->loaded()) {
            emit loaderSearchPrev(i, str, sensitivity);
            return;
        } else {
            ApiTraceCall *call = frame->findPrevCall(0, str, sensitivity);
            if (call) {
                emit findResult(SearchFound, call);
                return;
            }
        }
    }
    emit findResult(SearchWrapped, 0);
}

void ApiTrace::loaderSearchResult(ApiTrace::SearchResult result,
                                  ApiTraceCall *call)
{
    //qDebug()<<"Search result = "<<result
    //       <<", call is = "<<call;
    emit findResult(result, call);
}

void ApiTrace::findFrameStart(ApiTraceFrame *frame)
{
    if (frame->loaded()) {
        emit foundFrameStart(frame);
    } else {
        emit loaderFindFrameStart(frame);
    }
}

void ApiTrace::findFrameEnd(ApiTraceFrame *frame)
{
    if (frame->loaded()) {
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
        if (frame->loaded()) {
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
        unsigned numCallsInFrame =  frame->loaded()
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

#include "apitrace.moc"
