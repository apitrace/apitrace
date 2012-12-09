#include "histogramview.h"

#include <QPen>
#include <QBrush>
#include <qmath.h>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>

HistogramView::HistogramView(QWidget* parent) :
    GraphView(parent),
    m_data(NULL)
{
    setMouseTracking(true);

    m_gradientUnselected.setColorAt(0.9, QColor(200, 200, 200));
    m_gradientUnselected.setColorAt(0.0, QColor(220, 220, 220));

    m_gradientSelected.setColorAt(0.9, QColor(0, 0, 210));
    m_gradientSelected.setColorAt(0.0, QColor(130, 130, 255));
}


void HistogramView::setDataProvider(GraphDataProvider* data)
{
    delete m_data;
    m_data = data;

    if (m_data) {
        m_data->setSelectionState(m_selectionState);
        setDefaultView(0, m_data->size());
        m_viewWidthMin = 10;
    } else {
        setDefaultView(0, 0);
    }
}


void HistogramView::setSelectionState(SelectionState* state)
{
    if (m_data) {
        m_data->setSelectionState(state);
    }

    GraphView::setSelectionState(state);
}


void HistogramView::setSelectedGradient(const QLinearGradient& gradient)
{
    m_gradientSelected = gradient;
}


void HistogramView::setUnelectedGradient(const QLinearGradient& gradient)
{
    m_gradientUnselected = gradient;
}


void HistogramView::mouseMoveEvent(QMouseEvent *e)
{
    GraphView::mouseMoveEvent(e);

    if (e->buttons() || !m_data) {
        QToolTip::hideText();
        return;
    }

    qint64 index = itemAtPosition(e->pos());
    qint64 time = valueAtPosition(e->pos());

    if (m_data->value(index) >= time) {
        QToolTip::showText(e->globalPos(), m_data->itemTooltip(index));
    } else {
        QToolTip::hideText();
    }
}


void HistogramView::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        qint64 index = itemAtPosition(e->pos());
        qint64 time = valueAtPosition(e->pos());

        if (m_data->value(index) >= time) {
            m_data->itemDoubleClicked(index);
            return;
        }
    }

    GraphView::mouseDoubleClickEvent(e);
}


void HistogramView::update()
{
    m_graphBottom = 0;
    m_graphTop = 0;

    if (m_data) {
        for (qint64 i = m_viewLeft; i < m_viewRight; ++i) {
            qint64 value = m_data->value(i);

            if (value > m_graphTop) {
                m_graphTop = value;
            }
        }
    }

    GraphView::update();
}


void HistogramView::resizeEvent(QResizeEvent *)
{
    m_gradientSelected.setStart(0, height());
    m_gradientUnselected.setStart(0, height());
}


/* Draw the histogram
 *
 * When the view is zoomed such that there is more than one item occupying a single pixel
 * the one with the highest value will be displayed.
 */
void HistogramView::paintEvent(QPaintEvent *)
{
    if (!m_data) {
        return;
    }

    QBrush selectedBrush = QBrush(m_gradientSelected);
    QPen selectedPen = QPen(selectedBrush, 1);

    QBrush unselectedBrush = QBrush(m_gradientUnselected);
    QPen unselectedPen = QPen(unselectedBrush, 1);

    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::white);

    double dydv = height() / (double)m_graphTop;
    double dxdv = width() / (double)(m_viewRight - m_viewLeft);
    bool selection = m_selectionState && m_selectionState->type != SelectionState::None;

    if (dxdv < 1.0) {
        /* Less than one pixel per item */
        qint64 longestValue = m_graphBottom;
        qint64 longestSelected = m_graphBottom;
        int lastX = 0;
        double x = 0;

        if (selection) {
            painter.setPen(unselectedPen);
        } else {
            painter.setPen(selectedPen);
        }

        for (qint64 i = m_viewLeft; i < m_viewRight; ++i) {
            qint64 value = m_data->value(i);
            int ix;

            if (value > longestValue) {
                longestValue = value;
            }

            if (selection && m_data->selected(i) && value > longestSelected) {
                longestSelected = value;
            }

            x += dxdv;
            ix = (int)x;

            if (lastX != ix) {
                painter.drawLine(lastX, height(), lastX, height() - (longestValue * dydv));

                if (selection && longestSelected > m_graphBottom) {
                    painter.setPen(selectedPen);
                    painter.drawLine(lastX, height(), lastX, height() - (longestSelected * dydv));
                    painter.setPen(unselectedPen);
                    longestSelected = m_graphBottom;
                }

                longestValue = m_graphBottom;
                lastX = ix;
            }
        }
    } else {
        /* Draw rectangles for graph */
        double x = 0;

        for (qint64 i = m_viewLeft; i < m_viewRight; ++i, x += dxdv) {
            qint64 value = m_data->value(i);
            int y = qMax<int>(1, value * dydv);

            if (!selection || m_data->selected(i)) {
                painter.fillRect(x, height() - y, dxdv, y, selectedBrush);
            } else {
                painter.fillRect(x, height() - y, dxdv, y, unselectedBrush);
            }
        }
    }

    /* Draw the borders for the selection */
    if (m_selectionState && m_selectionState->type == SelectionState::Horizontal) {
        double dxdt = width() / (double)m_viewWidth;
        double scroll = m_viewLeft * dxdt;
        double left = (m_selectionState->start * dxdt) - scroll;
        double right = (m_selectionState->end * dxdt) - scroll;

        painter.setPen(Qt::green);

        if (left >= 0 && left <= width()) {
            painter.drawLine(left, 0, left, height());
        }

        if (right >= 0 && right <= width()) {
            painter.drawLine(right, 0, right, height());
        }
    }
}


/* Find the item with the highest value at pos.x() +/- 1,
 * the mouse must be within the bar height-wise.
 */
qint64 HistogramView::itemAtPosition(QPoint pos) {
    double dvdx = m_viewWidth / (double)width();

    qint64 left = qFloor(dvdx * (pos.x() - 1)) + m_viewLeft;
    qint64 right = qCeil(dvdx * (pos.x() + 1)) + m_viewLeft;

    qint64 longestIndex = 0;
    qint64 longestValue = 0;

    left = qBound<qint64>(0, left, m_data->size() - 1);
    right = qBound<qint64>(0, right, m_data->size() - 1);

    for (qint64 i = left; i <= right; ++i) {
        if (m_data->value(i) > longestValue) {
            longestValue = m_data->value(i);
            longestIndex = i;
        }
    }

    return longestIndex;
}


/* Return the value at position */
qint64 HistogramView::valueAtPosition(QPoint pos) {
    double value = m_graphTop / (double)height();
    value *= height() - pos.y();
    value += m_graphBottom;
    return (qint64)value;
}

