#include "heatmapview.h"

#include <qmath.h>
#include <QToolTip>
#include <QPainter>
#include <QMouseEvent>

HeatmapView::HeatmapView(QWidget* parent) :
    GraphView(parent),
    m_data(NULL)
{
    m_rowHeight = 20;
    setMouseTracking(true);
}


void HeatmapView::setDataProvider(HeatmapDataProvider* data)
{
    delete m_data;
    m_data = data;

    if (m_data) {
        m_data->setSelectionState(m_selectionState);

        setDefaultView(m_data->start(), m_data->end());

        m_viewWidthMin = 1000;

        m_graphBottom = 0;
        m_graphTop = (m_data->headerRows() + m_data->dataRows()) * m_rowHeight;
    } else {
        setDefaultView(0, 0);
        m_graphBottom = m_graphTop = 0;
    }

    update();
}


void HeatmapView::setSelectionState(SelectionState* state)
{
    if (m_data) {
        m_data->setSelectionState(state);
    }

    GraphView::setSelectionState(state);
}


void HeatmapView::mouseMoveEvent(QMouseEvent *e)
{
    GraphView::mouseMoveEvent(e);

    if (e->buttons() || !m_data) {
        QToolTip::hideText();
        return;
    }

    qint64 index = itemAtPosition(e->pos());

    if (index >= 0) {
        QToolTip::showText(e->globalPos(), m_data->itemTooltip(index));
    } else {
        QToolTip::hideText();
    }
}


void HeatmapView::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_data && e->button() == Qt::LeftButton) {
        qint64 index = itemAtPosition(e->pos());

        if (index >= 0) {
            m_data->itemDoubleClicked(index);
            return;
        }
    }

    GraphView::mouseDoubleClickEvent(e);
}


void HeatmapView::paintEvent(QPaintEvent *)
{
    if (!m_data) {
        return;
    }

    QPainter painter(this);
    painter.fillRect(0, m_data->headerRows() * m_rowHeight, width(), height(), Qt::white);

    /* Draw data rows */
    painter.translate(0, m_data->headerRows() * m_rowHeight - m_viewBottom % m_rowHeight);
    int rowStart = m_viewBottom / m_rowHeight;
    int rowEnd = qMin<int>(qCeil(m_viewTop / (double)m_rowHeight), m_data->dataRows());

    for (unsigned i = rowStart; i < rowEnd; ++i) {
        HeatmapRowIterator* itr = m_data->dataRowIterator(i, m_viewLeft, m_viewRight, width());
        paintRow(painter, itr);
        painter.translate(0, m_rowHeight);
        delete itr;
    }

    /* Draw Header */
    painter.resetTransform();
    painter.fillRect(0, 0, width(), m_data->headerRows() * m_rowHeight, Qt::white);

    for (unsigned i = 0; i < m_data->headerRows(); ++i) {
        HeatmapRowIterator* itr = m_data->headerRowIterator(i, m_viewLeft, m_viewRight, width());
        paintRow(painter, itr);
        painter.translate(0, m_rowHeight);
        delete itr;
    }

    /* Draw Axis Lines */
    painter.resetTransform();
    painter.setPen(Qt::black);
    painter.drawLine(0, m_rowHeight, width(), m_rowHeight);
    painter.drawLine(0, m_rowHeight * 2, width(), m_rowHeight * 2);

    painter.setPen(QColor(240, 240, 240));
    painter.translate(0, m_data->headerRows() * m_rowHeight - m_viewBottom % m_rowHeight);

    for (unsigned i = rowStart; i < rowEnd; ++i) {
        painter.drawLine(0, m_rowHeight, width(), m_rowHeight);
        painter.translate(0, m_rowHeight);
    }

    /* Draw selection borders */
    painter.resetTransform();
    painter.setPen(Qt::green);

    if (m_selectionState->type == SelectionState::Horizontal) {
        double dxdt = width() / (double)m_viewWidth;
        double scroll = m_viewLeft * dxdt;
        double left = (m_selectionState->start * dxdt) - scroll;
        double right = (m_selectionState->end * dxdt) - scroll;

        /* Highlight time */
        if (left >= 0 && left <= width()) {
            painter.drawLine(left, 0, left, height());
        }

        if (right >= 0 && right <= width()) {
            painter.drawLine(right, 0, right, height());
        }
    } else if (m_selectionState->type == SelectionState::Vertical) {
        /* Highlight row */
        int row = m_selectionState->start;

        for (unsigned i = rowStart; i < rowEnd; ++i) {
            if (m_data->dataRowAt(i) == row) {
                row = i - rowStart;

                painter.translate(0, m_data->headerRows() * m_rowHeight - m_viewBottom % m_rowHeight);
                painter.drawLine(0, (row + 1) * m_rowHeight, width(), (row + 1) * m_rowHeight);

                if (row > 0) {
                    painter.drawLine(0, row * m_rowHeight, width(), row * m_rowHeight);
                }

                break;
            }
        }
    }
}


