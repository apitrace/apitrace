#include "mainwindow.h"

#include "apitrace.h"
#include "apitracecall.h"

#include <QApplication>
#include <QMetaType>
#include <QVariant>

Q_DECLARE_METATYPE(QList<ApiTraceFrame*>);
Q_DECLARE_METATYPE(QVector<ApiTraceCall*>);
Q_DECLARE_METATYPE(Qt::CaseSensitivity);
Q_DECLARE_METATYPE(ApiTrace::SearchResult);
Q_DECLARE_METATYPE(ApiTrace::SearchRequest);

static void usage(void)
{
    qWarning("usage: qapitrace [TRACE] [CALLNO]\n");
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    qRegisterMetaType<QList<ApiTraceFrame*> >();
    qRegisterMetaType<QVector<ApiTraceCall*> >();
    qRegisterMetaType<ApiTraceState>();
    qRegisterMetaType<Qt::CaseSensitivity>();
    qRegisterMetaType<ApiTrace::SearchResult>();
    qRegisterMetaType<ApiTrace::SearchRequest>();
    QStringList args = app.arguments();

    int i = 1;
    while (i < args.count()) {
        QString arg = args[i];
        if (arg[0] != QLatin1Char('-')) {
            break;
        }
        ++i;
        if (arg == QLatin1String("--")) {
            break;
        } else if (arg == QLatin1String("-h") ||
                   arg == QLatin1String("--help")) {
            usage();
            exit(0);
        } else {
            usage();
            exit(1);
        }
    }

    MainWindow window;
    window.show();

    if (i < args.count()) {
        QString fileName = args[i++];

        int callNum = -1;
        if (i < args.count()) {
            callNum = args[i++].toInt();
        }
        window.loadTrace(fileName, callNum);
    }

    app.exec();
}
