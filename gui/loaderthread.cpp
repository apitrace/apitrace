#include "loaderthread.h"

#include "trace_parser.hpp"

#define FRAMES_TO_CACHE 100

static ApiTraceCall *
apiCallFromTraceCall(const Trace::Call *call)
{
    ApiTraceCall *apiCall = new ApiTraceCall();
    apiCall->name = QString::fromStdString(call->sig->name);
    apiCall->index = call->no;

    QString argumentsText;
    for (int i = 0; i < call->sig->arg_names.size(); ++i) {
        apiCall->argNames +=
            QString::fromStdString(call->sig->arg_names[i]);
    }
    if (call->ret) {
        VariantVisitor retVisitor;
        call->ret->visit(retVisitor);
        apiCall->returnValue = retVisitor.variant();
    }
    for (int i = 0; i < call->args.size(); ++i) {
        VariantVisitor argVisitor;
        call->args[i]->visit(argVisitor);
        apiCall->argValues += argVisitor.variant();
    }

    return apiCall;
}

LoaderThread::LoaderThread(QObject *parent)
    : QThread(parent),
      m_frameMarker(ApiTrace::FrameMarker_SwapBuffers)
{
}

void LoaderThread::run()
{
    QList<ApiTraceFrame*> frames;
    ApiTraceFrame *currentFrame = 0;
    int frameCount = 0;
    Trace::Parser p;
    if (p.open(m_fileName.toLatin1().constData())) {
        Trace::Call *call;
        call = p.parse_call();
        while (call) {
            //std::cout << *call;
            if (!currentFrame) {
                currentFrame = new ApiTraceFrame();
                currentFrame->number = frameCount;
                ++frameCount;
            }
            ApiTraceCall *apiCall =
                apiCallFromTraceCall(call);
            apiCall->parentFrame = currentFrame;
            currentFrame->calls.append(apiCall);
            if (ApiTrace::isCallAFrameMarker(apiCall,
                                             m_frameMarker)) {
                frames.append(currentFrame);
                currentFrame = 0;
                if (frames.count() >= FRAMES_TO_CACHE) {
                    emit parsedFrames(frames);
                    frames.clear();
                }
            }
            call = p.parse_call();
        }
    }
    //last frames won't have markers
    //  it's just a bunch of Delete calls for every object
    //  after the last SwapBuffers
    if (currentFrame) {
        frames.append(currentFrame);
        currentFrame = 0;
    }
    if (frames.count()) {
        emit parsedFrames(frames);
    }
}

void LoaderThread::loadFile(const QString &fileName)
{
    m_fileName = fileName;
    start();
}

ApiTrace::FrameMarker LoaderThread::frameMarker() const
{
    return m_frameMarker;
}

void LoaderThread::setFrameMarker(ApiTrace::FrameMarker marker)
{
    Q_ASSERT(!isRunning());
    m_frameMarker = marker;
}

#include "loaderthread.moc"
