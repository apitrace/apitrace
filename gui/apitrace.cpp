#include "apitrace.h"

#include "loaderthread.h"
#include "saverthread.h"

#include <QDir>

ApiTrace::ApiTrace()
    : m_frameMarker(ApiTrace::FrameMarker_SwapBuffers),
      m_needsSaving(false)
{
    m_loader = new LoaderThread(this);
    connect(m_loader, SIGNAL(parsedFrames(const QList<ApiTraceFrame*>)),
            this, SLOT(addFrames(const QList<ApiTraceFrame*>)));
    connect(m_loader, SIGNAL(started()),
            this, SIGNAL(startedLoadingTrace()));
    connect(m_loader, SIGNAL(finished()),
            this, SIGNAL(finishedLoadingTrace()));

    m_saver = new SaverThread(this);
    connect(m_saver, SIGNAL(traceSaved()),
            this, SLOT(slotSaved()));
    connect(m_saver, SIGNAL(traceSaved()),
            this, SIGNAL(saved()));
}

ApiTrace::~ApiTrace()
{
    qDeleteAll(m_calls);
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
               call->name() == QLatin1String("CGLFlushDrawable");
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
    return m_calls.isEmpty();
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

QList<ApiTraceCall*> ApiTrace::calls() const
{
    return m_calls;
}

ApiTraceCall * ApiTrace::callAt(int idx) const
{
    return m_calls.value(idx);
}

int ApiTrace::numCalls() const
{
    return m_calls.count();
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

        if (m_loader->isRunning()) {
            m_loader->terminate();
            m_loader->wait();
        }
        m_frames.clear();
        m_calls.clear();
        m_errors.clear();
        m_editedCalls.clear();
        m_needsSaving = false;
        emit invalidated();

        m_loader->loadFile(m_fileName);
    }
}

void ApiTrace::setFrameMarker(FrameMarker marker)
{
    if (m_frameMarker != marker) {
        emit framesInvalidated();

        qDeleteAll(m_frames);
        m_frames.clear();
        detectFrames();
    }
}

void ApiTrace::addFrames(const QList<ApiTraceFrame*> &frames)
{
    int currentFrames = m_frames.count();
    int numNewFrames = frames.count();

    emit beginAddingFrames(currentFrames, numNewFrames);

    m_frames += frames;

    int currentCalls = m_calls.count();
    int numNewCalls = 0;
    foreach(ApiTraceFrame *frame, frames) {
        frame->setParentTrace(this);
        numNewCalls += frame->numChildren();
        m_calls += frame->calls();
    }

    emit endAddingFrames();
    emit callsAdded(currentCalls, numNewCalls);
}

void ApiTrace::detectFrames()
{
    if (m_calls.isEmpty())
        return;

    emit beginAddingFrames(0, m_frames.count());

    ApiTraceFrame *currentFrame = 0;
    foreach(ApiTraceCall *apiCall, m_calls) {
        if (!currentFrame) {
            currentFrame = new ApiTraceFrame();
            currentFrame->setParentTrace(this);
            currentFrame->number = m_frames.count();
        }
        apiCall->setParentFrame(currentFrame);
        currentFrame->addCall(apiCall);
        if (ApiTrace::isCallAFrameMarker(apiCall,
                                         m_frameMarker)) {
            m_frames.append(currentFrame);
            currentFrame = 0;
        }
    }
    //last frames won't have markers
    //  it's just a bunch of Delete calls for every object
    //  after the last SwapBuffers
    if (currentFrame) {
        m_frames.append(currentFrame);
        currentFrame = 0;
    }
    emit endAddingFrames();
}

ApiTraceCall * ApiTrace::callWithIndex(int idx) const
{
    for (int i = 0; i < m_calls.count(); ++i) {
        ApiTraceCall *call = m_calls[i];
        if (call->index() == idx)
            return call;
    }
    return NULL;
}

ApiTraceState ApiTrace::defaultState() const
{
    ApiTraceFrame *frame = frameAt(0);
    if (!frame)
        return ApiTraceState();

    return frame->state();
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
    m_saver->saveFile(m_tempFileName, m_calls);
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

#include "apitrace.moc"
