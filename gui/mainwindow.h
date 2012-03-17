#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "trace_api.hpp"
#include "apitrace.h"

#include <QMainWindow>
#include <QProcess>

class ApiTrace;
class ApiTraceCall;
class ApiTraceEvent;
class ApiTraceFilter;
class ApiTraceFrame;
class ApiTraceModel;
class ApiTraceState;
class ArgumentsEditor;
class JumpWidget;
class QModelIndex;
class QProgressBar;
class QTreeWidgetItem;
class QUrl;
struct ApiTraceError;
class Retracer;
class SearchWidget;
class ShadersSourceWidget;
class TraceProcess;
class VertexDataInterpreter;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void loadTrace(const QString &fileName, int callNum = -1);

private slots:
    void callItemSelected(const QModelIndex &index);
    void callItemActivated(const QModelIndex &index);
    void createTrace();
    void openTrace();
    void replayStart();
    void replayStop();
    void replayFinished(const QString &output);
    void replayStateFound(ApiTraceState *state);
    void replayError(const QString &msg);
    void startedLoadingTrace();
    void loadProgess(int percent);
    void finishedLoadingTrace();
    void lookupState();
    void showSettings();
    void openHelp(const QUrl &url);
    void showSurfacesMenu(const QPoint &pos);
    void showSelectedSurface();
    void saveSelectedSurface();
    void slotGoTo();
    void slotJumpTo(int callNum);
    void createdTrace(const QString &path);
    void traceError(const QString &msg);
    void slotSearch();
    void slotSearchNext(const QString &str, Qt::CaseSensitivity sensitivity);
    void slotSearchPrev(const QString &str, Qt::CaseSensitivity sensitivity);
    void fillState(bool nonDefaults);
    void customContextMenuRequested(QPoint pos);
    void editCall();
    void slotStartedSaving();
    void slotSaved();
    void slotGoFrameStart();
    void slotGoFrameEnd();
    void slotTraceChanged(ApiTraceCall *call);
    void slotRetraceErrors(const QList<ApiTraceError> &errors);
    void slotErrorSelected(QTreeWidgetItem *current);
    void slotSearchResult(const ApiTrace::SearchRequest &request,
                          ApiTrace::SearchResult result,
                          ApiTraceCall *call);
    void slotFoundFrameStart(ApiTraceFrame *frame);
    void slotFoundFrameEnd(ApiTraceFrame *frame);
    void slotJumpToResult(ApiTraceCall *call);

private:
    void initObjects();
    void initConnections();
    void newTraceFile(const QString &fileName);
    void replayTrace(bool dumpState);
    void fillStateForFrame();

    /* there's a difference between selected frame/call and
     * current call/frame. the former implies actual selection
     * the latter might be just a highlight, e.g. during searching
     */
    ApiTraceFrame *selectedFrame() const;
    ApiTraceCall *selectedCall() const;
    ApiTraceFrame *currentFrame() const;
    ApiTraceCall *currentCall() const;


private:
    Ui_MainWindow m_ui;
    ShadersSourceWidget *m_sourcesWidget;

    trace::API m_api;

    ApiTrace *m_trace;
    ApiTraceModel *m_model;
    ApiTraceFilter *m_proxyModel;
    int m_initalCallNum;

    QProgressBar *m_progressBar;

    ApiTraceEvent *m_selectedEvent;

    ApiTraceEvent *m_stateEvent;

    Retracer *m_retracer;

    VertexDataInterpreter *m_vdataInterpreter;

    JumpWidget *m_jumpWidget;
    SearchWidget *m_searchWidget;

    TraceProcess *m_traceProcess;

    ArgumentsEditor *m_argsEditor;

    ApiTraceEvent *m_nonDefaultsLookupEvent;
};


#endif
