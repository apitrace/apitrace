#pragma once

#include "ui_jumpwidget.h"
#include <QWidget>

class QKeyEvent;
class QShowEvent;

class JumpWidget : public QWidget
{
    Q_OBJECT
public:
    JumpWidget(QWidget *parent = 0);

signals:
    void jumpTo(int callNumber);

private slots:
    void slotJump();
    void slotCancel();

protected:
    virtual bool eventFilter(QObject *object, QEvent* event) override;
    virtual void showEvent(QShowEvent *event) override;

private:
    Ui_JumpWidget m_ui;
};
