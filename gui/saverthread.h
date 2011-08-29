#ifndef SAVERTHREAD_H
#define SAVERTHREAD_H


#include "apitrace.h"
#include <QThread>
#include <QVector>

class ApiTraceCall;
class ApiTraceFrame;

class SaverThread : public QThread
{
    Q_OBJECT
public:
    SaverThread(QObject *parent=0);

public slots:
    void saveFile(const QString &fileName,
                  const QVector<ApiTraceCall*> &calls);

signals:
    void traceSaved();

protected:
    virtual void run();

private:
    QString m_fileName;
    QVector<ApiTraceCall*> m_calls;
};


#endif
