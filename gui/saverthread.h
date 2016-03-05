#pragma once


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
    void saveFile(const QString &saveFileName,
                  const QString &readFileName,
                  const QSet<ApiTraceCall*> &editedCalls);

signals:
    void traceSaved();

protected:
    virtual void run() override;

private:
    QString m_readFileName;
    QString m_writeFileName;
    QSet<ApiTraceCall*> m_editedCalls;
};
