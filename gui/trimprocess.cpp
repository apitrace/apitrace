#include "trimprocess.h"
#include "apitrace.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

TrimProcess::TrimProcess(QObject *parent)
    : QObject(parent)
{
    m_process = new QProcess(this);

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(trimFinished()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(trimError(QProcess::ProcessError)));
}

TrimProcess::~TrimProcess()
{
}

void TrimProcess::trimFinished()
{
    // consume verbose output spew
    QByteArray outputStrings = m_process->readAllStandardOutput();
    QByteArray errorStrings = m_process->readAllStandardError();
#if 0
    qDebug()<<"trim finished on " << m_trimPath;
    qDebug()<<"\terr = "<<errorStrings;
    qDebug()<<"\tout = "<<outputStrings;
#endif
    emit trimmedFile(m_trimPath);
}

void TrimProcess::trimError(QProcess::ProcessError err)
{
    // consume verbose output spew
    QByteArray outputStrings = m_process->readAllStandardOutput();
    QByteArray errorStrings = m_process->readAllStandardError();
#if 1
    qDebug()<<"trace error = "<<m_tracePath;
    qDebug()<<"\terr = "<<errorStrings;
    qDebug()<<"\tout = "<<outputStrings;
#endif
    emit error(errorStrings);
}


void TrimProcess::start()
{
    QStringList arguments;

    QString outputFormat = QLatin1String("--output=%1");
    QString outputArgument = outputFormat
                                .arg(m_trimPath);

    QString callSetFormat = QLatin1String("--calls=0-%1");
    QString callSetArgument = callSetFormat
                                .arg(m_trimIndex);

    arguments << QLatin1String("trim-auto");
    arguments << outputArgument;
    arguments << callSetArgument;
    arguments << m_tracePath;

    m_process->start(QLatin1String("apitrace"), arguments);
}

int TrimProcess::trimIndex()
{
    return m_trimIndex;
}

void TrimProcess::setTrimIndex(int trimIndex)
{
    m_trimIndex = trimIndex;

    updateTrimPath();
}

void TrimProcess::setTracePath(const QString &str)
{
    m_tracePath = str;

    updateTrimPath();
}

QString TrimProcess::tracePath() const
{
    return m_tracePath;
}

void TrimProcess::updateTrimPath()
{

    QFileInfo fi(m_tracePath);
    QString baseName = fi.baseName();
    QString path = fi.path();

    QString format = QString::fromLatin1("%1/%2.%3.trim.trace");

    m_trimPath = format
                  .arg(path)
                  .arg(baseName)
                  .arg(m_trimIndex);
}

#include "trimprocess.moc"
