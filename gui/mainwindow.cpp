#include "mainwindow.h"

#include "apitracemodel.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileDialog>


MainWindow::MainWindow()
    : QMainWindow()
{
    m_ui.setupUi(this);

    m_model = new ApiTraceModel();
    m_ui.callView->setModel(m_model);
    for (int column = 0; column < m_model->columnCount(); ++column)
        m_ui.callView->resizeColumnToContents(column);

    connect(m_ui.actionOpen, SIGNAL(triggered()),
            this, SLOT(openTrace()));
}

void MainWindow::openTrace()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Open Trace"),
            QDir::homePath(),
            tr("Trace Files (*.trace)"));

    qDebug()<< "File name : " <<fileName;

    m_model->loadTraceFile(fileName);
}

void MainWindow::loadTrace(const QString &fileName)
{
    qDebug()<< "Loading  : " <<fileName;

    m_model->loadTraceFile(fileName);
}

#include "mainwindow.moc"
