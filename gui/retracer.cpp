#include "retracer.h"

#include "apitracecall.h"

#include <QDebug>
#include <QVariant>

#include <qjson/parser.h>

Retracer::Retracer(QObject *parent)
    : QThread(parent),
      m_benchmarking(false),
      m_doubleBuffered(true),
      m_captureState(false),
      m_captureCall(0)
{
#ifdef Q_OS_WIN
    QString format = QLatin1String("%1;");
#else
    QString format = QLatin1String("%1:");
#endif
    QString buildPath = format.arg(BUILD_DIR);
    m_processEnvironment = QProcessEnvironment::systemEnvironment();
    m_processEnvironment.insert("PATH", buildPath +
                                m_processEnvironment.value("PATH"));

    qputenv("PATH",
            m_processEnvironment.value("PATH").toLatin1());
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


void Retracer::run()
{
    RetraceProcess *retrace = new RetraceProcess();
    retrace->process()->setProcessEnvironment(m_processEnvironment);

    retrace->setFileName(m_fileName);
    retrace->setBenchmarking(m_benchmarking);
    retrace->setDoubleBuffered(m_doubleBuffered);
    retrace->setCaptureState(m_captureState);
    retrace->setCaptureAtCallNumber(m_captureCall);

    connect(retrace, SIGNAL(finished(const QString&)),
            this, SLOT(cleanup()));
    connect(retrace, SIGNAL(error(const QString&)),
            this, SLOT(cleanup()));
    connect(retrace, SIGNAL(finished(const QString&)),
            this, SIGNAL(finished(const QString&)));
    connect(retrace, SIGNAL(error(const QString&)),
            this, SIGNAL(error(const QString&)));
    connect(retrace, SIGNAL(foundState(const ApiTraceState&)),
            this, SIGNAL(foundState(const ApiTraceState&)));
    connect(retrace, SIGNAL(retraceErrors(const QList<RetraceError>&)),
            this, SIGNAL(retraceErrors(const QList<RetraceError>&)));

    retrace->start();

    exec();

    /* means we need to kill the process */
    if (retrace->process()->state() != QProcess::NotRunning) {
        retrace->terminate();
    }

    delete retrace;
}


void RetraceProcess::start()
{
    QStringList arguments;

    if (m_doubleBuffered) {
        arguments << QLatin1String("-db");
    } else {
        arguments << QLatin1String("-sb");
    }

    if (m_captureState) {
        arguments << QLatin1String("-D");
        arguments << QString::number(m_captureCall);
    } else {
        if (m_benchmarking) {
            arguments << QLatin1String("-b");
        }
    }

    arguments << m_fileName;

    m_process->start(QLatin1String("glretrace"), arguments);
}


void RetraceProcess::replayFinished()
{
    QByteArray output = m_process->readAllStandardOutput();
    QString msg;
    QString errStr = m_process->readAllStandardError();

#if 0
    qDebug()<<"Process finished = ";
    qDebug()<<"\terr = "<<errStr;
    qDebug()<<"\tout = "<<output;
#endif
    if (m_captureState) {
        bool ok = false;
        QVariantMap parsedJson = m_jsonParser->parse(output, &ok).toMap();
        ApiTraceState state(parsedJson);
        emit foundState(state);
        msg = tr("State fetched.");
    } else {
        msg = QString::fromUtf8(output);
    }

    QStringList errorLines = errStr.split('\n');
    QList<RetraceError> errors;
    QRegExp regexp("(^\\d+): +(\\b\\w+\\b): (.+$)");
    foreach(QString line, errorLines) {
        if (regexp.indexIn(line) != -1) {
            RetraceError error;
            error.callIndex = regexp.cap(1).toInt();
            error.type = regexp.cap(2);
            error.message = regexp.cap(3);
            errors.append(error);
        }
    }
    if (!errors.isEmpty()) {
        emit retraceErrors(errors);
    }
    emit finished(msg);
}

void RetraceProcess::replayError(QProcess::ProcessError err)
{
    qDebug()<<"Process error = "<<err;
    qDebug()<<"\terr = "<<m_process->readAllStandardError();
    qDebug()<<"\tout = "<<m_process->readAllStandardOutput();

    emit error(
        tr("Couldn't execute the replay file '%1'").arg(m_fileName));
}

Q_DECLARE_METATYPE(QList<RetraceError>);
RetraceProcess::RetraceProcess(QObject *parent)
    : QObject(parent)
{
    m_process = new QProcess(this);
    m_jsonParser = new QJson::Parser();

    qRegisterMetaType<QList<RetraceError> >();

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(replayFinished()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(replayError(QProcess::ProcessError)));
}

QProcess * RetraceProcess::process() const
{
    return m_process;
}

QString RetraceProcess::fileName() const
{
    return m_fileName;
}

void RetraceProcess::setFileName(const QString &name)
{
    m_fileName = name;
}

bool RetraceProcess::isBenchmarking() const
{
    return m_benchmarking;
}

void RetraceProcess::setBenchmarking(bool bench)
{
    m_benchmarking = bench;
}

bool RetraceProcess::isDoubleBuffered() const
{
    return m_doubleBuffered;
}

void RetraceProcess::setDoubleBuffered(bool db)
{
    m_doubleBuffered = db;
}

void RetraceProcess::setCaptureAtCallNumber(qlonglong num)
{
    m_captureCall = num;
}

qlonglong RetraceProcess::captureAtCallNumber() const
{
    return m_captureCall;
}

bool RetraceProcess::captureState() const
{
    return m_captureState;
}

void RetraceProcess::setCaptureState(bool enable)
{
    m_captureState = enable;
}

void RetraceProcess::terminate()
{
    if (m_process) {
        m_process->terminate();
        emit finished(tr("Process terminated."));
    }
}

void Retracer::cleanup()
{
    quit();
}

RetraceProcess::~RetraceProcess()
{
    delete m_jsonParser;
}

#include "retracer.moc"