void HeatmapView::paintRow(QPainter& painter, HeatmapRowIterator* itr)
{
    bool selection = m_selectionState ? m_selectionState->type != SelectionState::None : false;

    while (itr->next())
    {
        double heat = itr->heat();
        int width = itr->width();
        int x = itr->step();

        /* Gamma correction */
        heat = qPow(heat, 1.0 / 2.0);

        if (width == 1) {
            /* Draw single line */
            if (selection) {
                double selectedHeat = itr->selectedHeat();

                if (selectedHeat >= 0.999) {
                    heat = 255.0 - qBound(0.0, heat * 255.0, 255.0);

                    if (itr->isGpu()) {
                        painter.setPen(QColor(255, heat, heat));
                    } else {
                        painter.setPen(QColor(heat, heat, 255));
                    }

                    painter.drawLine(x, 0, x, m_rowHeight - 1);
                } else {
                    heat = 255.0 - qBound(0.0, heat * 100.0, 100.0);
                    painter.setPen(QColor(heat, heat, heat));
                    painter.drawLine(x, 0, x, m_rowHeight - 1);

                    if (selectedHeat > 0.001) {
                        selectedHeat = qPow(selectedHeat, 1.0 / 2.0);
                        selectedHeat = qBound(0.0, selectedHeat * 255.0, 255.0);

                        if (itr->isGpu()) {
                            painter.setPen(QColor(255, 0, 0, selectedHeat));
                        } else {
                            painter.setPen(QColor(0, 0, 255, selectedHeat));
                        }

                        painter.drawLine(x, 0, x, m_rowHeight - 1);
                    }
                }
            } else {
                heat = qBound(0.0, heat * 255.0, 255.0);

                if (itr->isGpu()) {
                    painter.setPen(QColor(255, 255 - heat, 255 - heat));
                } else {
                    painter.setPen(QColor(255 - heat, 255 - heat, 255));
                }

                painter.drawLine(x, 0, x, m_rowHeight - 1);
            }
        } else {
            double selectedHeat = itr->selectedHeat();

            if (selection && selectedHeat < 0.9) {
                painter.fillRect(x, 0, width, m_rowHeight, QColor(255 - 100, 255 - 100, 255 - 100));
            } else if (itr->isGpu()) {
                painter.fillRect(x, 0, width, m_rowHeight, QColor(255, 0, 0));
            } else {
                painter.fillRect(x, 0, width, m_rowHeight, QColor(0, 0, 255));
            }

            if (width > 6) {
                painter.setPen(Qt::white);
                QString elided = painter.fontMetrics().elidedText(itr->label(), Qt::ElideRight, width - 1);

                painter.drawText(x + 1, 0, width - 1, m_rowHeight - 1,
                                 Qt::AlignLeft | Qt::AlignVCenter,
                                 elided);
            }
        }
    }
}


qint64 HeatmapView::itemAtPosition(QPoint pos)
{
    if (!m_data) {
        return -1;
    }

    double t = m_viewWidth / (double)width();
    t *= pos.x();
    t += m_viewLeft;

    qint64 time = (qint64)t;
    qint64 index;

    if (pos.y() < m_data->headerRows() * m_rowHeight) {
        int row = pos.y() / m_rowHeight;
        index = m_data->headerItemAt(row, time);
    } else {
        int row = pos.y();
        row -= m_data->headerRows() * m_rowHeight;
        row += m_viewBottom;
        row /= m_rowHeight;
        index = m_data->dataItemAt(row, time);
    }

    return index;
}
