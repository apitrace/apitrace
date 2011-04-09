#include "retracer.h"

#include <QDebug>

Retracer::Retracer(QObject *parent)
    : QObject(parent),
      m_benchmarking(true),
      m_doubleBuffered(true),
      m_captureState(false),
      m_captureCall(0),
      m_process(0)
{
}

QString Retracer::fileName() const
{
    return m_fileName;
}

void Retracer::setFileName(const QString &name)
{
    m_fileName = name;
}

bool Retracer::isBenchmarking() const
{
    return m_benchmarking;
}

void Retracer::setBenchmarking(bool bench)
{
    m_benchmarking = bench;
}

bool Retracer::isDoubleBuffered() const
{
    return m_doubleBuffered;
}

void Retracer::setDoubleBuffered(bool db)
{
    m_doubleBuffered = db;
}

void Retracer::start()
{
    if (!m_process) {
#ifdef Q_OS_WIN
        QString format = QLatin1String("%1;");
#else
        QString format = QLatin1String("%1:");
#endif
        QString buildPath = format.arg(BUILD_DIR);
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", buildPath + env.value("PATH"));

        qputenv("PATH", env.value("PATH").toLatin1());

        m_process = new QProcess(this);
        m_process->setProcessEnvironment(env);

        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(replayFinished()));
        connect(m_process, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(replayError(QProcess::ProcessError)));
    }

    QStringList arguments;

    if (m_captureState) {
        arguments << QLatin1String("-D");
        arguments << QString::number(m_captureCall);
    } else {
        if (m_benchmarking) {
            arguments << QLatin1String("-b");
        }
        if (m_doubleBuffered) {
            arguments << QLatin1String("-db");
        }
    }

    arguments << m_fileName;

    m_process->start(QLatin1String("glretrace"), arguments);
}

void Retracer::terminate()
{
    if (m_process) {
        m_process->terminate();
    }
}

void Retracer::setCaptureAtCallNumber(qlonglong num)
{
    m_captureCall = num;
}

qlonglong Retracer::captureAtCallNumber() const
{
    return m_captureCall;
}

bool Retracer::captureState() const
{
    return m_captureState;
}

void Retracer::setCaptureState(bool enable)
{
    m_captureState = enable;
}

void Retracer::replayFinished()
{
    QByteArray output = m_process->readAllStandardOutput();

#if 0
    qDebug()<<"Process finished = ";
    qDebug()<<"\terr = "<<m_process->readAllStandardError();
    qDebug()<<"\tout = "<<output;
#endif
    emit finished(output);
}

void Retracer::replayError(QProcess::ProcessError err)
{
    qDebug()<<"Process error = "<<err;
    qDebug()<<"\terr = "<<m_process->readAllStandardError();
    qDebug()<<"\tout = "<<m_process->readAllStandardOutput();

    emit error(
        tr("Couldn't execute the replay file '%1'").arg(m_fileName));
}

#include "retracer.moc"
