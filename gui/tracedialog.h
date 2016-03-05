#pragma once

#include "ui_tracedialog.h"
#include <QDialog>

class TraceDialog : public QDialog, public Ui_TraceDialog
{
    Q_OBJECT
public:
    TraceDialog(QWidget *parent = 0);

    void accept() override;

    QString api() const;
    QString applicationPath() const;
    QStringList arguments() const;

private slots:
    void browse();

private:
    bool isFileOk(const QString &fileName);
};
