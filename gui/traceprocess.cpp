#include "traceprocess.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

static QString
findPreloader()
{
    QString libPath = QString::fromLatin1("%1/glxtrace.so")
                      .arg(BUILD_DIR);

    QFileInfo fi(libPath);
    if (fi.exists())
        return libPath;

    libPath = QString::fromLatin1("/usr/local/lib/glxtrace.so");
    fi = QFileInfo(libPath);
    if (fi.exists())
        return libPath;

    libPath = QString::fromLatin1("/usr/lib/glxtrace.so");
    fi = QFileInfo(libPath);
    if (fi.exists())
        return libPath;

    return QString();
}

TraceProcess::TraceProcess(QObject *parent)
    : QObject(parent),
      m_canTrace(true)
{
    m_process = new QProcess(this);

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(traceFinished()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(traceError(QProcess::ProcessError)));

#ifdef Q_OS_WIN
    qWarning()<<"Windows tracing isn't supported right now!";
    m_canTrace = false;
#else
    QString var = QLatin1String("LD_PRELOAD");
    QString libPath = findPreloader();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (libPath.isEmpty()) {
        m_canTrace = false;
    }

    env.insert("LD_PRELOAD", libPath);
    qputenv("LD_PRELOAD", env.value("LD_PRELOAD").toLatin1());

    m_process->setProcessEnvironment(env);
#endif
}

TraceProcess::~TraceProcess()
{
}

void TraceProcess::setExecutablePath(const QString &str)
{
    m_execPath = str;

    QFileInfo fi(m_execPath);

    m_process->setWorkingDirectory(fi.absolutePath());

    QString format = QString::fromLatin1("%1%2%3.trace");

    m_tracePath = format
                  .arg(fi.absolutePath())
                  .arg(QDir::separator())
                  .arg(fi.baseName());

    int i = 1;
    while (QFile::exists(m_tracePath)) {
        QString format = QString::fromLatin1("%1%2%3.%4.trace");
        m_tracePath = format
                      .arg(fi.absolutePath())
                      .arg(QDir::separator())
                      .arg(fi.baseName())
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
    m_process->start(m_execPath, m_args);
}

bool TraceProcess::canTrace() const
{
    return m_canTrace;
}

#include "traceprocess.moc"
