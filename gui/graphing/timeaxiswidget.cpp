#include "timeaxiswidget.h"
#include "profiling.h"

#include <qmath.h>
#include <QPainter>

TimeAxisWidget::TimeAxisWidget(QWidget* parent) :
    GraphAxisWidget(parent)
{
}

void TimeAxisWidget::paintEvent(QPaintEvent *)
{
    /* TODO: Support selections? */
    /* TODO: Vertical scrolling? */

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    if (m_orientation == GraphAxisWidget::Vertical) {
        int axisHeight = height() - 1;
        int fontHeight = painter.fontMetrics().height();
        int ticks     = axisHeight / (fontHeight * 2);

        double range  = m_valueMax - m_valueMin;
        double step   = range / (double)ticks;
        double step10 = qPow(10.0, qFloor(qLn(step) / qLn(10.0)));
        step = qFloor((step / step10) * 2) * (step10 / 2);

        if (step <= 0) {
            return;
        }

        painter.resetTransform();

        for (double tick = 0; tick <= range; tick += step) {
            int y = axisHeight - ((tick / range) * axisHeight);

            painter.drawLine(width() - 8, y, width(), y);

            painter.drawText(0,
                             qBound(0, y - fontHeight / 2, axisHeight - fontHeight),
                             width() - 10,
                             fontHeight,
                             Qt::AlignRight | Qt::AlignVCenter,
                             Profiling::getTimeString(tick, m_valueMax));
        }
    } else {
        int axisWidth = width() - 1;
        int fontWidth = painter.fontMetrics().width("0.000 ns");
        int fontHeight= painter.fontMetrics().height();
        int ticks     = axisWidth / (fontWidth * 2);

        double range  = m_valueMax - m_valueMin;
        double step   = range / (double)ticks;
        double step10 = qPow(10.0, qFloor(qLn(step) / qLn(10.0)));
        step = qFloor((step / step10) * 2) * (step10 / 2);

        if (step <= 0) {
            return;
        }

        painter.resetTransform();

        for (double tick = 0; tick <= range; tick += step) {
            int x = (tick / range) * axisWidth;

            painter.drawLine(x, 0, x, 8);

            painter.drawText(qBound(0, x - fontWidth / 2, axisWidth - fontWidth),
                             8,
                             fontWidth,
                             fontHeight,
                             Qt::AlignHCenter | Qt::AlignTop,
                             Profiling::getTimeString(tick, m_valueMax));
        }
    }
}
