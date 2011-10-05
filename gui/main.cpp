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

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    qRegisterMetaType<QList<ApiTraceFrame*> >();
    qRegisterMetaType<QVector<ApiTraceCall*> >();
    qRegisterMetaType<ApiTraceState>();
    qRegisterMetaType<Qt::CaseSensitivity>();
    qRegisterMetaType<ApiTrace::SearchResult>();
    qRegisterMetaType<ApiTrace::SearchRequest>();
    MainWindow window;

    window.show();

    if (app.arguments().count() == 2)
        window.loadTrace(app.arguments()[1]);

    app.exec();
}
