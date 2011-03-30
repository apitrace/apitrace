#include "mainwindow.h"

#include "apitrace.h"
#include "apitracecall.h"
#include "apicalldelegate.h"
#include "apitracemodel.h"
#include "apitracefilter.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QProgressBar>
#include <QToolBar>
#include <QWebView>


MainWindow::MainWindow()
    : QMainWindow(),
      m_replayProcess(0)
{
    m_ui.setupUi(this);

    m_trace = new ApiTrace();
    connect(m_trace, SIGNAL(startedLoadingTrace()),
            this, SLOT(startedLoadingTrace()));
    connect(m_trace, SIGNAL(finishedLoadingTrace()),
            this, SLOT(finishedLoadingTrace()));

    m_model = new ApiTraceModel();
    m_model->setApiTrace(m_trace);
    m_proxyModel = new ApiTraceFilter();
    m_proxyModel->setSourceModel(m_model);
    m_ui.callView->setModel(m_proxyModel);
    m_ui.callView->setItemDelegate(new ApiCallDelegate);
    for (int column = 0; column < m_model->columnCount(); ++column)
        m_ui.callView->resizeColumnToContents(column);

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    m_filterEdit = new QLineEdit(toolBar);
    toolBar->addWidget(m_filterEdit);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 0);
    statusBar()->addPermanentWidget(m_progressBar);
    m_progressBar->hide();

    m_ui.detailsDock->hide();

    connect(m_ui.actionOpen, SIGNAL(triggered()),
            this, SLOT(openTrace()));
    connect(m_ui.actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_ui.actionReplay, SIGNAL(triggered()),
            this, SLOT(replayStart()));
    connect(m_ui.actionStop, SIGNAL(triggered()),
            this, SLOT(replayStop()));

    connect(m_ui.callView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(callItemSelected(const QModelIndex &)));
    connect(m_filterEdit, SIGNAL(returnPressed()),
            this, SLOT(filterTrace()));
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

    newTraceFile(fileName);
}

void MainWindow::loadTrace(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        QMessageBox::warning(this, tr("File Missing"),
                             tr("File '%1' doesn't exist.").arg(fileName));
        return;
    }
    qDebug()<< "Loading  : " <<fileName;

    m_progressBar->setValue(0);
    newTraceFile(fileName);
}

void MainWindow::callItemSelected(const QModelIndex &index)
{
    ApiTraceCall *call = index.data().value<ApiTraceCall*>();
    if (call) {
        m_ui.detailsWebView->setHtml(call->toHtml());
        m_ui.detailsDock->show();
    } else {
        m_ui.detailsDock->hide();
    }
}

void MainWindow::filterTrace()
{
    m_proxyModel->setFilterString(m_filterEdit->text());
}

void MainWindow::replayStart()
{
    if (!m_replayProcess) {
#ifdef Q_OS_WIN
        QString format = QLatin1String("%1;");
#else
        QString format = QLatin1String("%1:");
#endif
        QString buildPath = format.arg(BUILD_DIR);
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", buildPath + env.value("PATH"));

        qputenv("PATH", env.value("PATH").toLatin1());

        m_replayProcess = new QProcess(this);
        m_replayProcess->setProcessEnvironment(env);

        connect(m_replayProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(replayFinished()));
        connect(m_replayProcess, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(replayError(QProcess::ProcessError)));
    }

    if (m_traceFileName.isEmpty())
        return;

    QStringList arguments;
    arguments << m_traceFileName;

    m_replayProcess->start(QLatin1String("glretrace"),
                           arguments);

    m_ui.actionStop->setEnabled(true);
    m_ui.actionReplay->setEnabled(false);
}

void MainWindow::replayStop()
{
    if (m_replayProcess) {
        m_replayProcess->kill();

        m_ui.actionStop->setEnabled(false);
        m_ui.actionReplay->setEnabled(true);
    }
}

void MainWindow::newTraceFile(const QString &fileName)
{
    m_traceFileName = fileName;
    m_trace->setFileName(fileName);

    if (m_traceFileName.isEmpty()) {
        m_ui.actionReplay->setEnabled(false);
    } else {
        m_ui.actionReplay->setEnabled(true);
    }
}

void MainWindow::replayFinished()
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);

    QString output = m_replayProcess->readAllStandardOutput();

#if 0
    qDebug()<<"Process finished = ";
    qDebug()<<"\terr = "<<m_replayProcess->readAllStandardError();
    qDebug()<<"\tout = "<<output;
#endif

    if (output.length() < 80) {
        statusBar()->showMessage(output);
    }
}

void MainWindow::replayError(QProcess::ProcessError err)
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);

    qDebug()<<"Process error = "<<err;
    qDebug()<<"\terr = "<<m_replayProcess->readAllStandardError();
    qDebug()<<"\tout = "<<m_replayProcess->readAllStandardOutput();
    QMessageBox::warning(
        this, tr("Replay Failed"),
        tr("Couldn't execute the replay file '%1'").arg(m_traceFileName));
}

void MainWindow::startedLoadingTrace()
{
    Q_ASSERT(m_trace);
    m_progressBar->show();
    QFileInfo info(m_trace->fileName());
    statusBar()->showMessage(
        tr("Loading %1...").arg(info.fileName()));
}

void MainWindow::finishedLoadingTrace()
{
    m_progressBar->hide();
    if (!m_trace) {
        return;
    }
    QFileInfo info(m_trace->fileName());
    statusBar()->showMessage(
        tr("Loaded %1").arg(info.fileName()), 3000);
}

#include "mainwindow.moc"
