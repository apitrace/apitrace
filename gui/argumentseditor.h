#ifndef ARGUMENTSEDITOR_H
#define ARGUMENTSEDITOR_H

#include "ui_argumentseditor.h"
#include <QStandardItemModel>
#include <QWidget>

class ApiTraceCall;

class ArgumentsEditor : public QWidget
{
    Q_OBJECT
public:
    ArgumentsEditor(QWidget *parent=0);
    ~ArgumentsEditor();


    void setCall(ApiTraceCall *call);
    ApiTraceCall *call() const;

signals:
    void argumentsEdited(ApiTraceCall *call);

private:
    void init();
    void setupCall();
    void setupShaderEditor(const QList<QVariant> &sources);
private:
    Ui_ArgumentsEditor m_ui;
    QStandardItemModel *m_model;

    ApiTraceCall *m_call;
};

#endif
