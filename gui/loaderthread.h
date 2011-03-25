#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include <QThread>
#include <QList>

class ApiTraceCall;
namespace Trace {
    class Call;
}

class LoaderThread : public QThread
{
    Q_OBJECT
public:
    LoaderThread(QObject *parent=0);

public slots:
    void loadFile(const QString &fileName);

signals:
    void parsedCalls(const QList<Trace::Call*> &calls);

protected:
    virtual void run();

private:
    QString m_fileName;
};

#endif
