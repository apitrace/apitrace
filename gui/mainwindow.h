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
class QLineEdit;
class QModelIndex;
class QProcess;
class QProgressBar;

namespace QJson {
    class Parser;
}

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
    void openTrace();
    void filterTrace();
    void replayStart();
    void replayStop();
    void replayFinished();
    void replayError(QProcess::ProcessError err);
    void startedLoadingTrace();
    void finishedLoadingTrace();
    void lookupState();
    void showSettings();

private:
    void newTraceFile(const QString &fileName);
    void replayTrace(bool dumpState);
    void parseState(const QVariantMap &params);
    void fillStateForFrame();

private:
    Ui_MainWindow m_ui;
    ApiTrace *m_trace;
    ApiTraceModel *m_model;
    ApiTraceFilter *m_proxyModel;
    QLineEdit *m_filterEdit;

    QProcess *m_replayProcess;

    QProgressBar *m_progressBar;

    QString m_traceFileName;

    ApiTraceEvent *m_selectedEvent;

    ApiTraceEvent *m_stateEvent;
    bool m_findingState;

    QJson::Parser *m_jsonParser;
};


#endif
