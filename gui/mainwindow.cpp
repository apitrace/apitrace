#include "mainwindow.h"

#include "apitrace.h"
#include "apitracecall.h"
#include "apicalldelegate.h"
#include "apitracemodel.h"
#include "apitracefilter.h"
#include "imageviewer.h"
#include "jumpwidget.h"
#include "retracer.h"
#include "settingsdialog.h"
#include "shaderssourcewidget.h"
#include "tracedialog.h"
#include "ui_retracerdialog.h"
#include "vertexdatainterpreter.h"

#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QShortcut>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebPage>
#include <QWebView>


MainWindow::MainWindow()
    : QMainWindow(),
      m_selectedEvent(0),
      m_stateEvent(0)
{
    m_ui.setupUi(this);
    initObjects();
    initConnections();
}

void MainWindow::createTrace()
{
    TraceDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        qDebug()<< "App : " <<dialog.applicationPath();
        qDebug()<< "  Arguments: "<<dialog.arguments();
    }
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
        if (call->hasBinaryData()) {
            QByteArray data =
                call->argValues[call->binaryDataIndex()].toByteArray();
            m_vdataInterpreter->setData(data);

            for (int i = 0; i < call->argNames.count(); ++i) {
                QString name = call->argNames[i];
                if (name == QLatin1String("stride")) {
                    int stride = call->argValues[i].toInt();
                    m_ui.vertexStrideSB->setValue(stride);
                } else if (name == QLatin1String("size")) {
                    int components = call->argValues[i].toInt();
                    m_ui.vertexComponentsSB->setValue(components);
                } else if (name == QLatin1String("type")) {
                    QString val = call->argValues[i].toString();
                    int textIndex = m_ui.vertexTypeCB->findText(val);
                    if (textIndex >= 0)
                        m_ui.vertexTypeCB->setCurrentIndex(textIndex);
                }
            }
        }
        m_ui.vertexDataDock->setVisible(call->hasBinaryData());
        m_selectedEvent = call;
    } else {
        if (event && event->type() == ApiTraceEvent::Frame) {
            m_selectedEvent = static_cast<ApiTraceFrame*>(event);
        } else
            m_selectedEvent = 0;
        m_ui.detailsDock->hide();
        m_ui.vertexDataDock->hide();
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
    QDialog dlg;
    Ui_RetracerDialog dlgUi;
    dlgUi.setupUi(&dlg);

    dlgUi.doubleBufferingCB->setChecked(
        m_retracer->isDoubleBuffered());
    dlgUi.benchmarkCB->setChecked(
        m_retracer->isBenchmarking());

    if (dlg.exec() == QDialog::Accepted) {
        m_retracer->setDoubleBuffered(
            dlgUi.doubleBufferingCB->isChecked());
        m_retracer->setBenchmarking(
            dlgUi.benchmarkCB->isChecked());
        replayTrace(false);
    }
}

void MainWindow::replayStop()
{
    m_retracer->quit();
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);
    m_ui.actionLookupState->setEnabled(true);
}

void MainWindow::newTraceFile(const QString &fileName)
{
    m_traceFileName = fileName;
    m_trace->setFileName(fileName);

    if (m_traceFileName.isEmpty()) {
        m_ui.actionReplay->setEnabled(false);
        m_ui.actionLookupState->setEnabled(false);
        setWindowTitle(tr("QApiTrace"));
    } else {
        QFileInfo info(fileName);
        m_ui.actionReplay->setEnabled(true);
        m_ui.actionLookupState->setEnabled(true);
        setWindowTitle(
            tr("QApiTrace - %1").arg(info.fileName()));
    }
}

void MainWindow::replayFinished(const QString &output)
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);
    m_ui.actionLookupState->setEnabled(true);

    m_progressBar->hide();
    if (output.length() < 80) {
        statusBar()->showMessage(output);
    }
    m_stateEvent = 0;
    statusBar()->showMessage(
        tr("Replaying finished!"), 2000);
}

