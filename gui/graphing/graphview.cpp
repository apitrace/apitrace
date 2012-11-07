#include "graphview.h"

#include <QMouseEvent>
#include <QApplication>

GraphView::GraphView(QWidget* parent) :
    QWidget(parent),
    m_viewLeft(0),
    m_viewRight(0),
    m_viewBottom(0),
    m_viewTop(0),
    m_graphLeft(0),
    m_graphRight(0),
    m_graphBottom(0),
    m_graphTop(0),
    m_viewWidth(0),
    m_viewWidthMin(0),
    m_viewWidthMax(0),
    m_viewHeight(0),
    m_viewHeightMin(0),
    m_viewHeightMax(0),
    m_selectionState(NULL)
{
    memset(&m_previous, -1, sizeof(m_previous));
}

void GraphView::update()
{
    if (m_graphLeft != m_previous.m_graphLeft || m_graphRight != m_previous.m_graphRight) {
        m_previous.m_graphLeft = m_graphLeft;
        m_previous.m_graphRight = m_graphRight;

        emit horizontalRangeChanged(m_graphLeft, m_graphRight);
    }

    if (m_viewLeft != m_previous.m_viewLeft || m_viewRight != m_previous.m_viewRight) {
        m_previous.m_viewLeft = m_viewLeft;
        m_previous.m_viewRight = m_viewRight;

        emit horizontalViewChanged(m_viewLeft, m_viewRight);
    }

    if (m_graphBottom != m_previous.m_graphBottom || m_graphTop != m_previous.m_graphTop) {
        m_previous.m_graphBottom = m_graphBottom;
        m_previous.m_graphTop = m_graphTop;

        emit verticalRangeChanged(m_graphBottom, m_graphTop);
    }

    if (m_viewBottom != m_previous.m_viewBottom || m_viewTop != m_previous.m_viewTop) {
        m_previous.m_viewBottom = m_viewBottom;
        m_previous.m_viewTop = m_viewTop;

        emit verticalViewChanged(m_viewBottom, m_viewTop);
    }

    QWidget::update();
}

void GraphView::resizeEvent(QResizeEvent *)
{
    m_viewHeight = height();
    m_viewHeightMin = m_viewHeight;
    m_viewHeightMax = m_viewHeight;

    m_viewTop = m_viewBottom + m_viewHeight;

    update();
}

void GraphView::wheelEvent(QWheelEvent *e)
{
    int zoomPercent = 10;

    /* If holding Ctrl key then zoom 2x faster */
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        zoomPercent = 20;
    }

    /* Zoom view by adjusting width */
    double dt = m_viewWidth;
    double size = dt;
    size *= -e->delta();

    /* Zoom deltas normally come in increments of 120 */
    size /= 120 * (100 / zoomPercent);

    m_viewWidth += size;
    m_viewWidth = qBound(m_viewWidthMin, m_viewWidth, m_viewWidthMax);

    /* Scroll view to zoom around mouse */
    dt -= m_viewWidth;
    dt *= e->x();
    dt /= width();

    m_viewLeft = dt + m_viewLeft;
    m_viewLeft = qBound(m_graphLeft, m_viewLeft, m_graphRight - m_viewWidth);
    m_viewRight = m_viewLeft + m_viewWidth;

    update();
}

void GraphView::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons().testFlag(Qt::LeftButton)) {
        /* Horizontal scroll */
        double dvdx = m_viewWidth / (double)width();
        dvdx *= m_mousePressPosition.x() - e->pos().x();

        m_viewLeft = m_mousePressViewLeft + dvdx;
        m_viewLeft = qBound(m_graphLeft, m_viewLeft, m_graphRight - m_viewWidth);
        m_viewRight = m_viewLeft + m_viewWidth;

        /* Vertical scroll */
        double dvdy = m_viewHeight / (double)height();
        dvdy *= m_mousePressPosition.y() - e->pos().y();

        m_viewBottom = m_mousePressViewBottom + dvdy;
        m_viewBottom = qBound(m_graphBottom, m_viewBottom, m_graphTop - m_viewHeight);
        m_viewTop = m_viewBottom + m_viewHeight;

        update();
    }
}

void GraphView::mousePressEvent(QMouseEvent *e)
{
    m_mousePressPosition = e->pos();
    m_mousePressViewLeft = m_viewLeft;
    m_mousePressViewBottom = m_viewBottom;
}

void GraphView::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_selectionState) {
        m_selectionState->type = SelectionState::None;
        emit selectionChanged();
    }
}

void GraphView::setSelectionState(SelectionState* state)
{
    m_selectionState = state;
}

void GraphView::setHorizontalView(qint64 start, qint64 end)
{
    m_viewLeft = qBound(m_graphLeft, start, m_graphRight - (end - start));
    m_viewRight = qBound(m_graphLeft, end, m_graphRight);
    m_viewWidth = m_viewRight - m_viewLeft;
    update();
}

void GraphView::setVerticalView(qint64 start, qint64 end)
{
    m_viewBottom = qBound(m_graphBottom, start, m_graphTop - (end - start));
    m_viewTop = qBound(m_graphBottom, end, m_graphTop);
    m_viewHeight = m_viewTop - m_viewBottom;
    update();
}

void GraphView::setDefaultView(qint64 min, qint64 max)
{
    m_graphLeft = min;
    m_graphRight = max;
    m_viewWidth = max - min;

    m_viewWidthMin = 1;
    m_viewWidthMax = m_viewWidth;

    m_viewLeft = min;
    m_viewRight = max;

    m_viewHeight = height();
    m_viewHeightMin = m_viewHeight;
    m_viewHeightMax = m_viewHeight;

    m_viewBottom = 0;
    m_viewTop = m_viewHeight;

    m_graphBottom = 0;
    m_graphTop = m_viewHeight;

    update();
}

#include "graphview.moc"
