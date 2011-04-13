#ifndef JUMPWIDGET_H
#define JUMPWIDGET_H

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
    virtual bool eventFilter(QObject *object, QEvent* event);
    virtual void showEvent(QShowEvent *event);

private:
    Ui_JumpWidget m_ui;
};

#endif
