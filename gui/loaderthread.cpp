#include "loaderthread.h"

#include "trace_parser.hpp"

#include <QFile>
#include <QHash>
#include <QUrl>

#include <QDebug>

#define FRAMES_TO_CACHE 100

static ApiTraceCall *
apiCallFromTraceCall(const Trace::Call *call,
                     const QHash<QString, QUrl> &helpHash)
{
    ApiTraceCall *apiCall = new ApiTraceCall(call);

    apiCall->setHelpUrl(helpHash.value(apiCall->name()));

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

    QHash<QString, QUrl> helpHash;


    QFile file(":/resources/glreference.tsv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString line;
        while (!file.atEnd()) {
            line = file.readLine();
            QString function = line.section('\t', 0, 0).trimmed();
            QUrl url = QUrl(line.section('\t', 1, 1).trimmed());
            //qDebug()<<"function = "<<function<<", url = "<<url.toString();
            helpHash.insert(function, url);
        }
    } else {
        qWarning() << "Couldn't open reference file "
                   << file.fileName();
    }
    file.close();

    Trace::Parser p;
    if (p.open(m_fileName.toLatin1().constData())) {
        Trace::Call *call = p.parse_call();
        while (call) {
            //std::cout << *call;
            if (!currentFrame) {
                currentFrame = new ApiTraceFrame();
                currentFrame->number = frameCount;
                ++frameCount;
            }
            ApiTraceCall *apiCall =
                apiCallFromTraceCall(call, helpHash);
            apiCall->setParentFrame(currentFrame);
            currentFrame->addCall(apiCall);
            if (ApiTrace::isCallAFrameMarker(apiCall,
                                             m_frameMarker)) {
                frames.append(currentFrame);
                currentFrame = 0;
                if (frames.count() >= FRAMES_TO_CACHE) {
                    emit parsedFrames(frames);
                    frames.clear();
                }
            }
            delete call;
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
