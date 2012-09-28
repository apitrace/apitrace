#include "graphwidget.h"

#include <QScrollBar>

GraphWidget::GraphWidget(QWidget* parent) :
    QWidget(parent),
    m_view(NULL),
    m_label(NULL),
    m_axisTop(NULL),
    m_axisLeft(NULL),
    m_axisRight(NULL),
    m_axisBottom(NULL),
    m_horizontalScrollbar(NULL),
    m_horizontalMin(0),
    m_horizontalMax(0),
    m_horizontalStart(0),
    m_horizontalEnd(0),
    m_horizontalScrollbarPolicy(Qt::ScrollBarAlwaysOff),
    m_verticalScrollbar(NULL),
    m_verticalMin(0),
    m_verticalMax(0),
    m_verticalStart(0),
    m_verticalEnd(0),
    m_verticalScrollbarPolicy(Qt::ScrollBarAlwaysOff)
{
    m_selection.type = SelectionState::None;

    m_verticalScrollbar = new QScrollBar(this);
    m_verticalScrollbar->setOrientation(Qt::Vertical);
    m_verticalScrollbar->hide();
    m_verticalScrollbar->resize(m_verticalScrollbar->sizeHint());

    m_horizontalScrollbar = new QScrollBar(this);
    m_horizontalScrollbar->setOrientation(Qt::Horizontal);
    m_horizontalScrollbar->hide();
    m_horizontalScrollbar->resize(m_horizontalScrollbar->sizeHint());

    updateLayout();
    setAutoFillBackground(true);
}


GraphView* GraphWidget::view()
{
    return m_view;
}


GraphLabelWidget* GraphWidget::label()
{
    return m_label;
}


GraphAxisWidget* GraphWidget::axis(AxisPosition pos)
{
    switch(pos) {
        case AxisTop:
            return m_axisTop;

        case AxisLeft:
            return m_axisLeft;

        case AxisRight:
            return m_axisRight;

        case AxisBottom:
            return m_axisBottom;

        default:
            return NULL;
    }
}


void GraphWidget::setView(GraphView* view)
{
    delete m_view;
    m_view = view;

    updateConnections();

    m_view->setSelectionState(&m_selection);
    m_view->show();
    m_view->update();
}


void GraphWidget::setLabel(GraphLabelWidget* label)
{
    delete m_label;
    m_label = label;
}


void GraphWidget::setAxis(AxisPosition pos, GraphAxisWidget* axis)
{
    switch(pos) {
        case AxisTop:
            delete m_axisTop;
            m_axisTop = axis;
            m_axisTop->setOrientation(GraphAxisWidget::Horizontal);
            m_axisTop->setSelectionState(&m_selection);
            break;

        case AxisLeft:
            delete m_axisLeft;
            m_axisLeft = axis;
            m_axisLeft->setOrientation(GraphAxisWidget::Vertical);
            m_axisLeft->setSelectionState(&m_selection);
            break;

        case AxisRight:
            delete m_axisRight;
            m_axisRight = axis;
            m_axisRight->setOrientation(GraphAxisWidget::Vertical);
            m_axisRight->setSelectionState(&m_selection);
            break;

        case AxisBottom:
            delete m_axisBottom;
            m_axisBottom = axis;
            m_axisBottom->setOrientation(GraphAxisWidget::Horizontal);
            m_axisBottom->setSelectionState(&m_selection);
            break;
    }

    updateConnections();
    updateSelection();
    axis->show();
}


void GraphWidget::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    m_horizontalScrollbarPolicy = policy;
    updateScrollbars();
}


void GraphWidget::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    m_verticalScrollbarPolicy = policy;
    updateScrollbars();
}


void GraphWidget::resizeEvent(QResizeEvent *e)
{
    updateLayout();
    update();
}


/* Used if a selection would be shared between graphs with different axis */
SelectionState GraphWidget::transformSelectionIn(SelectionState state)
{
    return state;
}


/* Used if a selection would be shared between graphs with different axis */
SelectionState GraphWidget::transformSelectionOut(SelectionState state)
{
    return state;
}


