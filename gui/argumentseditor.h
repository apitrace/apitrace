#pragma once

#include "apitracecall.h"
#include "ui_argumentseditor.h"

#include <QComboBox>
#include <QDialog>
#include <QItemEditorFactory>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QTextEdit>

#include <limits.h>
#include <float.h>


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


class BooleanComboBoxEditorCreator : public BooleanComboBox
{
    Q_OBJECT
    Q_PROPERTY(bool value READ value WRITE setValue USER true)
public:
    BooleanComboBoxEditorCreator(QWidget *widget = 0) : BooleanComboBox(widget)
    {
	this->setFrame(false);
    };
};

class UIntEditorCreator : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue USER true)
public:
    UIntEditorCreator(QWidget *widget = 0) : QSpinBox(widget)
    {
	this->setFrame(false);
        this->setMaximum(INT_MAX);
    };
};

class IntEditorCreator : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue USER true)
public:
    IntEditorCreator(QWidget *widget = 0) : QSpinBox(widget)
    {
	this->setFrame(false);
        this->setMinimum(INT_MIN);
        this->setMaximum(INT_MAX);
    };
};

class ULongLongEditorCreator : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue USER true)
public:
    ULongLongEditorCreator(QWidget *widget = 0) : QSpinBox(widget)
    {
	this->setFrame(false);
        this->setMaximum(INT_MAX);
    };
};

class LongLongEditorCreator : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue USER true)
public:
    LongLongEditorCreator(QWidget *widget = 0) : QSpinBox(widget)
    {
	this->setFrame(false);
        this->setMinimum(INT_MIN);
        this->setMaximum(INT_MAX);
    };
};

class PixmapEditorCreator : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText USER true)
public:
    PixmapEditorCreator(QWidget *widget = 0) : QLabel (widget)
    {
    };
};

class FloatEditorCreator : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue USER true)
public:
    FloatEditorCreator(QWidget *widget = 0) : QDoubleSpinBox(widget)
    {
	this->setFrame(false);
        this->setMinimum(-FLT_MAX);
        this->setMaximum(FLT_MAX);
        this->setDecimals(8);
    };
};

class DoubleEditorCreator : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue USER true)
public:
    DoubleEditorCreator(QWidget *widget = 0) : QDoubleSpinBox(widget)
    {
	this->setFrame(false);
        this->setMinimum(-DBL_MAX);
        this->setMaximum(DBL_MAX);
        this->setDecimals(8);
    };
};

class StringEditorCreator : public QTextEdit
{
    Q_OBJECT
    Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText USER true)
public:
    StringEditorCreator(QWidget *widget = 0) : QTextEdit(widget)
    {
    };
};

class InvalidEditorCreator : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText USER true)
public:
    InvalidEditorCreator(QWidget *widget = 0) :  QLabel(widget)
    {
    };
};

class ArgumentsEditor : public QDialog
{
    Q_OBJECT
public:
    ArgumentsEditor(QWidget *parent=0);
    ~ArgumentsEditor();


    virtual void accept() override;

    void setCall(ApiTraceCall *call);
    ApiTraceCall *call() const;

private slots:
    void currentSourceChanged(int idx);
    void sourceChanged();
    void revert();
private:
    void init();
    void setupCall();
    void setupShaderEditor(const QVector<QVariant> &sources);
    QVariant valueForName(const QString &name,
                          const QVariant &orignalValue,
                          bool *changed) const;
    QVariant arrayFromIndex(const QModelIndex &index,
                            const ApiArray &array,
                            bool *changed) const;
    QVariant arrayFromEditor(const ApiArray &origArray,
                             bool *changed) const;
private:
    Ui_ArgumentsEditor m_ui;
    QStandardItemModel *m_model;

    ApiTraceCall *m_call;
};
