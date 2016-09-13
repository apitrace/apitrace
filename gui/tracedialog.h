#pragma once

#include "ui_tracedialog.h"
#include <QDialog>

class TraceDialog : public QDialog, public Ui_TraceDialog
{
    Q_OBJECT
public:
    TraceDialog(QWidget *parent = 0);

    void accept() override;

    void setApi(const QString &api);
    QString api() const;

    void setApplicationPath(const QString &path);
    QString applicationPath() const;

    void setWorkingDirPath(const QString &path);
    QString workingDirPath() const;

    void setArguments(const QStringList &args);
    QStringList arguments() const;

private slots:
    void browseApplication();
    void browseWorkingDir();

private:
    bool isFileOk(const QString &fileName);
    bool isDirOk(const QString &path);
};