/* Update the scrollbars based on current view */
void GraphWidget::updateScrollbars()
{
    /* Vertical scroll bar */
    qint64 size = (m_verticalMax - m_verticalMin) - (m_verticalEnd - m_verticalStart);

    if (size <= INT_MAX) {
        m_verticalScrollbar->setValue(m_verticalStart - m_verticalMin);
        m_verticalScrollbar->setPageStep(m_verticalEnd - m_verticalStart);
        m_verticalScrollbar->setRange(0, size);
    } else {
        /* QScrollBar only supports up to INT_MAX values,
         * here we must scale our values to match this */
        double curSize = m_verticalEnd - m_verticalStart;
        double pages = (m_verticalMax - m_verticalMin) / curSize;
        double value = (m_verticalStart - m_verticalMin) / curSize;

        m_verticalScrollbar->setValue(value);
        m_verticalScrollbar->setPageStep(1);
        m_verticalScrollbar->setRange(0, pages);
    }

    /* Adhere to scrollbar policy */
    bool visible = false;

    if (m_verticalScrollbarPolicy == Qt::ScrollBarAlwaysOn) {
        visible = true;
    } else if (m_verticalScrollbarPolicy == Qt::ScrollBarAlwaysOff) {
        visible = false;
    } else if (m_verticalScrollbarPolicy == Qt::ScrollBarAsNeeded) {
        visible = m_verticalMin != m_verticalStart || m_verticalMax != m_verticalEnd;
    }

    if (visible != m_verticalScrollbar->isVisible()) {
        m_verticalScrollbar->setVisible(visible);
        updateLayout();
    }

    /* Horizontal scroll bar */
    size = (m_horizontalMax - m_horizontalMin) - (m_horizontalEnd - m_horizontalStart);

    if (size <= INT_MAX) {
        m_horizontalScrollbar->setValue(m_horizontalStart - m_horizontalMin);
        m_horizontalScrollbar->setPageStep(m_horizontalEnd - m_horizontalStart);
        m_horizontalScrollbar->setRange(0, size);
    } else {
        /* QScrollBar only supports up to INT_MAX values,
         * here we must scale our values to match this */
        double dxdv = INT_MAX / (double)size;
        double value = (m_horizontalStart - m_horizontalMin) * dxdv;
        double pageStep = (m_horizontalEnd - m_horizontalStart) * dxdv;

        m_horizontalScrollbar->setValue((int)value);
        m_horizontalScrollbar->setPageStep((int)pageStep);
        m_horizontalScrollbar->setRange(0, INT_MAX);
    }

    /* Adhere to scrollbar policy */
    visible = false;

    if (m_horizontalScrollbarPolicy == Qt::ScrollBarAlwaysOn) {
        visible = true;
    } else if (m_horizontalScrollbarPolicy == Qt::ScrollBarAlwaysOff) {
        visible = false;
    } else if (m_horizontalScrollbarPolicy == Qt::ScrollBarAsNeeded) {
        visible =  m_horizontalMin != m_horizontalStart || m_horizontalMax != m_horizontalEnd;
    }

    if (visible != m_horizontalScrollbar->isVisible()) {
        m_horizontalScrollbar->setVisible(visible);
        updateLayout();
    }
}


/* Update all signal / slot connections */
void GraphWidget::updateConnections()
{
    if (m_view) {
        connect(m_view, SIGNAL(selectionChanged()), this, SLOT(updateSelection()), Qt::UniqueConnection);

        connect(m_view, SIGNAL(horizontalViewChanged(qint64,qint64)), this, SLOT(horizontalViewChange(qint64,qint64)), Qt::UniqueConnection);
        connect(m_view, SIGNAL(horizontalRangeChanged(qint64,qint64)), this, SLOT(horizontalRangeChange(qint64,qint64)), Qt::UniqueConnection);

        connect(m_view, SIGNAL(verticalViewChanged(qint64,qint64)), this, SLOT(verticalViewChange(qint64,qint64)), Qt::UniqueConnection);
        connect(m_view, SIGNAL(verticalRangeChanged(qint64,qint64)), this, SLOT(verticalRangeChange(qint64,qint64)), Qt::UniqueConnection);
    }

    if (m_axisTop) {
        if (m_view) {
            connect(m_view, SIGNAL(horizontalViewChanged(qint64,qint64)), m_axisTop, SLOT(setView(qint64,qint64)), Qt::UniqueConnection);
            connect(m_view, SIGNAL(horizontalRangeChanged(qint64,qint64)), m_axisTop, SLOT(setRange(qint64,qint64)), Qt::UniqueConnection);
        }

        connect(m_axisTop, SIGNAL(selectionChanged()), this, SLOT(updateSelection()), Qt::UniqueConnection);
    }

    if (m_axisLeft) {
        if (m_view) {
            connect(m_view, SIGNAL(verticalViewChanged(qint64,qint64)), m_axisLeft, SLOT(setView(qint64,qint64)), Qt::UniqueConnection);
            connect(m_view, SIGNAL(verticalRangeChanged(qint64,qint64)), m_axisLeft, SLOT(setRange(qint64,qint64)), Qt::UniqueConnection);
        }

        connect(m_axisLeft, SIGNAL(selectionChanged()), this, SLOT(updateSelection()), Qt::UniqueConnection);
    }

    if (m_axisRight) {
        if (m_view) {
            connect(m_view, SIGNAL(verticalViewChanged(qint64,qint64)), m_axisRight, SLOT(setView(qint64,qint64)), Qt::UniqueConnection);
            connect(m_view, SIGNAL(verticalRangeChanged(qint64,qint64)), m_axisRight, SLOT(setRange(qint64,qint64)), Qt::UniqueConnection);
        }

        connect(m_axisRight, SIGNAL(selectionChanged()), this, SLOT(updateSelection()), Qt::UniqueConnection);
    }

    if (m_axisBottom) {
        if (m_view) {
            connect(m_view, SIGNAL(horizontalViewChanged(qint64,qint64)), m_axisBottom, SLOT(setView(qint64,qint64)), Qt::UniqueConnection);
            connect(m_view, SIGNAL(horizontalRangeChanged(qint64,qint64)), m_axisBottom, SLOT(setRange(qint64,qint64)), Qt::UniqueConnection);
        }

        connect(m_axisBottom, SIGNAL(selectionChanged()), this, SLOT(updateSelection()), Qt::UniqueConnection);
    }

    if (m_horizontalScrollbar) {
        connect(m_horizontalScrollbar, SIGNAL(actionTriggered(int)), this, SLOT(horizontalScrollAction(int)));
    }

    if (m_verticalScrollbar) {
        connect(m_verticalScrollbar, SIGNAL(actionTriggered(int)), this, SLOT(verticalScrollAction(int)));
    }
}


