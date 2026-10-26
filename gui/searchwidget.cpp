#include "searchwidget.h"

#include <QDebug>
#include <QKeyEvent>
#include <QKeySequence>

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);

    m_ui.notFoundLabel->hide();
    m_origPalette = m_ui.lineEdit->palette();

    connect(m_ui.nextButton, &QAbstractButton::clicked,
            this, &SearchWidget::slotSearchNext);
    connect(m_ui.prevButton, &QAbstractButton::clicked,
            this, &SearchWidget::slotSearchPrev);
    connect(m_ui.closeButton, &QAbstractButton::clicked,
            this, &SearchWidget::slotCancel);
    connect(m_ui.lineEdit, &QLineEdit::returnPressed,
            this, &SearchWidget::slotSearchNext);

    m_ui.nextButton->setShortcut(
        QKeySequence::FindNext);
    m_ui.prevButton->setShortcut(
        QKeySequence::FindPrevious);

    installEventFilter(this);
}

void SearchWidget::slotSearchNext()
{
    QString txt = m_ui.lineEdit->text();
    if (!txt.isEmpty())
        emit searchNext(txt, caseSensitivity(), regexEnabled());
}

void SearchWidget::slotSearchPrev()
{
    QString txt = m_ui.lineEdit->text();
    if (!txt.isEmpty())
        emit searchPrev(txt, caseSensitivity(), regexEnabled());
}

void SearchWidget::slotCancel()
{
    hide();
}

void SearchWidget::showEvent(QShowEvent *event)
{
    return QWidget::showEvent(event);
}

Qt::CaseSensitivity SearchWidget::caseSensitivity() const
{
    if (m_ui.caseSensitiveBox->isChecked())
        return Qt::CaseSensitive;
    else
        return Qt::CaseInsensitive;
}

bool SearchWidget::regexEnabled() const
{
    return m_ui.regexBox->isChecked();
}

bool SearchWidget::eventFilter(QObject *object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        if ((static_cast<QKeyEvent*>(event))->key() == Qt::Key_Escape) {
            hide();
        }
    }
    return QWidget::eventFilter(object, event);
}

void SearchWidget::setFound(bool found)
{
    QPalette pal = m_origPalette;
    pal.setColor(QPalette::Active, QPalette::Base,
                 found ? Qt::white : QColor(255, 102, 102));
    m_ui.lineEdit->setPalette(pal);
    m_ui.notFoundLabel->setVisible(!found);
}

void SearchWidget::show()
{
    QWidget::show();
    m_ui.lineEdit->selectAll();
    m_ui.lineEdit->setFocus(Qt::ShortcutFocusReason);
    m_ui.lineEdit->setPalette(m_origPalette);
}
