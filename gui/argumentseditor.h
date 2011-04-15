#ifndef ARGUMENTSEDITOR_H
#define ARGUMENTSEDITOR_H

#include "ui_argumentseditor.h"
#include <QComboBox>
#include <QItemEditorFactory>
#include <QStandardItemModel>
#include <QWidget>

class ApiTraceCall;

class BooleanComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(bool value READ value WRITE setValue USER true)
public:
    BooleanComboBox(QWidget *parent);
    void setValue(bool);
    bool value() const;
};

class ArgumentsItemEditorFactory : public QItemEditorFactory
{
public:
    ArgumentsItemEditorFactory();
    QWidget *createEditor(QVariant::Type type, QWidget *parent) const;
    QByteArray valuePropertyName(QVariant::Type) const;
};

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