/* Recalculate the layout */
void GraphWidget::updateLayout()
{
    int x, y;
    int padX = 0, padY = 0;

    if (m_axisTop) {
        padY += m_axisTop->height();
    }

    if (m_axisBottom) {
        padY += m_axisBottom->height();
    }

    if (m_axisLeft) {
        padX += m_axisLeft->width();
    }

    if (m_axisRight) {
        padX += m_axisRight->width();
    }

    if (m_horizontalScrollbar->isVisible()) {
        padY += m_horizontalScrollbar->height();
    }

    if (m_verticalScrollbar->isVisible()) {
        padX += m_verticalScrollbar->width();
    }

    if (m_axisTop) {
        x = m_axisLeft ? m_axisLeft->width() : 0;
        y = 0;

        m_axisTop->move(x, y);
        m_axisTop->resize(width() - padX, m_axisTop->height());
    }

    if (m_axisBottom) {
        x = m_axisLeft ? m_axisLeft->width() : 0;
        y = height() - m_axisBottom->height();

        if (m_horizontalScrollbar->isVisible())  {
            y -= m_horizontalScrollbar->height();
        }

        m_axisBottom->move(x, y);
        m_axisBottom->resize(width() - padX, m_axisBottom->height());
    }

    if (m_axisLeft) {
        x = 0;
        y = m_axisTop ? m_axisTop->height() : 0;

        m_axisLeft->move(x, y);
        m_axisLeft->resize(m_axisLeft->width(), height() - padY);
    }

    if (m_axisRight) {
        x = width() - m_axisRight->width();
        y = m_axisTop ? m_axisTop->height() : 0;

        if (m_verticalScrollbar->isVisible())  {
            x -= m_verticalScrollbar->width();
        }

        m_axisRight->move(x, y);
        m_axisRight->resize(m_axisRight->width(), height() - padY);
    }

    if (m_view) {
        x = m_axisLeft ? m_axisLeft->width() : 0;
        y = m_axisTop ? m_axisTop->height() : 0;

        m_view->move(x, y);
        m_view->resize(width() - padX, height() - padY);
    }

    if (m_label) {
        if (m_axisTop && m_axisLeft) {
            m_label->move(0, 0);
            m_label->resize(m_axisLeft->width(), m_axisTop->height());
        }
    }

    if (m_verticalScrollbar) {
        m_verticalScrollbar->move(width() - m_verticalScrollbar->width(), 0);

        if (m_horizontalScrollbar) {
            m_verticalScrollbar->resize(m_verticalScrollbar->width(), height() - m_horizontalScrollbar->height());
        } else {
            m_verticalScrollbar->resize(m_verticalScrollbar->width(), height());
        }
    }

    if (m_horizontalScrollbar) {
        m_horizontalScrollbar->move(0, height() - m_horizontalScrollbar->height());

        if (m_verticalScrollbar) {
            m_horizontalScrollbar->resize(width() - m_verticalScrollbar->width(), m_horizontalScrollbar->height());
        } else {
            m_horizontalScrollbar->resize(width(), m_horizontalScrollbar->height());
        }
    }
}


