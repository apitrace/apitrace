#include "mainwindow.h"

#include "apitrace.h"
#include "apitracecall.h"
#include "apicalldelegate.h"
#include "apitracemodel.h"
#include "apitracefilter.h"

#include <qjson/parser.h>

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
      m_replayProcess(0),
      m_selectedEvent(0),
      m_stateEvent(0),
      m_findingState(false),
      m_jsonParser(new QJson::Parser())
{
    m_ui.setupUi(this);
    m_ui.stateTreeWidget->sortByColumn(0, Qt::AscendingOrder);

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
    m_ui.callView->resizeColumnToContents(0);
    m_ui.callView->header()->swapSections(0, 1);
    m_ui.callView->setColumnWidth(1, 42);

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    m_filterEdit = new QLineEdit(toolBar);
    toolBar->addWidget(m_filterEdit);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 0);
    statusBar()->addPermanentWidget(m_progressBar);
    m_progressBar->hide();

    m_ui.detailsDock->hide();
    m_ui.stateDock->hide();

    connect(m_ui.actionOpen, SIGNAL(triggered()),
            this, SLOT(openTrace()));
    connect(m_ui.actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_ui.actionReplay, SIGNAL(triggered()),
            this, SLOT(replayStart()));
    connect(m_ui.actionStop, SIGNAL(triggered()),
            this, SLOT(replayStop()));
    connect(m_ui.actionLookupState, SIGNAL(triggered()),
            this, SLOT(lookupState()));

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
    ApiTraceEvent *event =
        index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();

    if (event && event->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
        m_ui.detailsWebView->setHtml(call->toHtml());
        m_ui.detailsDock->show();
        m_selectedEvent = call;
    } else {
        if (event && event->type() == ApiTraceEvent::Frame) {
            m_selectedEvent = static_cast<ApiTraceFrame*>(event);
        } else
            m_selectedEvent = 0;
        m_ui.detailsDock->hide();
    }
    if (m_selectedEvent && !m_selectedEvent->state().isEmpty()) {
        fillStateForFrame();
    } else
        m_ui.stateDock->hide();
}

void MainWindow::filterTrace()
{
    m_proxyModel->setFilterString(m_filterEdit->text());
}

void MainWindow::replayStart()
{
    replayTrace(false);
}

void MainWindow::replayStop()
{
    if (m_replayProcess) {
        m_replayProcess->kill();

        m_ui.actionStop->setEnabled(false);
        m_ui.actionReplay->setEnabled(true);
        m_ui.actionLookupState->setEnabled(true);
    }
}

void MainWindow::newTraceFile(const QString &fileName)
{
    m_traceFileName = fileName;
    m_trace->setFileName(fileName);

    if (m_traceFileName.isEmpty()) {
        m_ui.actionReplay->setEnabled(false);
        m_ui.actionLookupState->setEnabled(false);
    } else {
        m_ui.actionReplay->setEnabled(true);
        m_ui.actionLookupState->setEnabled(true);
    }
}

void MainWindow::replayFinished()
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);
    m_ui.actionLookupState->setEnabled(true);

    QByteArray output = m_replayProcess->readAllStandardOutput();

#if 0
    qDebug()<<"Process finished = ";
    qDebug()<<"\terr = "<<m_replayProcess->readAllStandardError();
    qDebug()<<"\tout = "<<output;
#endif

    if (m_findingState) {
        bool ok = false;
        QVariantMap parsedJson = m_jsonParser->parse(output, &ok).toMap();
        parseState(parsedJson[QLatin1String("parameters")].toMap());
    } else if (output.length() < 80) {
        statusBar()->showMessage(output);
    }
    m_stateEvent = 0;
    m_findingState = false;
}

void MainWindow::replayError(QProcess::ProcessError err)
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);
    m_ui.actionLookupState->setEnabled(true);
    m_findingState = false;
    m_stateEvent = 0;

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

void MainWindow::replayTrace(bool dumpState)
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
    if (dumpState &&
        m_selectedEvent) {
        int index = 0;
        if (m_selectedEvent->type() == ApiTraceEvent::Call) {
            index = static_cast<ApiTraceCall*>(m_selectedEvent)->index;
        } else if (m_selectedEvent->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(m_selectedEvent);
            if (frame->calls.isEmpty()) {
                //XXX i guess we could still get the current state
                qDebug()<<"tried to get a state for an empty frame";
                return;
            }
            index = frame->calls.first()->index;
        } else {
            qDebug()<<"Unknown event type";
            return;
        }
        arguments << QLatin1String("-D");
        arguments << QString::number(index);
    }
    arguments << m_traceFileName;

    m_replayProcess->start(QLatin1String("glretrace"),
                           arguments);

    m_ui.actionStop->setEnabled(true);
}

void MainWindow::lookupState()
{
    if (!m_selectedEvent) {
        QMessageBox::warning(
            this, tr("Unknown Event"),
            tr("To inspect the state select an event in the event list."));
        return;
    }
    m_stateEvent = m_selectedEvent;
    m_findingState = true;
    replayTrace(true);
}

MainWindow::~MainWindow()
{
    delete m_jsonParser;
}

void MainWindow::parseState(const QVariantMap &params)
{
    QVariantMap::const_iterator itr;

    m_stateEvent->setState(params);
    m_model->stateSetOnEvent(m_stateEvent);
    if (m_selectedEvent == m_stateEvent) {
        fillStateForFrame();
    } else {
        m_ui.stateDock->hide();
    }
}

static void
variantToString(const QVariant &var, QString &str)
{
    if (var.type() == QVariant::List) {
        QVariantList lst = var.toList();
        str += QLatin1String("[");
        for (int i = 0; i < lst.count(); ++i) {
            QVariant val = lst[i];
            variantToString(val, str);
            if (i < lst.count() - 1)
                str += QLatin1String(", ");
        }
        str += QLatin1String("]");
    } else if (var.type() == QVariant::Map) {
        Q_ASSERT(!"unsupported state type");
    } else if (var.type() == QVariant::Hash) {
        Q_ASSERT(!"unsupported state type");
    } else {
        str += var.toString();
    }
}

void MainWindow::fillStateForFrame()
{
    QVariantMap::const_iterator itr;
    QVariantMap params;

    if (!m_selectedEvent || m_selectedEvent->state().isEmpty())
        return;

    m_ui.stateTreeWidget->clear();
    params = m_selectedEvent->state();
    QList<QTreeWidgetItem *> items;
    for (itr = params.constBegin(); itr != params.constEnd(); ++itr) {
        QString key = itr.key();
        QString val;

        variantToString(itr.value(), val);
        //qDebug()<<"key = "<<key;
        //qDebug()<<"val = "<<val;
        QStringList lst;
        lst += key;
        lst += val;
        items.append(new QTreeWidgetItem((QTreeWidget*)0, lst));
    }
    m_ui.stateTreeWidget->insertTopLevelItems(0, items);
    m_ui.stateDock->show();
}

#include "mainwindow.moc"
