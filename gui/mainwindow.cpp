#include "mainwindow.h"

#include "apicalldelegate.h"
#include "apitracemodel.h"
#include "apitracefilter.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileDialog>


MainWindow::MainWindow()
    : QMainWindow()
{
    m_ui.setupUi(this);

    m_model = new ApiTraceModel();
    m_proxyModel = new ApiTraceFilter();
    m_proxyModel->setSourceModel(m_model);
    m_ui.callView->setModel(m_model);
    m_ui.callView->setItemDelegate(new ApiCallDelegate);
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
