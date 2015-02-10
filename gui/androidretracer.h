#ifndef ANDROIDRETRACER_H
#define ANDROIDRETRACER_H

#include <QAtomicInt>
#include <QTcpSocket>

#include "androidutils.h"
#include "retracer.h"

class AndroidRetracer : public Retracer
{
    Q_OBJECT
public:
    AndroidRetracer(QObject *parent);

    void setAndroidFileName(const QString &fileName);

signals:
    void statusMessage(const QString &message);

protected:
    void run();

private slots:
    bool setup(QString *error);

private:
    QString m_androdiFileName;
    QString m_serialNumber;
    AndroidUtils m_androidUtils;
    int m_stdoutPort;
    int m_stderrPort;
};

#endif // ANDROIDRETRACER_H
