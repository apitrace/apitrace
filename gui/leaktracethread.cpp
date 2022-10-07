#include "leaktracethread.h"

#include "apitracecall.h"

#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

void LeakTraceThread::run()
{
    QString msg = QLatin1String("Replay finished!");

    /*
     * Construct command line
     */

    QString prog = "apitrace";
    QStringList arguments;
    arguments << "leaks";
    arguments << filename;

    /*
     * Start the process.
     */

    {
        QDebug debug(QtDebugMsg);
        debug << "Running:";
        debug << prog;
        foreach (const QString &argument, arguments) {
            debug << argument;
        }
    }

    QProcess process;

    process.start(prog, arguments, QIODevice::ReadOnly);
    if (!process.waitForStarted(-1)) {
        return;
    }

    /*
     * Wait for process termination
     */

    process.waitForFinished(-1);

    if (process.exitStatus() != QProcess::NormalExit) {
        msg = QLatin1String("Process crashed");
    } else if (process.exitCode() != 0) {
        msg = QLatin1String("Process exited with non zero exit code");
    }

    /*
     * Parse errors.
     */

    QList<ApiTraceError> errors;
    process.setReadChannel(QProcess::StandardError);
    QRegularExpression regexp("^(\\d+): +(\\b\\w+\\b): ([^\\r\\n]+)[\\r\\n]*$");
    while (!process.atEnd()) {
        QString line = process.readLine();
        qDebug() << line;
        QRegularExpressionMatch match = regexp.match(line);
        if (match.hasMatch()) {
            qDebug() << "error";
            ApiTraceError error;
            error.callIndex = match.captured(1).toInt();
            error.type = match.captured(2);
            error.message = match.captured(3);
            errors.append(error);
        } else {
            qDebug() << line;
        }
    }

    /*
     * Emit signals
     */

    error = !errors.empty();
    emit leakTraceErrors(errors);
}

