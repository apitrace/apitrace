#include "frameaxiswidget.h"

#include <QPainter>

FrameAxisWidget::FrameAxisWidget(QWidget* parent) :
    GraphAxisWidget(parent),
    m_data(NULL)
{
    setSelectable(GraphAxisWidget::Range);
}

void FrameAxisWidget::setDataProvider(FrameDataProvider* data)
{
    delete m_data;
    m_data = data;
}

void FrameAxisWidget::paintEvent(QPaintEvent *)
{
    if (!m_data || m_orientation != GraphAxisWidget::Horizontal) {
        /* TODO: Vertical axis support */
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    qint64 range = m_valueEnd - m_valueBegin;
    double dxdv = width() / (double)range;
    double scroll = dxdv * m_valueBegin;
    int lastLabel = -9999;

    /* Iterate over frames, drawing a label when there is space to do so */
    for (unsigned i = 0; i < m_data->size(); ++i) {
        static const int padding = 4;
        qint64 start = m_data->frameStart(i);
        qint64 end = m_data->frameEnd(i);
        bool visible = false;

        if (start > m_valueEnd) {
            break;
        }

        if (end < m_valueBegin) {
            visible = false;
        }

        double left = dxdv * start;
        double right = dxdv * end;
        QString text = QString("%1").arg(i);

        int width = painter.fontMetrics().width(text) + padding * 2;

        if (right > scroll) {
            visible = true;
        }

        if (left - lastLabel > width) {
            lastLabel = left + width;

            if (visible) {
                int textX;

                if (left < scroll && right - left > width) {
                    if (right - scroll > width) {
                        textX = 0;
                    } else {
                        textX = right - scroll - width;
                    }
                } else {
                    textX = left - scroll;
                }

                painter.drawText(textX + padding, 0, width - padding, height() - 5, Qt::AlignLeft | Qt::AlignVCenter, text);
                painter.drawLine(left - scroll, height() / 2, left - scroll, height() - 1);
            }
        } else if (visible) {
            painter.drawLine(left - scroll, height() * 3/4.0, left - scroll, height() - 1);
        }
    }

    /* Draw selection */
    if (hasSelection()) {
        double left = (dxdv * m_selectionState->start) - scroll;
        double right = (dxdv * m_selectionState->end) - scroll;

        painter.setPen(Qt::green);

        if (left >= 0 && left <= width()) {
            painter.drawLine(left, 0, left, height());
        }

        if (right >= 0 && right <= width()) {
            painter.drawLine(right, 0, right, height());
        }
    }
}