void MainWindow::replayError(const QString &message)
{
    m_ui.actionStop->setEnabled(false);
    m_ui.actionReplay->setEnabled(true);
    m_ui.actionLookupState->setEnabled(true);
    m_stateEvent = 0;

    m_progressBar->hide();
    statusBar()->showMessage(
        tr("Replaying unsuccessful."), 2000);
    QMessageBox::warning(
        this, tr("Replay Failed"), message);
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
    if (m_traceFileName.isEmpty())
        return;

    m_retracer->setFileName(m_traceFileName);
    m_retracer->setCaptureState(dumpState);
    if (m_retracer->captureState() && m_selectedEvent) {
        int index = 0;
        if (m_selectedEvent->type() == ApiTraceEvent::Call) {
            index = static_cast<ApiTraceCall*>(m_selectedEvent)->index;
        } else if (m_selectedEvent->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame =
                static_cast<ApiTraceFrame*>(m_selectedEvent);
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
        m_retracer->setCaptureAtCallNumber(index);
    }
    m_retracer->start();

    m_ui.actionStop->setEnabled(true);
    m_progressBar->show();
    if (dumpState)
        statusBar()->showMessage(
            tr("Looking up the state..."));
    else
        statusBar()->showMessage(
            tr("Replaying the trace file..."));
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
    replayTrace(true);
}

MainWindow::~MainWindow()
{
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

    const ApiTraceState &state = m_selectedEvent->state();
    m_ui.stateTreeWidget->clear();
    params = state.parameters();
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

    QStringList shaderSources = state.shaderSources();
    if (shaderSources.isEmpty()) {
        m_sourcesWidget->setShaders(shaderSources);
    } else {
        m_sourcesWidget->setShaders(shaderSources);
    }

    const QList<ApiTexture> &textures =
        state.textures();
    const QList<ApiFramebuffer> &fbos =
        state.framebuffers();

    m_ui.surfacesTreeWidget->clear();
    if (textures.isEmpty() && fbos.isEmpty()) {
        m_ui.surfacesTab->setDisabled(false);
    } else {
        m_ui.surfacesTreeWidget->setIconSize(QSize(64, 64));
        if (!textures.isEmpty()) {
            QTreeWidgetItem *textureItem =
                new QTreeWidgetItem(m_ui.surfacesTreeWidget);
            textureItem->setText(0, tr("Textures"));
            for (int i = 0; i < textures.count(); ++i) {
                const ApiTexture &texture =
                    textures[i];
                QIcon icon(QPixmap::fromImage(texture.thumb()));
                QTreeWidgetItem *item = new QTreeWidgetItem(textureItem);
                item->setIcon(0, icon);
                int width = texture.size().width();
                int height = texture.size().height();
                QString descr =
                    QString::fromLatin1("%1, %2 x %3")
                    .arg(texture.target())
                    .arg(width)
                    .arg(height);
                item->setText(1, descr);

                item->setData(0, Qt::UserRole,
                              texture.image());
            }
        }
        if (!fbos.isEmpty()) {
            QTreeWidgetItem *fboItem =
                new QTreeWidgetItem(m_ui.surfacesTreeWidget);
            fboItem->setText(0, tr("Framebuffers"));
            for (int i = 0; i < fbos.count(); ++i) {
                const ApiFramebuffer &fbo =
                    fbos[i];
                QIcon icon(QPixmap::fromImage(fbo.thumb()));
                QTreeWidgetItem *item = new QTreeWidgetItem(fboItem);
                item->setIcon(0, icon);
                int width = fbo.size().width();
                int height = fbo.size().height();
                QString descr =
                    QString::fromLatin1("%1, %2 x %3")
                    .arg(fbo.type())
                    .arg(width)
                    .arg(height);
                item->setText(1, descr);

                item->setData(0, Qt::UserRole,
                              fbo.image());
            }
        }
        m_ui.surfacesTab->setEnabled(true);
    }
    m_ui.stateDock->show();
}

void MainWindow::showSettings()
{
    SettingsDialog dialog;
    dialog.setFilterOptions(m_proxyModel->filterOptions());

    if (dialog.exec() == QDialog::Accepted) {
        m_proxyModel->setFilterOptions(dialog.filterOptions());
    }
}

void MainWindow::openHelp(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void MainWindow::showSurfacesMenu(const QPoint &pos)
{
    QTreeWidget *tree = m_ui.surfacesTreeWidget;
    QTreeWidgetItem *item = tree->itemAt(pos);
    if (!item)
        return;

    QMenu menu(tr("Surfaces"), this);
    //add needed actions
    QAction *act = menu.addAction(tr("View Image"));
    act->setStatusTip(tr("View the currently selected surface"));
    connect(act, SIGNAL(triggered()),
            SLOT(showSelectedSurface()));

    menu.exec(tree->viewport()->mapToGlobal(pos));
}

void MainWindow::showSelectedSurface()
{
    QTreeWidgetItem *item =
        m_ui.surfacesTreeWidget->currentItem();

    if (!item)
        return;

    QVariant var = item->data(0, Qt::UserRole);
    m_imageViewer->setImage(var.value<QImage>());
    m_imageViewer->show();
    m_imageViewer->raise();
    m_imageViewer->activateWindow();
}

void MainWindow::initObjects()
{
    m_ui.stateTreeWidget->sortByColumn(0, Qt::AscendingOrder);

    m_sourcesWidget = new ShadersSourceWidget(m_ui.shadersTab);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_sourcesWidget);
    m_ui.shadersTab->setLayout(layout);

    m_trace = new ApiTrace();
    m_retracer = new Retracer(this);

    m_vdataInterpreter = new VertexDataInterpreter(this);
    m_vdataInterpreter->setListWidget(m_ui.vertexDataListWidget);
    m_vdataInterpreter->setStride(
        m_ui.vertexStrideSB->value());
    m_vdataInterpreter->setComponents(
        m_ui.vertexComponentsSB->value());
    m_vdataInterpreter->setStartingOffset(
        m_ui.startingOffsetSB->value());
    m_vdataInterpreter->setTypeFromString(
        m_ui.vertexTypeCB->currentText());

    m_imageViewer = new ImageViewer(this);

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
    m_ui.vertexDataDock->hide();
    m_ui.stateDock->hide();
    setDockOptions(dockOptions() | QMainWindow::ForceTabbedDocks);

    tabifyDockWidget(m_ui.stateDock, m_ui.vertexDataDock);

    m_ui.surfacesTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_ui.detailsWebView->page()->setLinkDelegationPolicy(
        QWebPage::DelegateExternalLinks);

    m_jumpWidget = new JumpWidget(this);
    m_ui.centralLayout->addWidget(m_jumpWidget);
    m_jumpWidget->hide();


    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_G),
                  this, SLOT(slotGoTo()));
}

