#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QProcess>

class ApiTrace;
class ApiTraceEvent;
class ApiTraceFilter;
class ApiTraceFrame;
class ApiTraceModel;
class ApiTraceState;
class ImageViewer;
class JumpWidget;
class QLineEdit;
class QModelIndex;
class QProgressBar;
class QUrl;
class Retracer;
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
    void loadTrace(const QString &fileName);

private slots:
    void callItemSelected(const QModelIndex &index);
    void createTrace();
    void openTrace();
    void filterTrace();
    void replayStart();
    void replayStop();
    void replayFinished(const QString &output);
    void replayStateFound(const ApiTraceState &state);
    void replayError(const QString &msg);
    void startedLoadingTrace();
    void finishedLoadingTrace();
    void lookupState();
    void showSettings();
    void openHelp(const QUrl &url);
    void showSurfacesMenu(const QPoint &pos);
    void showSelectedSurface();
    void slotGoTo();
    void slotJumpTo(int callNum);
    void createdTrace(const QString &path);
    void traceError(const QString &msg);

private:
    void initObjects();
    void initConnections();
    void newTraceFile(const QString &fileName);
    void replayTrace(bool dumpState);
    void fillStateForFrame();

private:
    Ui_MainWindow m_ui;
    ShadersSourceWidget *m_sourcesWidget;

    ApiTrace *m_trace;
    ApiTraceModel *m_model;
    ApiTraceFilter *m_proxyModel;
    QLineEdit *m_filterEdit;

    QProgressBar *m_progressBar;

    QString m_traceFileName;

    ApiTraceEvent *m_selectedEvent;

    ApiTraceEvent *m_stateEvent;

    Retracer *m_retracer;

    VertexDataInterpreter *m_vdataInterpreter;

    ImageViewer *m_imageViewer;

    JumpWidget *m_jumpWidget;

    TraceProcess *m_traceProcess;
};


#endif
