#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QtGui/QMainWindow>

class ApiTraceModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

public slots:
    void loadTrace(const QString &fileName);

private slots:
    void openTrace();

private:
    Ui_MainWindow m_ui;
    ApiTraceModel *m_model;
};


#endif
