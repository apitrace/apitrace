#ifndef SAVERTHREAD_H
#define SAVERTHREAD_H


#include "apitrace.h"
#include <QThread>
#include <QList>

class ApiTraceCall;
class ApiTraceFrame;

class SaverThread : public QThread
{
    Q_OBJECT
public:
    SaverThread(QObject *parent=0);

public slots:
    void saveFile(const QString &fileName,
                  const QList<ApiTraceCall*> &calls);

signals:
    void traceSaved();

protected:
    virtual void run();

private:
    QString m_fileName;
    QList<ApiTraceCall*> m_calls;
};


#endif
