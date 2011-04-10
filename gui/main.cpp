#include "mainwindow.h"

#include "apitracecall.h"

#include <QApplication>
#include <QMetaType>
#include <QVariant>

Q_DECLARE_METATYPE(QList<ApiTraceFrame*>);

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    qRegisterMetaType<QList<ApiTraceFrame*> >();
    qRegisterMetaType<ApiTraceState>();
    MainWindow window;

    window.show();

    if (app.arguments().count() == 2)
        window.loadTrace(app.arguments()[1]);

    app.exec();
}
