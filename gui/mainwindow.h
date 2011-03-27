#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>


class ApiTraceModel;
class ApiTraceFilter;
class QModelIndex;

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

private:
    Ui_MainWindow m_ui;
    ApiTraceModel *m_model;
    ApiTraceFilter *m_proxyModel;
};


#endif
