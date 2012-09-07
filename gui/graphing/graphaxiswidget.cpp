#include "graphaxiswidget.h"

#include <QMouseEvent>

GraphAxisWidget::GraphAxisWidget(QWidget* parent) :
    QWidget(parent),
    m_selectable(None),
    m_selectionState(NULL)
{
}


bool GraphAxisWidget::hasSelection()
{
    if (!m_selectionState) {
        return false;
    }

    if (m_selectionState->type == SelectionState::Horizontal && m_orientation == GraphAxisWidget::Horizontal) {
        return true;
    }

    if (m_selectionState->type == SelectionState::Vertical && m_orientation == GraphAxisWidget::Vertical) {
        return true;
    }

    return false;
}


void GraphAxisWidget::setSelectable(SelectionStyle selectable)
{
    m_selectable = selectable;
}


void GraphAxisWidget::setSelectionState(SelectionState* state)
{
    m_selectionState = state;
}


void GraphAxisWidget::setOrientation(Orientation v)
{
    m_orientation = v;

    if (m_orientation == Horizontal) {
        setMinimumWidth(60);
    } else {
        setMinimumHeight(60);
    }
}


void GraphAxisWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_selectable == None) {
        return;
    }

    int pos, max;

    if (m_orientation == Horizontal) {
        pos = e->x();
        max = width();
    } else {
        pos = e->y();
        max = height();
    }

    double value = m_valueEnd - m_valueBegin;
    value *= pos / (double)max;
    value += m_valueBegin;

    if (e->buttons().testFlag(Qt::LeftButton)) {
        m_selectionState->start = qMin<qint64>(m_mousePressValue, value);
        m_selectionState->end = qMax<qint64>(m_mousePressValue, value);
        m_selectionState->type = m_orientation == Horizontal ? SelectionState::Horizontal : SelectionState::Vertical;
        emit selectionChanged();
        update();
    }
}


void GraphAxisWidget::mousePressEvent(QMouseEvent *e)
{
    if (m_selectable == None) {
        return;
    }

    int pos, max;

    if (m_orientation == Horizontal) {
        pos = e->x();
        max = width();
    } else {
        pos = e->y();
        max = height();
    }

    double value = m_valueEnd - m_valueBegin;
    value *= pos / (double)max;
    value += m_valueBegin;

    m_mousePressPosition = e->pos();
    m_mousePressValue = value;
}


void GraphAxisWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_selectable == None) {
        return;
    }

    int dx = qAbs(m_mousePressPosition.x() - e->x());
    int dy = qAbs(m_mousePressPosition.y() - e->y());

    if (dx + dy < 2) {
        m_selectionState->type = SelectionState::None;
        emit selectionChanged();
    }
}


void GraphAxisWidget::setRange(qint64 min, qint64 max)
{
    m_valueMin = min;
    m_valueMax = max;
    update();
}


void GraphAxisWidget::setView(qint64 start, qint64 end)
{
    m_valueBegin = start;
    m_valueEnd = end;
    update();
}

#include "graphaxiswidget.moc"
