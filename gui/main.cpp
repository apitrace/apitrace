#include "mainwindow.h"

#include "trace_model.hpp"

#include <QApplication>
#include <QMetaType>
#include <QVariant>

Q_DECLARE_METATYPE(QList<Trace::Call*>);

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    qRegisterMetaType<QList<Trace::Call*> >();
    MainWindow window;

    window.show();

    if (app.arguments().count() == 2)
        window.loadTrace(app.arguments()[1]);

    app.exec();
}
