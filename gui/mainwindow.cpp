#include "mainwindow.h"

#include "apitrace.h"
#include "apitracecall.h"
#include "apicalldelegate.h"
#include "apitracemodel.h"
#include "apitracefilter.h"
#include "argumentseditor.h"
#include "imageviewer.h"
#include "jumpwidget.h"
#include "profiledialog.h"
#include "retracer.h"
#include "searchwidget.h"
#include "settingsdialog.h"
#include "shaderssourcewidget.h"
#include "tracedialog.h"
#include "traceprocess.h"
#include "trimprocess.h"
#include "thumbnail.h"
#include "ui_retracerdialog.h"
#include "ui_profilereplaydialog.h"
#include "vertexdatainterpreter.h"
#include "trace_profiler.hpp"
#include "image.hpp"
#include "leaktracethread.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QSettings>


MainWindow::MainWindow()
    : QMainWindow(),
      m_api(trace::API_GL),
      m_initalCallNum(-1),
      m_selectedEvent(0),
      m_stateEvent(0),
      m_nonDefaultsLookupEvent(0)
{
    m_ui.setupUi(this);
    initWindowState();
    updateActionsState(false);
    initObjects();
    initConnections();
    updateRecentLaunchesMenu();
}

MainWindow::~MainWindow()
{
    delete m_trace;
    m_trace = 0;

    delete m_proxyModel;
    delete m_model;
}