void GraphWidget::setSelection(SelectionState state)
{
    m_selection = transformSelectionIn(state);
    updateSelection(false);
}


void GraphWidget::setHorizontalView(qint64 start, qint64 end)
{
    if (m_view) {
        m_view->setHorizontalView(start, end);
    }
}


void GraphWidget::setVerticalView(qint64 start, qint64 end)
{
    if (m_view) {
        m_view->setVerticalView(start, end);
    }
}


/* Called when the view is translated / zoomed */
void GraphWidget::verticalViewChange(qint64 start, qint64 end)
{
    m_verticalStart = start;
    m_verticalEnd = end;
    updateScrollbars();

    emit verticalViewChanged(start, end);
}


void GraphWidget::verticalRangeChange(qint64 start, qint64 end)
{
    m_verticalMin = start;
    m_verticalMax = end;
    updateScrollbars();

    emit verticalRangeChanged(start, end);
}


void GraphWidget::horizontalViewChange(qint64 start, qint64 end)
{
    m_horizontalStart = start;
    m_horizontalEnd = end;
    updateScrollbars();

    emit horizontalViewChanged(start, end);
}


void GraphWidget::horizontalRangeChange(qint64 start, qint64 end)
{
    m_horizontalMin = start;
    m_horizontalMax = end;
    updateScrollbars();

    emit horizontalRangeChanged(start, end);
}


/* User interaction with horizontal scroll bar */
void GraphWidget::horizontalScrollAction(int /*action*/)
{
    int value = m_horizontalScrollbar->sliderPosition();
    qint64 size = (m_horizontalMax - m_horizontalMin) - (m_horizontalEnd - m_horizontalStart);

    /* Calculate the new scroll values */
    if (size <= INT_MAX) {
        m_horizontalEnd -= m_horizontalStart;
        m_horizontalStart = value + m_horizontalMin;
        m_horizontalEnd += value;
    } else {
        /* QScrollBar only supports up to INT_MAX values, here we must scale
         * our values to match this */
        double dxdv = INT_MAX / (double)size;

        size = m_horizontalEnd - m_horizontalStart;
        m_horizontalStart = value / dxdv + m_horizontalMin;
        m_horizontalEnd = m_horizontalStart + size;
    }

    /* Update view */
    if (m_view) {
        m_view->setHorizontalView(m_horizontalStart, m_horizontalEnd);
    }

    /* Update horizontal axes */
    if (m_axisTop) {
        m_axisTop->setView(m_horizontalStart, m_horizontalEnd);
    }

    if (m_axisBottom) {
        m_axisBottom->setView(m_horizontalStart, m_horizontalEnd);
    }

    /* Inform the world of our changes! */
    emit horizontalViewChanged(m_horizontalStart, m_horizontalEnd);
}


/* User interaction with vertical scroll bar */
void GraphWidget::verticalScrollAction(int /*action*/)
{
    int value = m_verticalScrollbar->sliderPosition();
    qint64 size = (m_verticalMax - m_verticalMin) - (m_verticalEnd - m_verticalStart);

    /* Calculate the new scroll values */
    if (size <= INT_MAX) {
        m_verticalEnd -= m_verticalStart;
        m_verticalStart = value + m_verticalMin;
        m_verticalEnd += value;
    } else {
        /* QScrollBar only supports up to INT_MAX values, here we must scale
         * our values to match this */
        double dxdv = INT_MAX / (double)size;

        size = m_verticalEnd - m_verticalStart;
        m_verticalStart = value / dxdv + m_verticalMin;
        m_verticalEnd = m_verticalStart + size;
    }

    /* Update view */
    if (m_view) {
        m_view->setVerticalView(m_verticalStart, m_verticalEnd);
    }

    /* Update vertical axes */
    if (m_axisLeft) {
        m_axisLeft->setView(m_verticalStart, m_verticalEnd);
    }

    if (m_axisRight) {
        m_axisRight->setView(m_verticalStart, m_verticalEnd);
    }

    /* Inform the world of our changes! */
    emit verticalViewChanged(m_verticalStart, m_verticalEnd);
}


/* Update child elements when selection changes */
void GraphWidget::updateSelection(bool emitSignal)
{
    if (m_view) {
        m_view->update();
    }

    if (m_axisTop) {
        m_axisTop->update();
    }

    if (m_axisLeft) {
        m_axisLeft->update();
    }

    if (m_axisRight) {
        m_axisRight->update();
    }

    if (m_axisBottom) {
        m_axisBottom->update();
    }

    if (emitSignal) {
        emit selectionChanged(transformSelectionOut(m_selection));
    }
}


#include "graphwidget.moc"
