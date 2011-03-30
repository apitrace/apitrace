#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include "apitrace.h"
#include <QThread>
#include <QList>

class ApiTraceCall;
class ApiTraceFrame;

class LoaderThread : public QThread
{
    Q_OBJECT
public:
    LoaderThread(QObject *parent=0);

    ApiTrace::FrameMarker frameMarker() const;
    void setFrameMarker(ApiTrace::FrameMarker marker);
public slots:
    void loadFile(const QString &fileName);

signals:
    void parsedFrames(const QList<ApiTraceFrame*> &frames);

protected:
    virtual void run();

private:
    QString m_fileName;
    ApiTrace::FrameMarker m_frameMarker;
};

#endif
