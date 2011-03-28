#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QProcess>


class ApiTraceModel;
class ApiTraceFilter;
class QLineEdit;
class QModelIndex;
class QProcess;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

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

private:
    void newTraceFile(const QString &fileName);

private:
    Ui_MainWindow m_ui;
    ApiTraceModel *m_model;
    ApiTraceFilter *m_proxyModel;
    QLineEdit *m_filterEdit;

    QProcess *m_replayProcess;

    QString m_traceFileName;
};


#endif