void MainWindow::createTrace(const RecentLaunch* optionLaunch)
{
    if (!m_traceProcess->canTrace()) {
        QMessageBox::warning(
            this,
            tr("Unsupported"),
            tr("Current configuration doesn't support tracing."));
        return;
    }

    TraceDialog dialog;

    if(optionLaunch) {
        dialog.setApi(optionLaunch->api);
        dialog.setApplicationPath(optionLaunch->execPath);
        dialog.setWorkingDirPath(optionLaunch->workingDir);
        dialog.setArguments(optionLaunch->args);
    }

    if (dialog.exec() == QDialog::Accepted) {
        qDebug()<< "App : " <<dialog.applicationPath();
        qDebug()<< "  Arguments: "<<dialog.arguments();

        RecentLaunch rl;
        rl.api = dialog.api();
        rl.execPath = dialog.applicationPath();
        rl.workingDir = dialog.workingDirPath();
        rl.args = dialog.arguments();
        addRecentLaunch(rl);
        updateRecentLaunchesMenu();

        m_traceProcess->setApi(rl.api);
        m_traceProcess->setExecutablePathAndWorkingDir(
                    rl.execPath, rl.workingDir);
        m_traceProcess->setArguments(rl.args);
        m_traceProcess->start();
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

    if (!fileName.isEmpty() && QFile::exists(fileName)) {
        newTraceFile(fileName);
    }
}

void MainWindow::saveTrace()
{
    QString localFile = m_trace->fileName();

    QString fileName =
            QFileDialog::getSaveFileName(
                this,
                tr("Save Trace As"),
                QFileInfo(localFile).fileName(),
                tr("Trace Files (*.trace);;All Files (*)"));

    if (!fileName.isEmpty()) {
        // copy won't overwrite existing files!
        if (QFile::exists(fileName)) {
            if (!QFile::remove(fileName)) {
                QMessageBox::warning(
                    this, tr("Could not overwrite file"),
                    tr("The existing file %0 could not be replaced!")
                        .arg(fileName));
            }
        }
        QFile::copy(localFile, fileName);
    }
}

void MainWindow::loadTrace(const QString &fileName, int callNum)
{
    if (!QFile::exists(fileName)) {
        QMessageBox::warning(this, tr("File Missing"),
                             tr("File '%1' doesn't exist.").arg(fileName));
        return;
    }

    m_initalCallNum = callNum;
    newTraceFile(fileName);
}

void MainWindow::setRemoteTarget(const QString &host)
{
    m_retracer->setRemoteTarget(host);
}

void MainWindow::callItemSelected(const QModelIndex &index)
{
    ApiTraceEvent *event =
        index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();

    if (event && event->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
        m_ui.detailsDock->setWindowTitle(
            tr("Details View. Frame %1, Call %2")
            .arg(call->parentFrame() ? call->parentFrame()->number : 0)
            .arg(call->index()));
        m_ui.detailsWebView->setHtml(call->toHtml());
        m_ui.detailsDock->show();
        m_ui.callView->scrollTo(index);
        if (call->hasBinaryData()) {
            QByteArray data =
                call->arguments()[call->binaryDataIndex()].toByteArray();
            m_vdataInterpreter->setData(data);

            QVector<QVariant> args = call->arguments();
            for (int i = 0; i < call->argNames().count(); ++i) {
                QString name = call->argNames()[i];
                if (name == QLatin1String("stride")) {
                    int stride = args[i].toInt();
                    m_ui.vertexStrideSB->setValue(stride);
                } else if (name == QLatin1String("size")) {
                    int components = args[i].toInt();
                    m_ui.vertexComponentsSB->setValue(components);
                } else if (name == QLatin1String("type")) {
                    QString val = args[i].toString();
                    int textIndex = m_ui.vertexTypeCB->findText(val);
                    if (textIndex >= 0) {
                        m_ui.vertexTypeCB->setCurrentIndex(textIndex);
                    }
                }
            }
        }
        m_ui.backtraceBrowser->setText(call->backtrace());
        m_ui.backtraceDock->setVisible(!call->backtrace().isNull());
        m_ui.vertexDataDock->setVisible(call->hasBinaryData());
        m_selectedEvent = call;
    } else {
        if (event && event->type() == ApiTraceEvent::Frame) {
            m_selectedEvent = static_cast<ApiTraceFrame*>(event);
        } else {
            m_selectedEvent = 0;
        }
        m_ui.detailsDock->hide();
        m_ui.backtraceDock->hide();
        m_ui.vertexDataDock->hide();
    }
    if (m_selectedEvent && m_selectedEvent->hasState()) {
        fillStateForFrame();
    } else {
        m_ui.stateDock->hide();
    }
}

void MainWindow::callItemActivated(const QModelIndex &index) {
    lookupState();
}

void MainWindow::replayStart()
{
    if (m_trace->isSaving()) {
        QMessageBox::warning(
            this,
            tr("Trace Saving"),
            tr("QApiTrace is currently saving the edited trace file. "
               "Please wait until it finishes and try again."));
        return;
    }

    QDialog dlg;
    Ui_RetracerDialog dlgUi;
    dlgUi.setupUi(&dlg);

    dlgUi.doubleBufferingCB->setChecked(
        m_retracer->isDoubleBuffered());

    dlgUi.errorCheckCB->setChecked(
        !m_retracer->isBenchmarking());

    dlgUi.singlethreadCB->setChecked(
        m_retracer->isSinglethread());

    dlgUi.coreProfileCB->setChecked(
        m_retracer->isCoreProfile());

    dlgUi.queryHandlingSelector->addItem("Skip");
    dlgUi.queryHandlingSelector->addItem("Run");
    dlgUi.queryHandlingSelector->addItem("Run & Check");
    dlgUi.queryHandlingSelector->setCurrentIndex(
                m_retracer->queryHandling());

    dlgUi.queryCheckReportThreshold->setValue(
                m_retracer->queryCheckReportThreshold());


    if (dlg.exec() == QDialog::Accepted) {
        m_retracer->setDoubleBuffered(
            dlgUi.doubleBufferingCB->isChecked());

        m_retracer->setBenchmarking(
            !dlgUi.errorCheckCB->isChecked());

        m_retracer->setSinglethread(
            dlgUi.singlethreadCB->isChecked());

        m_retracer->setCoreProfile(
            dlgUi.coreProfileCB->isChecked());

        m_retracer->setProfiling(false, false, false);

        m_retracer->setQueryHandling(
                    dlgUi.queryHandlingSelector->currentIndex());
        m_retracer->setQueryCheckReportThreshold(
                    dlgUi.queryCheckReportThreshold->value());

        replayTrace(false, false);
    }
}

void MainWindow::replayProfile()
{
    if (m_trace->isSaving()) {
        QMessageBox::warning(
            this,
            tr("Trace Saving"),
            tr("QApiTrace is currently saving the edited trace file. "
               "Please wait until it finishes and try again."));
        return;
    }

    QDialog dlg;
    Ui_ProfileReplayDialog dlgUi;
    dlgUi.setupUi(&dlg);

    if (dlg.exec() == QDialog::Accepted) {
        m_retracer->setProfiling(
            dlgUi.gpuTimesCB->isChecked(),
            dlgUi.cpuTimesCB->isChecked(),
            dlgUi.pixelsDrawnCB->isChecked());

        replayTrace(false, false);
    }
}

void MainWindow::replayStop()
{
    m_retracer->quit();
    updateActionsState(true, true);
}

void MainWindow::newTraceFile(const QString &fileName)
{
    qDebug()<< "Loading:" << fileName;

    m_progressBar->setValue(0);
    m_trace->setFileName(fileName);

    if (fileName.isEmpty()) {
        updateActionsState(false);
        setWindowTitle(tr("QApiTrace"));
    } else {
        updateActionsState(true);
        QFileInfo info(fileName);
        setWindowTitle(
            tr("QApiTrace - %1").arg(info.fileName()));
    }
}

void MainWindow::replayFinished(const QString &message)
{
    updateActionsState(true);
    m_progressBar->hide();
    statusBar()->showMessage(message, 2000);
    m_stateEvent = 0;
    m_ui.actionShowErrorsDock->setEnabled(m_trace->hasErrors());
    m_ui.errorsDock->setVisible(m_trace->hasErrors());
    if (!m_trace->hasErrors()) {
        m_ui.errorsTreeWidget->clear();
    }
}

void MainWindow::replayError(const QString &message)
{
    updateActionsState(true);
    m_stateEvent = 0;
    m_nonDefaultsLookupEvent = 0;

    m_progressBar->hide();
    statusBar()->showMessage(
        tr("Replaying unsuccessful."), 2000);
    QMessageBox::warning(
        this, tr("Replay Failed"), message);
}

void MainWindow::loadError(const QString &message)
{
    m_progressBar->hide();
    statusBar()->showMessage(
        tr("Load unsuccessful."), 2000);
    QMessageBox::warning(
        this, tr("Load Failed"), message);
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
    m_api = m_trace->api();
    QFileInfo info(m_trace->fileName());
    statusBar()->showMessage(
        tr("Loaded %1").arg(info.fileName()), 3000);
    if (m_initalCallNum >= 0) {
        m_trace->findCallIndex(m_initalCallNum);
        m_initalCallNum = -1;
    } else {
       m_trace->finishedParsing();
    }
}

void MainWindow::replayTrace(bool dumpState, bool dumpThumbnails)
{
    if (m_trace->fileName().isEmpty()) {
        return;
    }

    m_retracer->setFileName(m_trace->fileName());
    m_retracer->setAPI(m_api);
    m_retracer->setCaptureState(dumpState);
    m_retracer->setMsaaResolve(m_ui.surfacesResolveMSAA->isChecked());
    m_retracer->setCaptureThumbnails(dumpThumbnails);
    if (m_retracer->captureState() && m_selectedEvent) {
        int index = 0;
        if (m_selectedEvent->type() == ApiTraceEvent::Call) {
            index = static_cast<ApiTraceCall*>(m_selectedEvent)->index();
        } else if (m_selectedEvent->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame =
                static_cast<ApiTraceFrame*>(m_selectedEvent);
            if (frame->isEmpty()) {
                //XXX i guess we could still get the current state
                qDebug()<<"tried to get a state for an empty frame";
                return;
            }
            index = frame->lastCallIndex();
        } else {
            qDebug()<<"Unknown event type";
            return;
        }
        m_retracer->setCaptureAtCallNumber(index);
    }
    if (m_trace->isMissingThumbnails()) {
        m_retracer->resetThumbnailsToCapture();
        m_trace->iterateMissingThumbnails(this, this->thumbnailCallback);
        m_trace->resetMissingThumbnails();
    }
    m_retracer->start();

    m_ui.actionStop->setEnabled(true);
    m_progressBar->show();
    if (dumpState || dumpThumbnails) {
        if (dumpState && dumpThumbnails) {
            statusBar()->showMessage(
                tr("Looking up the state and capturing thumbnails..."));
        } else if (dumpState) {
            statusBar()->showMessage(
                tr("Looking up the state..."));
        } else if (dumpThumbnails) {
            statusBar()->showMessage(
                tr("Capturing thumbnails..."));
        }
    } else if (m_retracer->isProfiling()) {
        statusBar()->showMessage(
                    tr("Profiling draw calls in trace file..."));
    } else {
        statusBar()->showMessage(
            tr("Replaying the trace file..."));
    }
}

void MainWindow::trimEvent()
{
    int trimIndex = 0;

    Q_ASSERT(m_trimEvent->type() == ApiTraceEvent::Call ||
             m_trimEvent->type() == ApiTraceEvent::Frame);

    if (m_trimEvent->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(m_trimEvent);
        trimIndex = call->index();
    } else if (m_trimEvent->type() == ApiTraceEvent::Frame) {
        ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(m_trimEvent);
        const QList<ApiTraceFrame*> frames = m_trace->frames();
        trimIndex = frame->lastCallIndex();
    }

    m_trimProcess->setTracePath(m_trace->fileName());
    m_trimProcess->setTrimIndex(trimIndex);

    m_trimProcess->start();
}

void MainWindow::lookupState()
{
    if (!m_selectedEvent) {
        QMessageBox::warning(
            this, tr("Unknown Event"),
            tr("To inspect the state select an event in the event list."));
        return;
    }
    if (m_trace->isSaving()) {
        QMessageBox::warning(
            this,
            tr("Trace Saving"),
            tr("QApiTrace is currently saving the edited trace file. "
               "Please wait until it finishes and try again."));
        return;
    }
    m_stateEvent = m_selectedEvent;
    replayTrace(true, false);
}

void MainWindow::showThumbnails()
{
    replayTrace(false, true);
}

void MainWindow::trim()
{
    if (!m_selectedEvent) {
        QMessageBox::warning(
            this, tr("Unknown Event"),
            tr("To trim select a frame or an event in the event list."));
        return;
    }
    m_trimEvent = m_selectedEvent;
    trimEvent();
}

void MainWindow::toggleCalls() {
    QItemSelection selection = m_ui.callView->selectionModel()->selection();
    if (selection.empty()) {
        QMessageBox::warning(
            this, tr("Unknown Event"),
            tr("To toggle calls select events in the event list."));
        return;
    }

    QModelIndexList selectedIndexes = selection.indexes();

    for (const QModelIndex& index : selectedIndexes) {
        ApiTraceEvent *event =
            index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
        if (event->type() == ApiTraceEvent::Call) {
            event->setIgnored(false);
        }
    }

    m_ignoredCalls.merge(selection, QItemSelectionModel::Toggle);

    selectedIndexes = m_ignoredCalls.indexes();
    for (const QModelIndex& index : selectedIndexes) {
        ApiTraceEvent *event =
            index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
        if (event->type() == ApiTraceEvent::Call){
            event->setIgnored(true);
        }
    }

    QList<RetracerCallRange> callRanges;
    for (const QItemSelectionRange& range : m_ignoredCalls) {
        ApiTraceEvent *eventTop =
            range.topLeft().data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();

        ApiTraceEvent *eventBottom =
            range.bottomRight().data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();

        if (eventTop->type() == ApiTraceEvent::Call && eventBottom->type() == ApiTraceEvent::Call) {
            RetracerCallRange callRange;
            callRange.m_callStartNo = static_cast<ApiTraceCall*>(eventTop)->index();
            callRange.m_callEndNo = static_cast<ApiTraceCall*>(eventBottom)->index();

            callRanges.push_back(callRange);
        }
    }

    m_retracer->setCallsToIgnore(callRanges);

    m_ui.callView->model()->dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::enableAllCalls() {
    const QModelIndexList ignoredCallIndexes = m_ignoredCalls.indexes();
    for (const QModelIndex& index : ignoredCallIndexes) {
        ApiTraceEvent *event =
            index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
        if (event->type() == ApiTraceEvent::Call){
            event->setIgnored(false);
        }
    }

    m_ignoredCalls.clear();
    m_retracer->setCallsToIgnore(QList<RetracerCallRange>());
    m_ui.callView->model()->dataChanged(QModelIndex(), QModelIndex());
}

static void
variantToString(const QVariant &var, QString &str)
{
    if (var.type() == QVariant::List) {
        QVector<QVariant> lst = var.toList().toVector();
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

static QTreeWidgetItem *
variantToItem(const QString &key, const QVariant &var,
              const QVariant &defaultVar);

static void
variantMapToItems(const QVariantMap &map, const QVariantMap &defaultMap,
                  QList<QTreeWidgetItem *> &items)
{
    QVariantMap::const_iterator itr;
    for (itr = map.constBegin(); itr != map.constEnd(); ++itr) {
        QString key = itr.key();
        QVariant var = itr.value();
        QVariant defaultVar = defaultMap[key];

        QTreeWidgetItem *item = variantToItem(key, var, defaultVar);
        if (item) {
            items.append(item);
        }
    }
}

static void
variantListToItems(const QVector<QVariant> &lst,
                   const QVector<QVariant> &defaultLst,
                   QList<QTreeWidgetItem *> &items)
{
    int width = QString::number(lst.count()).length();
    for (int i = 0; i < lst.count(); ++i) {
        QString key = QString::number(i).rightJustified(width, ' ');
        QVariant var = lst[i];
        QVariant defaultVar;

        if (i < defaultLst.count()) {
            defaultVar = defaultLst[i];
        }

        QTreeWidgetItem *item = variantToItem(key, var, defaultVar);
        if (item) {
            items.append(item);
        }
    }
}

// Get the depth (dimensionality) of the variant:
//
// It will return:
//  0: scalar
//  1: vector (up to 4 elems)
//  2: matrix (up to 4x4 elements)
//  3: array
//  4: map, etc.
static unsigned
getVariantDepth(const QVariant &var)
{
    if (var.type() == QVariant::List) {
        QVector<QVariant> lst = var.toList().toVector();
        unsigned maxDepth = 0;
        for (int i = 0; i < lst.count(); ++i) {
            unsigned elemDepth = getVariantDepth(lst[i]);
            if (elemDepth > maxDepth) {
                if (elemDepth >= 4) {
                    return elemDepth;
                }
                maxDepth = elemDepth;
            }
        }
        if (lst.count() > 1) {
            if (lst.count() > 4) {
                return 3;
            }
            maxDepth += 1;
        }
        return maxDepth;
    } else if (var.type() == QVariant::Map) {
        return 4;
    } else if (var.type() == QVariant::Hash) {
        return 4;
    } else {
        return 0;
    }
}

static QTreeWidgetItem *
variantToItem(const QString &key, const QVariant &var,
              const QVariant &defaultVar)
{
    if (var == defaultVar) {
        return NULL;
    }

    QString val;

    bool deep = getVariantDepth(var) >= 3;
    if (!deep) {
        variantToString(var, val);
    }

    //qDebug()<<"key = "<<key;
    //qDebug()<<"val = "<<val;
    QStringList lst;
    lst += key;
    lst += val;

    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidgetItem *)0, lst);

    if (deep) {
        QList<QTreeWidgetItem *> children;
        if (var.type() == QVariant::Map) {
            QVariantMap map = var.toMap();
            QVariantMap defaultMap = defaultVar.toMap();
            variantMapToItems(map, defaultMap, children);
        }
        if (var.type() == QVariant::List) {
            QVector<QVariant> lst = var.toList().toVector();
            QVector<QVariant> defaultLst = defaultVar.toList().toVector();
            variantListToItems(lst, defaultLst, children);
        }
        item->addChildren(children);
    }

    return item;
}

static void addSurfaceItem(const ApiSurface &surface,
                           const QString &label,
                           QTreeWidgetItem *parent,
                           QTreeWidget *tree,
                           bool opaque, bool alpha)
{
    QIcon icon(QPixmap::fromImage(surface.calculateThumbnail(opaque, alpha)));
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setIcon(0, icon);

    int width = surface.size().width();
    int height = surface.size().height();
    int depth = surface.depth();
    QString descr =
        QString::fromUtf8(u8"%1, %2, %3 \u00d7 %4")
        .arg(label)
        .arg(surface.formatName())
        .arg(width)
        .arg(height);
    if (depth > 1) {
        descr += QString::fromUtf8(u8" \u00d7 %1").arg(depth);
    }

    QString toolTip;
    toolTip += QString::fromLatin1("label = %1\n").arg(label);
    toolTip += QString::fromLatin1("format = %1\n").arg(surface.formatName());
    toolTip += QString::fromLatin1("width = %1\n").arg(width);
    toolTip += QString::fromLatin1("height = %1\n").arg(height);
    toolTip += QString::fromLatin1("depth = %1\n").arg(depth);
    item->setToolTip(0, toolTip);
    item->setToolTip(1, toolTip);

    //item->setText(1, descr);
    QLabel *l = new QLabel(descr, tree);
    l->setWordWrap(true);
    tree->setItemWidget(item, 1, l);

    item->setData(0, Qt::UserRole, surface.data());
}

void MainWindow::addSurface(const ApiTexture &image, QTreeWidgetItem *parent) {
    addSurface(image, image.label(), parent);
}

void MainWindow::addSurface(const ApiFramebuffer &fbo, QTreeWidgetItem *parent) {
    addSurface(fbo, fbo.type(), parent);
}

void MainWindow::addSurface(const ApiSurface &surface, const QString &label,
                            QTreeWidgetItem *parent)
{
    addSurfaceItem(surface, label, parent,
                   m_ui.surfacesTreeWidget, m_ui.surfacesOpaqueCB->isChecked(),
                   m_ui.surfacesAlphaCB->isChecked());
}

template <typename Surface>
void MainWindow::addSurfaces(const QList<Surface> &surfaces, const char *label) {
    if (!surfaces.isEmpty()) {
        QTreeWidgetItem *imageItem = new QTreeWidgetItem(m_ui.surfacesTreeWidget);
        imageItem->setText(0, tr(label));
        if (surfaces.count() <= 6) {
            imageItem->setExpanded(true);
        }
        for (int i = 0; i < surfaces.count(); ++i) {
            addSurface(surfaces[i], imageItem);
        }
    }
}

QStringList shortenReferencingShaderNames(QStringList referencingShaders)
{
    static QMap<QString, QString> map = {
        {"GL_REFERENCED_BY_VERTEX_SHADER", "VS"},
        {"GL_REFERENCED_BY_TESS_CONTROL_SHADER", "TCS"},
        {"GL_REFERENCED_BY_TESS_EVALUATION_SHADER", "TES"},
        {"GL_REFERENCED_BY_GEOMETRY_SHADER", "GS"},
        {"GL_REFERENCED_BY_FRAGMENT_SHADER", "FS"},
        {"GL_REFERENCED_BY_COMPUTE_SHADER", "CS"}};
    for (auto &referencingShader : referencingShaders) {
        assert(map.count(referencingShader));
        referencingShader = map[referencingShader];
    }
    return referencingShaders;
}

QStringList getReferencingShaders(QTreeWidgetItem *bufferItem)
{
    QStringList referencingShaders;
    for (int i = 0; i < bufferItem->childCount(); ++i) {
        const auto &text = bufferItem->child(i)->text(0);
        if (text.startsWith("GL_REFERENCED_BY_") && text.endsWith("_SHADER")) {
            referencingShaders.append(text);
        }
    }
    return referencingShaders;
}

void setValueOfSSBBItem(const ApiTraceState &state, QTreeWidgetItem *bufferItem)
{
    assert(bufferItem);
    const auto &bufferBindingItem = bufferItem->child(0);
    assert(bufferBindingItem->text(0) == "GL_BUFFER_BINDING");
    const int bufferBindingIndex = bufferBindingItem->text(1).toInt();
    qDebug() << bufferBindingIndex;
    assert(state.parameters().count("GL_SHADER_STORAGE_BUFFER"));
    assert(state.parameters()["GL_SHADER_STORAGE_BUFFER"].toMap().count("i"));
    assert(bufferBindingIndex < state.parameters()["GL_SHADER_STORAGE_BUFFER"]
                                    .toMap()["i"]
                                    .toList()
                                    .size());
    const auto &SSB = state.parameters()["GL_SHADER_STORAGE_BUFFER"]
                          .toMap()["i"]
                          .toList()[bufferBindingIndex]
                          .toMap();

    assert(SSB.count("GL_SHADER_STORAGE_BUFFER_START"));
    auto start = SSB["GL_SHADER_STORAGE_BUFFER_START"].toInt();

    assert(SSB.count("GL_SHADER_STORAGE_BUFFER_SIZE"));
    auto size = SSB["GL_SHADER_STORAGE_BUFFER_SIZE"].toInt();

    assert(SSB.count("GL_SHADER_STORAGE_BUFFER_BINDING"));
    auto bufferName = SSB["GL_SHADER_STORAGE_BUFFER_BINDING"].toInt();

    // Build overview text like:
    // "Binding 1 in VS, FS; Buffer 16 (6 Bytes starting at 2)"
    QString bindingText = QString("Binding %0").arg(bufferBindingIndex);

    QStringList referencingShaders =
        shortenReferencingShaderNames(getReferencingShaders(bufferItem));
    if (!referencingShaders.empty()) {
        bindingText += " in ";
        bindingText += referencingShaders.join(", ");
    }

    QString bufferText;
    if (bufferName != 0) {
        bufferText = QString("Buffer %0").arg(bufferName);
        if (size != 0) {
            if (start != 0) {
                bufferText +=
                    QString(" (%0 Bytes starting at %1)").arg(size).arg(start);
            } else {
                bufferText += QString(" (first %0 Bytes)").arg(size);
            }
        } else {
            if (start != 0) {
                bufferText += QString(" (starting at offset %0)").arg(start);
            }
        }
    }
    if (bufferText.isEmpty()) {
        bufferItem->setText(1, bindingText);
    } else {
        bufferItem->setText(1, bindingText + "; " + bufferText);
    }
}

void MainWindow::updateSurfacesView()
{
    updateSurfacesView(*m_selectedEvent->state());
}

void MainWindow::updateSurfacesView(const ApiTraceState &state)
{
    const QList<ApiTexture> &textures =
        state.textures();
    const QList<ApiFramebuffer> &fbos =
        state.framebuffers();

    m_ui.surfacesTreeWidget->clear();
    if (textures.isEmpty() && fbos.isEmpty()) {
        m_ui.surfacesTab->setEnabled(false);
    } else {
        m_ui.surfacesTreeWidget->setIconSize(QSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE));
        addSurfaces(textures, "Textures");
        addSurfaces(fbos, "Framebuffers");
        m_ui.surfacesTab->setEnabled(true);
    }
}

void MainWindow::fillStateForFrame()
{
    if (!m_selectedEvent || !m_selectedEvent->hasState()) {
        return;
    }

    if (m_nonDefaultsLookupEvent) {
        m_ui.nonDefaultsCB->blockSignals(true);
        m_ui.nonDefaultsCB->setChecked(true);
        m_ui.nonDefaultsCB->blockSignals(false);
    }

    bool nonDefaults = m_ui.nonDefaultsCB->isChecked();
    QVariantMap defaultParams;
    if (nonDefaults) {
        ApiTraceState defaultState = m_trace->defaultState();
        defaultParams = defaultState.parameters();
    }

    const ApiTraceState &state = *m_selectedEvent->state();
    m_ui.stateTreeWidget->clear();
    QList<QTreeWidgetItem *> items;
    variantMapToItems(state.parameters(), defaultParams, items);
    m_ui.stateTreeWidget->insertTopLevelItems(0, items);

    QMap<QString, QString> shaderSources = state.shaderSources();
    m_sourcesWidget->setShaders(shaderSources);

    m_ui.uniformsTreeWidget->clear();
    QList<QTreeWidgetItem *> uniformsItems;
    variantMapToItems(state.uniforms(), QVariantMap(), uniformsItems);
    m_ui.uniformsTreeWidget->insertTopLevelItems(0, uniformsItems);

    m_ui.buffersTreeWidget->clear();
    QList<QTreeWidgetItem *> buffersItems;
    variantMapToItems(state.buffers(), QVariantMap(), buffersItems);
    m_ui.buffersTreeWidget->insertTopLevelItems(0, buffersItems);

    updateSurfacesView(state);
    m_ui.stateDock->show();

    {
        m_ui.ssbsTreeWidget->clear();
        QList<QTreeWidgetItem *> buffersItems;
        variantMapToItems(state.shaderStorageBufferBlocks(), QVariantMap(),
                          buffersItems);
        const bool hasSSBs = buffersItems.size() > 0;
        for (auto const &bufferItem : buffersItems) {
            setValueOfSSBBItem(state, bufferItem);
        }
        m_ui.ssbsTreeWidget->insertTopLevelItems(0, buffersItems);
        m_ui.ssbTab->setEnabled(hasSSBs);
    }
}

void MainWindow::showSettings()
{
    SettingsDialog dialog;
    dialog.setFilterModel(m_proxyModel);

    dialog.exec();
}

void MainWindow::leakTrace()
{
    LeakTraceThread *t=new LeakTraceThread(m_trace->fileName());

    connect (t, &QThread::finished, this, &MainWindow::leakTraceFinished);
    connect (t, &LeakTraceThread::leakTraceErrors,
            this, &MainWindow::slotRetraceErrors);

    t->start();
}

void MainWindow::leakTraceFinished(){

    LeakTraceThread *t = qobject_cast<LeakTraceThread*>(sender());

    m_ui.errorsDock->setVisible(t->hasError());

    delete t;
}

void MainWindow::showSurfacesMenu(const QPoint &pos)
{
    QTreeWidget *tree = m_ui.surfacesTreeWidget;
    QTreeWidgetItem *item = tree->itemAt(pos);
    if (!item) {
        return;
    }

    QMenu menu(tr("Surfaces"), this);

    QAction *act = menu.addAction(tr("View Image"));
    act->setStatusTip(tr("View the currently selected surface"));
    connect(act, &QAction::triggered, this, &MainWindow::showSelectedSurface);

    act = menu.addAction(tr("Save Image"));
    act->setStatusTip(tr("Save the currently selected surface"));
    connect(act, &QAction::triggered, this, &MainWindow::saveSelectedSurface);

    menu.exec(tree->viewport()->mapToGlobal(pos));
}

void MainWindow::showSelectedSurface()
{
    QTreeWidgetItem *item =
        m_ui.surfacesTreeWidget->currentItem();
    if (!item) {
        return;
    }

    QVariant var = item->data(0, Qt::UserRole);
    if (!var.isValid()) {
        return;
    }

    ImageViewer *viewer =
        new ImageViewer(this, m_ui.surfacesOpaqueCB->isChecked(),
                              m_ui.surfacesAlphaCB->isChecked());

    QString title;
    if (selectedCall()) {
        title = tr("QApiTrace - Surface at %1 (%2)")
                .arg(selectedCall()->name())
                .arg(selectedCall()->index());
    } else {
        title = tr("QApiTrace - Surface Viewer");
    }
    viewer->setWindowTitle(title);

    viewer->setAttribute(Qt::WA_DeleteOnClose, true);

    QByteArray data = var.value<QByteArray>();
    viewer->setData(data);

    viewer->show();
    viewer->raise();
    viewer->activateWindow();
}

void MainWindow::initWindowState()
{
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
}

void MainWindow::saveWindowState()
{
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

void MainWindow::initObjects()
{
    m_ui.stateTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    m_ui.uniformsTreeWidget->sortByColumn(0, Qt::AscendingOrder);

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

    m_model = new ApiTraceModel();
    m_model->setApiTrace(m_trace);
    m_proxyModel = new ApiTraceFilter();
    m_proxyModel->setSourceModel(m_model);
    m_ui.callView->setModel(m_proxyModel);
    m_ui.callView->setItemDelegate(
        new ApiCallDelegate(m_ui.callView));
    m_ui.callView->resizeColumnToContents(0);
    m_ui.callView->header()->swapSections(0, 1);
    m_ui.callView->setColumnWidth(1, 42);
    m_ui.callView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui.callView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
    m_ui.callView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    statusBar()->addPermanentWidget(m_progressBar);
    m_progressBar->hide();

    m_argsEditor = new ArgumentsEditor(this);

    m_ui.detailsDock->hide();
    m_ui.backtraceDock->hide();
    m_ui.errorsDock->hide();
    m_ui.vertexDataDock->hide();
    m_ui.stateDock->hide();
    setDockOptions(dockOptions() | QMainWindow::ForceTabbedDocks);

    tabifyDockWidget(m_ui.stateDock, m_ui.vertexDataDock);
    tabifyDockWidget(m_ui.detailsDock, m_ui.errorsDock);
    tabifyDockWidget(m_ui.detailsDock, m_ui.backtraceDock);

    m_ui.surfacesTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_jumpWidget = new JumpWidget(this);
    m_ui.centralLayout->addWidget(m_jumpWidget);
    m_jumpWidget->hide();

    m_searchWidget = new SearchWidget(this);
    m_ui.centralLayout->addWidget(m_searchWidget);
    m_searchWidget->hide();

    m_traceProcess = new TraceProcess(this);
    m_trimProcess = new TrimProcess(this);

    m_profileDialog = new ProfileDialog();
}

void MainWindow::initConnections()
{
    connect(m_trace, &ApiTrace::problemLoadingTrace,
            this, &MainWindow::loadError);
    connect(m_trace, &ApiTrace::startedLoadingTrace,
            this, &MainWindow::startedLoadingTrace);
    connect(m_trace, &ApiTrace::loaded,
            this, &MainWindow::loadProgess);
    connect(m_trace, &ApiTrace::finishedLoadingTrace,
            this, &MainWindow::finishedLoadingTrace);
    connect(m_trace, &ApiTrace::startedSaving,
            this, &MainWindow::slotStartedSaving);
    connect(m_trace, &ApiTrace::saved,
            this, &MainWindow::slotSaved);
    connect(m_trace, &ApiTrace::changed,
            this, &MainWindow::slotTraceChanged);
    connect(m_trace, &ApiTrace::findResult,
            this, &MainWindow::slotSearchResult);
    connect(m_trace, &ApiTrace::foundFrameStart,
            this, &MainWindow::slotFoundFrameStart);
    connect(m_trace, &ApiTrace::foundFrameEnd,
            this, &MainWindow::slotFoundFrameEnd);
    connect(m_trace, &ApiTrace::foundCallIndex,
            this, &MainWindow::slotJumpToResult);

    connect(m_retracer, &Retracer::finished,
            this, &MainWindow::replayFinished);
    connect(m_retracer, &Retracer::error,
            this, &MainWindow::replayError);
    connect(m_retracer, &Retracer::foundState,
            this, &MainWindow::replayStateFound);
    connect(m_retracer, &Retracer::foundProfile,
            this, &MainWindow::replayProfileFound);
    connect(m_retracer, qOverload<const ImageHash&>(&Retracer::foundThumbnails),
            this, &MainWindow::replayThumbnailsFound);
    connect(m_retracer, &Retracer::retraceErrors,
            this, &MainWindow::slotRetraceErrors);

    connect(m_ui.vertexInterpretButton, &QAbstractButton::clicked,
            m_vdataInterpreter, &VertexDataInterpreter::interpretData);
    connect(m_ui.bufferExportButton, &QAbstractButton::clicked,
            this, &MainWindow::exportBufferData);
    connect(m_ui.vertexTypeCB, &QComboBox::currentTextChanged,
            m_vdataInterpreter, &VertexDataInterpreter::setTypeFromString);
    connect(m_ui.vertexStrideSB, &QSpinBox::valueChanged,
            m_vdataInterpreter, &VertexDataInterpreter::setStride);
    connect(m_ui.vertexComponentsSB, &QSpinBox::valueChanged,
            m_vdataInterpreter, &VertexDataInterpreter::setComponents);
    connect(m_ui.startingOffsetSB, &QSpinBox::valueChanged,
            m_vdataInterpreter, &VertexDataInterpreter::setStartingOffset);


    connect(m_ui.actionNew, &QAction::triggered,
            this, qOverload<>(&MainWindow::createTrace));
    connect(m_ui.actionOpen, &QAction::triggered,
            this, &MainWindow::openTrace);
    connect(m_ui.actionSave, &QAction::triggered,
            this, &MainWindow::saveTrace);
    connect(m_ui.actionQuit, &QAction::triggered,
            this, &QWidget::close);

    connect(m_ui.actionFind, &QAction::triggered,
            this, &MainWindow::slotSearch);
    connect(m_ui.actionGo, &QAction::triggered,
            this, &MainWindow::slotGoTo);
    connect(m_ui.actionGoFrameStart, &QAction::triggered,
            this, &MainWindow::slotGoFrameStart);
    connect(m_ui.actionGoFrameEnd, &QAction::triggered,
            this, &MainWindow::slotGoFrameEnd);

    connect(m_ui.actionReplay, &QAction::triggered,
            this, &MainWindow::replayStart);
    connect(m_ui.actionProfile, &QAction::triggered,
            this, &MainWindow::replayProfile);
    connect(m_ui.actionStop, &QAction::triggered,
            this, &MainWindow::replayStop);
    connect(m_ui.actionLookupState, &QAction::triggered,
            this, &MainWindow::lookupState);
    connect(m_ui.actionTrim, &QAction::triggered,
            this, &MainWindow::trim);
    connect(m_ui.actionToggleCalls, &QAction::triggered,
            this, &MainWindow::toggleCalls);
    connect(m_ui.actionEnableAllCalls, &QAction::triggered,
            this, &MainWindow::enableAllCalls);
    connect(m_ui.actionShowThumbnails, &QAction::triggered,
            this, &MainWindow::showThumbnails);
    connect(m_ui.actionOptions, &QAction::triggered,
            this, &MainWindow::showSettings);
    connect(m_ui.actionLeakTrace,&QAction::triggered,
            this, &MainWindow::leakTrace);

    connect(m_ui.callView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::callItemSelected);
    connect(m_ui.callView, &QAbstractItemView::doubleClicked,
            this, &MainWindow::callItemActivated);
    connect(m_ui.callView, &QWidget::customContextMenuRequested,
            this, &MainWindow::customContextMenuRequested);

    connect(m_ui.surfacesTreeWidget, &QWidget::customContextMenuRequested,
            this, &MainWindow::showSurfacesMenu);
    connect(m_ui.surfacesTreeWidget, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::showSelectedSurface);

    connect(m_ui.nonDefaultsCB, &QAbstractButton::toggled,
            this, &MainWindow::fillState);

    connect(m_jumpWidget, &JumpWidget::jumpTo, this, &MainWindow::slotJumpTo);

    connect(m_searchWidget, &SearchWidget::searchNext,
            this, &MainWindow::slotSearchNext);
    connect(m_searchWidget, &SearchWidget::searchPrev,
            this, &MainWindow::slotSearchPrev);

    connect(m_traceProcess, &TraceProcess::tracedFile,
            this, &MainWindow::createdTrace);
    connect(m_traceProcess, &TraceProcess::error,
            this, &MainWindow::traceError);

    connect(m_trimProcess, &TrimProcess::trimmedFile,
            this, &MainWindow::createdTrim);
    connect(m_trimProcess, &TrimProcess::error,
            this, &MainWindow::trimError);

    connect(m_ui.errorsDock, &QDockWidget::visibilityChanged,
            m_ui.actionShowErrorsDock, &QAction::setChecked);
    connect(m_ui.actionShowErrorsDock, &QAction::triggered,
            m_ui.errorsDock, &QWidget::setVisible);
    connect(m_ui.errorsTreeWidget, &QTreeWidget::itemActivated,
            this, &MainWindow::slotErrorSelected);

    connect(m_ui.actionShowProfileDialog, &QAction::triggered,
            m_profileDialog, &QWidget::show);
    connect(m_profileDialog, &ProfileDialog::jumpToCall,
            this, &MainWindow::slotJumpTo);

    connect(m_ui.surfacesOpaqueCB, &QCheckBox::stateChanged, this,
            qOverload<>(&MainWindow::updateSurfacesView));
    connect(m_ui.surfacesAlphaCB, &QCheckBox::stateChanged, this,
            qOverload<>(&MainWindow::updateSurfacesView));
    connect(m_ui.surfacesResolveMSAA, &QCheckBox::stateChanged, this,
            qOverload<>(&MainWindow::updateSurfacesView));
}

void MainWindow::updateActionsState(bool traceLoaded, bool stopped)
{
    if (traceLoaded) {
        /* File */
        m_ui.actionSave          ->setEnabled(true);

        /* Edit */
        m_ui.actionFind          ->setEnabled(true);
        m_ui.actionGo            ->setEnabled(true);
        m_ui.actionGoFrameStart  ->setEnabled(true);
        m_ui.actionGoFrameEnd    ->setEnabled(true);

        /* Trace */
        if (stopped) {
            m_ui.actionStop->setEnabled(false);
            m_ui.actionReplay->setEnabled(true);
        }
        else {
            m_ui.actionStop->setEnabled(true);
            m_ui.actionReplay->setEnabled(false);
        }

        m_ui.actionProfile       ->setEnabled(true);
        m_ui.actionLookupState   ->setEnabled(true);
        m_ui.actionShowThumbnails->setEnabled(true);
        m_ui.actionTrim          ->setEnabled(true);
        m_ui.actionToggleCalls   ->setEnabled(true);
        m_ui.actionEnableAllCalls->setEnabled(true);
    }
    else {
        /* File */
        m_ui.actionSave          ->setEnabled(false);

        /* Edit */
        m_ui.actionFind          ->setEnabled(false);
        m_ui.actionGo            ->setEnabled(false);
        m_ui.actionGoFrameStart  ->setEnabled(false);
        m_ui.actionGoFrameEnd    ->setEnabled(false);

        /* Trace */
        m_ui.actionReplay        ->setEnabled(false);
        m_ui.actionProfile       ->setEnabled(false);
        m_ui.actionStop          ->setEnabled(false);
        m_ui.actionLookupState   ->setEnabled(false);
        m_ui.actionShowThumbnails->setEnabled(false);
        m_ui.actionTrim          ->setEnabled(false);
        m_ui.actionToggleCalls   ->setEnabled(false);
        m_ui.actionEnableAllCalls->setEnabled(false);
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    m_profileDialog->close();
    saveWindowState();
    QMainWindow::closeEvent(event);
}

void MainWindow::replayProfileFound(trace::Profile *profile)
{
    m_ui.actionShowProfileDialog->setEnabled(true);
    m_profileDialog->setProfile(profile);
    m_profileDialog->show();
    m_profileDialog->activateWindow();
    m_profileDialog->setFocus();
}

void MainWindow::replayStateFound(ApiTraceState *state)
{
    m_stateEvent->setState(state);
    m_model->stateSetOnEvent(m_stateEvent);
    if (m_selectedEvent == m_stateEvent ||
        m_nonDefaultsLookupEvent == m_selectedEvent) {
        fillStateForFrame();
    } else {
        m_ui.stateDock->hide();
    }
    m_nonDefaultsLookupEvent = 0;
}

void MainWindow::replayThumbnailsFound(const ImageHash &thumbnails)
{
    m_ui.callView->setUniformRowHeights(false);
    m_trace->bindThumbnails(thumbnails);
}

void MainWindow::slotGoTo()
{
    m_searchWidget->hide();
    m_jumpWidget->show();
}

void MainWindow::slotJumpTo(int callNum)
{
    m_trace->findCallIndex(callNum);
}

void MainWindow::createdTrace(const QString &path)
{
    qDebug()<<"Done tracing "<<path;
    newTraceFile(path);
}

void MainWindow::traceError(const QString &msg)
{
    QMessageBox::warning(
            this,
            tr("Tracing Error"),
            msg);
}

void MainWindow::createdTrim(const QString &path)
{
    qDebug()<<"Done trimming "<<path;

    newTraceFile(path);
}

void MainWindow::trimError(const QString &msg)
{
    QMessageBox::warning(
            this,
            tr("Trim Error"),
            msg);
}

void MainWindow::slotSearch()
{
    m_jumpWidget->hide();
    m_searchWidget->show();
}

void MainWindow::slotSearchNext(const QString &str,
                                Qt::CaseSensitivity sensitivity,
                                bool useRegex)
{
    ApiTraceCall *call = currentCall();
    ApiTraceFrame *frame = currentFrame();

    if (!frame) {
        // Trace is still loading.
        if (!call) {
            return;
        }
        frame = call->parentFrame();
    }
    Q_ASSERT(frame);

    m_trace->findNext(frame, call, str, sensitivity, useRegex);
}

void MainWindow::slotSearchPrev(const QString &str,
                                Qt::CaseSensitivity sensitivity,
                                bool useRegex)
{
    ApiTraceCall *call = currentCall();
    ApiTraceFrame *frame = currentFrame();

    if (!frame) {
        // Trace is still loading.
        if (!call) {
            return;
        }
        frame = call->parentFrame();
    }
    Q_ASSERT(frame);

    m_trace->findPrev(frame, call, str, sensitivity, useRegex);
}

void MainWindow::fillState(bool nonDefaults)
{
    if (nonDefaults) {
        ApiTraceState defaultState = m_trace->defaultState();
        if (defaultState.isEmpty()) {
            m_ui.nonDefaultsCB->blockSignals(true);
            m_ui.nonDefaultsCB->setChecked(false);
            m_ui.nonDefaultsCB->blockSignals(false);
            ApiTraceFrame *firstFrame =
                m_trace->frameAt(0);
            if (!firstFrame) {
                return;
            }
            if (!firstFrame->isLoaded()) {
                m_trace->loadFrame(firstFrame);
                return;
            }
            ApiTraceCall *firstCall = firstFrame->calls().first();
            ApiTraceEvent *oldSelected = m_selectedEvent;
            m_nonDefaultsLookupEvent = m_selectedEvent;
            m_selectedEvent = firstCall;
            lookupState();
            m_selectedEvent = oldSelected;
        }
    }
    fillStateForFrame();
}

void MainWindow::customContextMenuRequested(QPoint pos)
{
    QModelIndex index = m_ui.callView->indexAt(pos);

    callItemSelected(index);
    if (!index.isValid()) {
        return;
    }

    ApiTraceEvent *event =
        index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
    if (!event) {
        return;
    }

    QMenu menu;
    menu.addAction(QIcon(":/resources/media-record.png"),
                   tr("Lookup state"), this, &MainWindow::lookupState);
    if (event->type() == ApiTraceEvent::Call) {
        menu.addAction(tr("Edit"), this, &MainWindow::editCall);
    }

    menu.exec(QCursor::pos());
}

void MainWindow::editCall()
{
    if (m_selectedEvent && m_selectedEvent->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(m_selectedEvent);
        m_argsEditor->setCall(call);
        m_argsEditor->show();
    }
}

void MainWindow::slotStartedSaving()
{
    m_progressBar->show();
    statusBar()->showMessage(
        tr("Saving to %1").arg(m_trace->fileName()));
}

void MainWindow::slotSaved()
{
    statusBar()->showMessage(
        tr("Saved to %1").arg(m_trace->fileName()), 2000);
    m_progressBar->hide();
}

void MainWindow::slotGoFrameStart()
{
    ApiTraceFrame *frame = currentFrame();
    ApiTraceCall *call = currentCall();

    if (!frame && call) {
        frame = call->parentFrame();
    }

    m_trace->findFrameStart(frame);
}

void MainWindow::slotGoFrameEnd()
{
    ApiTraceFrame *frame = currentFrame();
    ApiTraceCall *call = currentCall();

    if (!frame && call) {
        frame = call->parentFrame();
    }

    m_trace->findFrameEnd(frame);
}

ApiTraceFrame * MainWindow::selectedFrame() const
{
    if (m_selectedEvent) {
        if (m_selectedEvent->type() == ApiTraceEvent::Frame) {
            return static_cast<ApiTraceFrame*>(m_selectedEvent);
        } else {
            Q_ASSERT(m_selectedEvent->type() == ApiTraceEvent::Call);
            ApiTraceCall *call = static_cast<ApiTraceCall*>(m_selectedEvent);
            return call->parentFrame();
        }
    }
    return NULL;
}

void MainWindow::slotTraceChanged(ApiTraceEvent *event)
{
    Q_ASSERT(event);
    if (event == m_selectedEvent) {
        if (event->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
            m_ui.detailsWebView->setHtml(call->toHtml());
        }
    }
}

void MainWindow::slotRetraceErrors(const QList<ApiTraceError> &errors)
{
    m_ui.errorsTreeWidget->clear();

    foreach(ApiTraceError error, errors) {
        m_trace->setCallError(error);

        QTreeWidgetItem *item =
            new QTreeWidgetItem(m_ui.errorsTreeWidget);
        item->setData(0, Qt::DisplayRole, error.callIndex);
        item->setData(0, Qt::UserRole, error.callIndex);
        QString type = error.type;
        type[0] = type[0].toUpper();
        item->setData(1, Qt::DisplayRole, type);
        item->setData(2, Qt::DisplayRole, error.message);
    }
}

void MainWindow::slotErrorSelected(QTreeWidgetItem *current)
{
    if (current) {
        int callIndex =
            current->data(0, Qt::UserRole).toInt();
        m_trace->findCallIndex(callIndex);
    }
}

ApiTraceCall * MainWindow::selectedCall() const
{
    if (m_selectedEvent &&
        m_selectedEvent->type() == ApiTraceEvent::Call) {
        return static_cast<ApiTraceCall*>(m_selectedEvent);
    }
    return NULL;
}

void MainWindow::saveSelectedSurface()
{
    QTreeWidgetItem *item =
        m_ui.surfacesTreeWidget->currentItem();

    if (!item || !m_trace) {
        return;
    }

    QVariant var = item->data(0, Qt::UserRole);
    if (!var.isValid()) {
        return;
    }

    QImage img = var.value<QImage>();
    if (img.isNull()) {
        image::Image *traceImage = ApiSurface::imageFromData(var.value<QByteArray>());
        img = ApiSurface::qimageFromRawImage(traceImage);
        delete traceImage;
    }
    if (img.isNull()) {
        statusBar()->showMessage( "Failed to save image", 5000);
        return;
    }

    QString imageIndex;
    ApiTraceCall *call = selectedCall();
    if (call) {
        imageIndex = tr("_call_%1")
                     .arg(call->index());
    } else {
        ApiTraceFrame *frame = selectedFrame();
        if (frame) {
            imageIndex = tr("_frame_%1")
                         .arg(frame->number);
        }
    }

    //which of the surfaces are we saving
    QTreeWidgetItem *parent = item->parent();
    int parentIndex =
        m_ui.surfacesTreeWidget->indexOfTopLevelItem(parent);
    if (parentIndex < 0) {
        parentIndex = 0;
    }
    int childIndex = 0;
    if (parent) {
        childIndex = parent->indexOfChild(item);
    } else {
        childIndex = m_ui.surfacesTreeWidget->indexOfTopLevelItem(item);
    }


    QString fileName =
        tr("%1%2-%3_%4.png")
        .arg(m_trace->fileName())
        .arg(imageIndex)
        .arg(parentIndex)
        .arg(childIndex);
    //qDebug()<<"save "<<fileName;
    if (img.save(fileName, "PNG")) {
        statusBar()->showMessage( tr("Saved '%1'").arg(fileName), 5000);
    } else {
        statusBar()->showMessage( tr("Failed to save '%1'").arg(fileName), 5000);
    }
}

void MainWindow::exportBufferData()
{
    QByteArray data = m_vdataInterpreter->data();
    if (data.isEmpty())
        return;

    QString bufferIndex;
    ApiTraceCall *call = selectedCall();
    if (call) {
        bufferIndex = tr("_call_%1").arg(call->index());
    } else {
        ApiTraceFrame *frame = selectedFrame();
        if (frame) {
            bufferIndex = tr("_frame_%1")
                         .arg(frame->number);
        }
    }

    QString filename =
        tr("%1%2_buffer.raw")
        .arg(m_trace->fileName())
        .arg(bufferIndex);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        statusBar()->showMessage("Failed to save buffer data", 5000);
        return;
    }

    if (file.write(data) == -1) {
        statusBar()->showMessage("Failed to save buffer data", 5000);
        return;
    }

    file.close();

    statusBar()->showMessage( tr("Saved buffer to '%1'").arg(filename), 5000);
}

void MainWindow::loadProgess(int percent)
{
    m_progressBar->setValue(percent);
}

void MainWindow::slotSearchResult(const ApiTrace::SearchRequest &request,
                                  ApiTrace::SearchResult result,
                                  ApiTraceCall *call)
{
    switch (result) {
    case ApiTrace::SearchResult_NotFound:
        m_searchWidget->setFound(false);
        break;
    case ApiTrace::SearchResult_Found: {
        QModelIndex index = m_proxyModel->indexForCall(call);

        if (index.isValid()) {
            m_ui.callView->setCurrentIndex(index);
            m_searchWidget->setFound(true);
        } else {
            //call is filtered out, so continue searching but from the
            // filtered call
            if (!call) {
                qDebug()<<"Error: search success with no call";
                return;
            }
//            qDebug()<<"filtered! search from "<<call->searchText()
//                   <<", call idx = "<<call->index();

            if (request.direction == ApiTrace::SearchRequest::Next) {
                m_trace->findNext(call->parentFrame(), call,
                                  request.text, request.cs, request.useRegex);
            } else {
                assert(request.direction == ApiTrace::SearchRequest::Prev);
                m_trace->findPrev(call->parentFrame(), call,
                                  request.text, request.cs, request.useRegex);
            }
        }
    }
        break;
    case ApiTrace::SearchResult_Wrapped:
        m_searchWidget->setFound(false);
        break;
    }
}

ApiTraceFrame * MainWindow::currentFrame() const
{
    QModelIndex index = m_ui.callView->currentIndex();
    ApiTraceEvent *event = 0;

    if (!index.isValid()) {
        index = m_proxyModel->index(0, 0, QModelIndex());
        if (!index.isValid()) {
            qDebug()<<"no currently valid index";
            return 0;
        }
    }

    event = index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
    Q_ASSERT(event);
    if (!event) {
        return 0;
    }

    ApiTraceFrame *frame = 0;
    if (event->type() == ApiTraceCall::Frame) {
        frame = static_cast<ApiTraceFrame*>(event);
    }
    return frame;
}

ApiTraceCall * MainWindow::currentCall() const
{
    QModelIndex index = m_ui.callView->currentIndex();
    ApiTraceEvent *event = 0;

    if (!index.isValid()) {
        index = m_proxyModel->index(0, 0, QModelIndex());
        if (!index.isValid()) {
            qDebug()<<"no currently valid index";
            return 0;
        }
    }

    event = index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();
    Q_ASSERT(event);
    if (!event) {
        return 0;
    }

    ApiTraceCall *call = 0;
    if (event->type() == ApiTraceCall::Call) {
        call = static_cast<ApiTraceCall*>(event);
    }

    return call;

}

void MainWindow::slotFoundFrameStart(ApiTraceFrame *frame)
{
    Q_ASSERT(frame->isLoaded());
    if (!frame || frame->isEmpty()) {
        return;
    }

    QVector<ApiTraceCall*>::const_iterator itr;
    QVector<ApiTraceCall*> calls = frame->calls();

    itr = calls.constBegin();
    while (itr != calls.constEnd()) {
        ApiTraceCall *call = *itr;
        QModelIndex idx = m_proxyModel->indexForCall(call);
        if (idx.isValid()) {
            m_ui.callView->setCurrentIndex(idx);
            m_ui.callView->scrollTo(idx, QAbstractItemView::PositionAtTop);
            break;
        }
        ++itr;
    }
}

void MainWindow::slotFoundFrameEnd(ApiTraceFrame *frame)
{
    Q_ASSERT(frame->isLoaded());
    if (!frame || frame->isEmpty()) {
        return;
    }
    QVector<ApiTraceCall*>::const_iterator itr;
    QVector<ApiTraceCall*> calls = frame->calls();

    itr = calls.constEnd();
    do {
        --itr;
        ApiTraceCall *call = *itr;
        QModelIndex idx = m_proxyModel->indexForCall(call);
        if (idx.isValid()) {
            m_ui.callView->setCurrentIndex(idx);
            m_ui.callView->scrollTo(idx, QAbstractItemView::PositionAtBottom);
            break;
        }
    } while (itr != calls.constBegin());
}

void MainWindow::slotJumpToResult(ApiTraceCall *call)
{
    QModelIndex idx = m_proxyModel->indexForCall(call);
    if (idx.isValid()) {
        activateWindow();
        m_ui.callView->setFocus();
        m_ui.callView->setCurrentIndex(idx);
        m_ui.callView->scrollTo(idx, QAbstractItemView::PositionAtCenter);
    } else {
        statusBar()->showMessage(tr("Call has been filtered out."));
    }
}

void MainWindow::thumbnailCallback(void *object, int thumbnailIdx)
{
    //qDebug() << QLatin1String("debug: transfer from trace to retracer thumbnail index: ") << thumbnailIdx;
    MainWindow *mySelf = (MainWindow *) object;
    mySelf->m_retracer->addThumbnailToCapture(thumbnailIdx);
}

void MainWindow::addRecentLaunch(const RecentLaunch& launch)
{
    QList<RecentLaunch> launches = readRecentLaunches();

    for(int i = 0; i < launches.size(); ++i) {
        const RecentLaunch& rl = launches[i];

        if(rl.api == launch.api
                && rl.execPath == launch.execPath
                && rl.workingDir == launch.workingDir
                && rl.args == launch.args) {
            launches.removeAt(i);
        } else {
            ++i;
        }
    }

    launches.push_front(launch);

    const int maxHistorySize = 10;
    if(launches.size() > maxHistorySize) {
        launches.erase(launches.begin() + maxHistorySize, launches.end());
    }

    QSettings settings;
    settings.beginWriteArray("launch-history", launches.size());
    for(int i = 0; i < launches.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("api", launches[i].api);
        settings.setValue("exec", launches[i].execPath);
        settings.setValue("working-dir", launches[i].workingDir);
        settings.setValue("args", launches[i].args);
    }
    settings.endArray();
}

QList<MainWindow::RecentLaunch> MainWindow::readRecentLaunches() const
{
    QList<RecentLaunch> launches;
    QSettings settings;

    int historySize = settings.beginReadArray("launch-history");
    for(int i = 0; i < historySize; ++i) {
        settings.setArrayIndex(i);

        RecentLaunch rl;
        rl.api = settings.value("api").toString();
        rl.execPath = settings.value("exec").toString();
        rl.workingDir = settings.value("working-dir").toString();
        rl.args = settings.value("args").toStringList();
        launches.push_back(rl);
    }
    settings.endArray();

    return launches;
}

void MainWindow::updateRecentLaunchesMenu()
{
    QList<RecentLaunch> launches = readRecentLaunches();

    QMenu* rootMenu = m_ui.actionRecentLaunches->menu();
    if(!rootMenu) {
        rootMenu = new QMenu();
        m_ui.actionRecentLaunches->setMenu(rootMenu);
    }

    rootMenu->clear();
    for(int i = 0; i < launches.size(); ++i) {
        const RecentLaunch& rl = launches[i];
        QAction* action = rootMenu->addAction(rl.execPath);

        connect(action, &QAction::triggered, [this, i](){
            QList<RecentLaunch> launches = readRecentLaunches();
            if(i < launches.size())
                createTrace(&launches[i]);
        });
    }
}
