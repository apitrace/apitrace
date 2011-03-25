#include "loaderthread.h"

#include "trace_parser.hpp"

LoaderThread::LoaderThread(QObject *parent)
    : QThread(parent)
{
}

void LoaderThread::run()
{
    QList<Trace::Call*> traceCalls;
    Trace::Parser p;
    if (p.open(m_fileName.toLatin1().constData())) {
        Trace::Call *call;
        call = p.parse_call();
        while (call) {
            //std::cout << *call;
            traceCalls.append(call);
            if (traceCalls.count() >= 1000) {
                emit parsedCalls(traceCalls);
                traceCalls.clear();
            }
            call = p.parse_call();
        }
    }
    if (traceCalls.count()) {
        emit parsedCalls(traceCalls);
        traceCalls.clear();
    }
}

void LoaderThread::loadFile(const QString &fileName)
{
    m_fileName = fileName;
    start();
}

#include "loaderthread.moc"
