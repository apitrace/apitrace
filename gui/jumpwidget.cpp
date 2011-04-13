#include "jumpwidget.h"

#include <QDebug>
#include <QKeyEvent>

JumpWidget::JumpWidget(QWidget *parent )
    : QWidget(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.jumpButton, SIGNAL(clicked()),
            SLOT(slotJump()));
    connect(m_ui.spinBox, SIGNAL(editingFinished()),
            SLOT(slotJump()));
    connect(m_ui.cancelButton, SIGNAL(clicked()),
            SLOT(slotCancel()));

    installEventFilter(this);
}

void JumpWidget::slotJump()
{
    if (isVisible()) {
        emit jumpTo(m_ui.spinBox->value());
    }
    hide();
}

void JumpWidget::slotCancel()
{
    hide();
}

void JumpWidget::showEvent(QShowEvent *event)
{
    m_ui.spinBox->setFocus(Qt::ShortcutFocusReason);
    return QWidget::showEvent(event);
}

bool JumpWidget::eventFilter(QObject *object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        if ((static_cast<QKeyEvent*>(event))->key() == Qt::Key_Escape) {
            hide();
        }
    }
    return QWidget::eventFilter(object, event);
}

#include "jumpwidget.moc"
