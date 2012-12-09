#include "traceprocess.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

TraceProcess::TraceProcess(QObject *parent)
    : QObject(parent),
      m_canTrace(true)
{
    m_process = new QProcess(this);

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(traceFinished()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(traceError(QProcess::ProcessError)));
}

TraceProcess::~TraceProcess()
{
}

void TraceProcess::setApi(const QString &str)
{
    m_api = str;
}

void TraceProcess::setExecutablePath(const QString &str)
{
    m_execPath = str;

    QFileInfo fi(m_execPath);
    QString baseName = fi.baseName();

    QString format = QString::fromLatin1("%1.trace");

    m_tracePath = format
                  .arg(baseName);

    int i = 1;
    while (QFile::exists(m_tracePath)) {
        format = QString::fromLatin1("%1.%2.trace");
        m_tracePath = format
                      .arg(baseName)
                      .arg(i++);
    }
}

QString TraceProcess::executablePath() const
{
    return m_execPath;
}

void TraceProcess::setArguments(const QStringList &args)
{
    m_args = args;
}

QStringList TraceProcess::arguments() const
{
    return m_args;
}

void TraceProcess::traceFinished()
{
#if 0
    qDebug()<<"trace finished on " << m_tracePath;
    qDebug()<<"\terr = "<<m_process->readAllStandardError();
    qDebug()<<"\tout = "<<m_process->readAllStandardOutput();
#endif
    emit tracedFile(m_tracePath);
}

void TraceProcess::traceError(QProcess::ProcessError err)
{
#if 1
    qDebug()<<"trace error = "<<m_tracePath;
    qDebug()<<"\terr = "<<m_process->readAllStandardError();
    qDebug()<<"\tout = "<<m_process->readAllStandardOutput();
#endif
    emit error(m_process->readAllStandardError());
}


void TraceProcess::start()
{
    QStringList arguments;

    arguments << QLatin1String("trace");
    arguments << QLatin1String("--api");
    arguments << m_api;
    arguments << QLatin1String("--output");
    arguments << m_tracePath;
    arguments << QLatin1String("--");
    arguments << m_execPath;
    arguments << m_args;

    m_process->start(QLatin1String("apitrace"), arguments);
}

bool TraceProcess::canTrace() const
{
    return m_canTrace;
}

#include "traceprocess.moc"