void MainWindow::initConnections()
{
    connect(m_trace, SIGNAL(startedLoadingTrace()),
            this, SLOT(startedLoadingTrace()));
    connect(m_trace, SIGNAL(finishedLoadingTrace()),
            this, SLOT(finishedLoadingTrace()));

    connect(m_retracer, SIGNAL(finished(const QString&)),
            this, SLOT(replayFinished(const QString&)));
    connect(m_retracer, SIGNAL(error(const QString&)),
            this, SLOT(replayError(const QString&)));
    connect(m_retracer, SIGNAL(foundState(const ApiTraceState&)),
            this, SLOT(replayStateFound(const ApiTraceState&)));

    connect(m_ui.vertexInterpretButton, SIGNAL(clicked()),
            m_vdataInterpreter, SLOT(interpretData()));
    connect(m_ui.vertexTypeCB, SIGNAL(currentIndexChanged(const QString&)),
            m_vdataInterpreter, SLOT(setTypeFromString(const QString&)));
    connect(m_ui.vertexStrideSB, SIGNAL(valueChanged(int)),
            m_vdataInterpreter, SLOT(setStride(int)));
    connect(m_ui.vertexComponentsSB, SIGNAL(valueChanged(int)),
            m_vdataInterpreter, SLOT(setComponents(int)));
    connect(m_ui.startingOffsetSB, SIGNAL(valueChanged(int)),
            m_vdataInterpreter, SLOT(setStartingOffset(int)));


    connect(m_ui.actionNew, SIGNAL(triggered()),
            this, SLOT(createTrace()));
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
    connect(m_ui.actionOptions, SIGNAL(triggered()),
            this, SLOT(showSettings()));

    connect(m_ui.callView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(callItemSelected(const QModelIndex &)));
    connect(m_filterEdit, SIGNAL(returnPressed()),
            this, SLOT(filterTrace()));

    connect(m_ui.surfacesTreeWidget,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            SLOT(showSurfacesMenu(const QPoint &)));
    connect(m_ui.surfacesTreeWidget,
            SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            SLOT(showSelectedSurface()));

    connect(m_ui.detailsWebView, SIGNAL(linkClicked(const QUrl&)),
            this, SLOT(openHelp(const QUrl&)));

    connect(m_jumpWidget, SIGNAL(jumpTo(int)),
            SLOT(slotJumpTo(int)));
}

void MainWindow::replayStateFound(const ApiTraceState &state)
{
    m_stateEvent->setState(state);
    m_model->stateSetOnEvent(m_stateEvent);
    if (m_selectedEvent == m_stateEvent) {
        fillStateForFrame();
    } else {
        m_ui.stateDock->hide();
    }
}

void MainWindow::slotGoTo()
{
    m_jumpWidget->show();
}

void MainWindow::slotJumpTo(int callNum)
{
    QModelIndex index = m_proxyModel->callIndex(callNum);
    if (index.isValid()) {
        m_ui.callView->setCurrentIndex(index);
    }
}

#include "mainwindow.moc"
