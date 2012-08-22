#include "graphwidget.h"
#include "timelinewidget.h"
#include "profiledialog.h"

#include <qmath.h>
#include <QLocale>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include <QApplication>

typedef trace::Profile::Call Call;
typedef trace::Profile::Frame Frame;
typedef trace::Profile::Program Program;

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
      m_profile(0),
      m_axisWidth(80),
      m_axisHeight(30),
      m_axisForeground(Qt::black),
      m_axisBackground(Qt::lightGray)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

    m_graphGradientGpu.setColorAt(0.9, QColor(255, 0, 0));
    m_graphGradientGpu.setColorAt(0.0, QColor(255, 200, 200));

    m_graphGradientCpu.setColorAt(0.9, QColor(0, 0, 255));
    m_graphGradientCpu.setColorAt(0.0, QColor(200, 200, 255));
}


/**
 * Setup graph data from profile results
 */
void GraphWidget::setProfile(trace::Profile* profile, GraphType type)
{
    m_type = type;
    m_profile = profile;
    m_maxTime = 0;

    /* Find longest call to use as y axis */
    if (m_type == GraphGpu) {
        for (std::vector<Call>::const_iterator itr = m_profile->calls.begin(); itr != m_profile->calls.end(); ++itr) {
            const Call& call = *itr;

            if (call.gpuDuration > m_maxTime) {
                m_maxTime = call.gpuDuration;
            }
        }
    } else {
        for (std::vector<Call>::const_iterator itr = m_profile->calls.begin(); itr != m_profile->calls.end(); ++itr) {
            const Call& call = *itr;

            if (call.cpuDuration > m_maxTime) {
                m_maxTime = call.cpuDuration;
            }
        }
    }

    m_minCall = 0;
    m_maxCall = m_profile->calls.size();

    m_minCallWidth = 10;
    m_maxCallWidth = m_maxCall - m_minCall;

    m_call = m_minCall;
    m_callWidth = m_maxCallWidth;

    update();
}


/**
 * Slot to synchronise with other graph views
 */
void GraphWidget::changeView(int call, int width)
{
    m_call = call;
    m_callWidth = width;
    update();
}


/**
 * Calculate the maxTime variable when model is updated
 */
void GraphWidget::update()
{
    m_maxTime = 0;

    if (m_type == GraphGpu) {
        for (int i = m_call; i < m_call + m_callWidth; ++i) {
            const Call& call =  m_profile->calls[i];

            if (call.gpuDuration > m_maxTime) {
                m_maxTime = call.gpuDuration;
            }
        }
    } else {
        for (int i = m_call; i < m_call + m_callWidth; ++i) {
            const Call& call =  m_profile->calls[i];

            if (call.cpuDuration > m_maxTime) {
                m_maxTime = call.cpuDuration;
            }
        }
    }

    QWidget::update();
}


/**
 * Find the call at (x, y) position
 */
const Call* GraphWidget::callAtPosition(const QPoint& pos)
{
    int left, right, size;
    int64_t time;

    if (!m_profile) {
        return NULL;
    }

    int posX = qMax(0, pos.x() - m_axisWidth);
    int posY = qMax(0, pos.y() - m_axisHeight);

    time  = ((m_graphHeight - posY) / (double)m_graphHeight) * m_maxTime;
    time -= (2 * m_maxTime) / m_graphHeight;

    size  = m_callWidth / (double)m_graphWidth;

    left  = m_call + (posX / (double)m_graphWidth) * m_callWidth;
    left  = qMax(m_minCall, left - size);

    right = qMin(m_maxCall - 1, left + size * 2);

    if (m_type == GraphGpu) {
        const Call* longest = NULL;

        for (int i = left; i <= right; ++i) {
            const Call& call = m_profile->calls[i];

            if (call.pixels >= 0) {
                if (!longest || call.gpuDuration > longest->gpuDuration) {
                    longest = &call;
                }
            }
        }

        if (longest && time < longest->gpuDuration) {
            return longest;
        }
    } else {
        const Call* longest = NULL;

        for (int i = left; i <= right; ++i) {
            const Call& call = m_profile->calls[i];

            if (!longest || call.cpuDuration > longest->cpuDuration) {
                longest = &call;
            }
        }

        if (longest && time < longest->cpuDuration) {
            return longest;
        }
    }

    return NULL;
}


void GraphWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressPosition = e->pos();
        m_mousePressCall = m_call;
    }
}


void GraphWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    if (e->pos().x() < m_axisWidth || e->pos().y() < m_axisHeight) {
        return;
    }

    if (e->buttons().testFlag(Qt::LeftButton)) {
        /* Horizontal scroll */
        double dcdx = m_callWidth / (double)m_graphWidth;
        dcdx *= m_mousePressPosition.x() - e->pos().x();

        m_call = m_mousePressCall + dcdx;
        m_call = qBound(m_minCall, m_call, m_maxCall - m_callWidth);

        emit viewChanged(m_call, m_callWidth);
        update();
    }

    const Call* call = callAtPosition(e->pos());

    if (e->button() == Qt::NoButton && call) {
        QString text;
        text  = QString::fromStdString(call->name);
        text += QString("\nCall: %1").arg(call->no);
        text += QString("\nCPU Duration: %1").arg(getTimeString(call->cpuDuration));

        if (call->pixels >= 0) {
            text += QString("\nGPU Duration: %1").arg(getTimeString(call->gpuDuration));
            text += QString("\nPixels Drawn: %1").arg(QLocale::system().toString((qlonglong)call->pixels));
        }

        QToolTip::showText(e->globalPos(), text);
    } else {
        QToolTip::hideText();
    }
}


void GraphWidget::wheelEvent(QWheelEvent *e)
{
    if (!m_profile) {
        return;
    }

    if (e->pos().x() < m_axisWidth || e->pos().y() < m_axisHeight) {
        return;
    }

    int zoomPercent = 10;

    /* If holding Ctrl key then zoom 2x faster */
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        zoomPercent = 20;
    }

    /* Zoom view by adjusting width */
    double dt = m_callWidth;
    double size = dt * -e->delta();

    /* Zoom deltas normally come in increments of 120 */
    size /= 120 * (100 / zoomPercent);

    m_callWidth += size;
    m_callWidth = qBound(m_minCallWidth, m_callWidth, m_maxCallWidth);

    /* Scroll view to zoom around mouse */
    dt -= m_callWidth;
    dt *= e->x() - m_axisWidth;
    dt /= m_graphWidth;

    m_call = dt + m_call;
    m_call = qBound(m_minCall, m_call, m_maxCall - m_callWidth);

    emit viewChanged(m_call, m_callWidth);
    update();
}


void GraphWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    const Call* call = callAtPosition(e->pos());

    if (call) {
        emit jumpToCall(call->no);
    }
}


void GraphWidget::resizeEvent(QResizeEvent *e)
{
    m_graphWidth = qMax(0, width() - m_axisWidth);
    m_graphHeight = qMax(0, height() - m_axisHeight);

    m_graphGradientGpu.setStart(0, m_graphHeight);
    m_graphGradientCpu.setStart(0, m_graphHeight);
}


/**
 * Draw the vertical axis of time
 */
void GraphWidget::paintVerticalAxis(QPainter& painter)
{
    int height = painter.fontMetrics().height();
    int ticks  = m_graphHeight / (height * 2);

    double step   = m_maxTime / (double)ticks;
    double step10 = qPow(10.0, qFloor(qLn(step) / qLn(10.0)));
    step = qFloor((step / step10) * 2) * (step10 / 2);

    painter.resetTransform();
    painter.translate(0, m_axisHeight);
    painter.setPen(m_axisForeground);

    for (double tick = 0; tick <= m_maxTime; tick += step) {
        int y = m_graphHeight - ((tick / m_maxTime) * m_graphHeight);

        painter.drawLine(m_axisWidth - 8, y, m_axisWidth - 1, y);

        painter.drawText(0,
                         qBound(0, y - height / 2, m_graphHeight - height),
                         m_axisWidth - 10,
                         height,
                         Qt::AlignRight | Qt::AlignVCenter,
                         getTimeString(tick, m_maxTime));
    }
}


/**
 * Draw horizontal axis of frame numbers
 */
void GraphWidget::paintHorizontalAxis(QPainter& painter)
{
    double dxdc = m_graphWidth / (double)m_callWidth;
    double scroll = dxdc * m_call;
    int lastLabel = -9999;

    painter.resetTransform();
    painter.fillRect(0, 0, width(), m_axisHeight, m_axisBackground);
    painter.fillRect(0, 0, m_axisWidth, height(), m_axisBackground);

    painter.setPen(m_axisForeground);
    painter.drawLine(0, m_axisHeight - 1, width(), m_axisHeight - 1);
    painter.drawLine(m_axisWidth - 1, 0, m_axisWidth - 1, height());

    painter.translate(m_axisWidth, 0);

    for (std::vector<Frame>::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
        static const int padding = 4;
        const Frame& frame = *itr;
        bool draw = true;
        int width;

        if (frame.calls.begin > m_call + m_callWidth) {
            break;
        }

        if (frame.calls.end < m_call) {
            draw = false;
        }

        double left = dxdc * frame.calls.begin;
        double right = dxdc * frame.calls.end;
        QString text = QString("%1").arg(frame.no);

        width = painter.fontMetrics().width(text) + padding * 2;

        if (left + width > scroll)
            draw = true;

        /* Draw a frame number if we have space since the last one */
        if (left - lastLabel > width) {
            lastLabel = left + width;

            if (draw) {
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

                /* Draw frame number and major ruler marking */
                painter.drawText(textX + padding, 0, width - padding, m_axisHeight - 5, Qt::AlignLeft | Qt::AlignVCenter, text);
                painter.drawLine(left - scroll, m_axisHeight / 2, left - scroll, m_axisHeight - 1);
            }
        } else if (draw) {
            /* Draw a minor ruler marking */
            painter.drawLine(left - scroll, m_axisHeight - (m_axisHeight / 4), left - scroll, m_axisHeight - 1);
        }
    }
}


void GraphWidget::paintEvent(QPaintEvent *e)
{
    if (!m_profile) {
        return;
    }

    QPainter painter(this);
    QBrush brush;

    /* Draw axes */
    paintHorizontalAxis(painter);
    paintVerticalAxis(painter);

    /* Draw the label */
    painter.resetTransform();
    painter.fillRect(0, 0, m_axisWidth - 1, m_axisHeight - 1, Qt::lightGray);

    if (m_type == GraphGpu) {
        painter.drawText(0, 0, m_axisWidth, m_axisHeight, Qt::AlignHCenter | Qt::AlignVCenter, "GPU");
    } else {
        painter.drawText(0, 0, m_axisWidth, m_axisHeight, Qt::AlignHCenter | Qt::AlignVCenter, "CPU");
    }

    /* Draw graph */
    if (m_type == GraphGpu) {
        brush = QBrush(m_graphGradientGpu);
    } else {
        brush = QBrush(m_graphGradientCpu);
    }

    painter.setBrush(brush);
    painter.setPen(QPen(brush, 1));
    painter.translate(m_axisWidth, m_axisHeight);

    double x = 0;
    double dydt = m_graphHeight / (double)m_maxTime;
    double dxdc = m_graphWidth / (double)m_callWidth;

    if (dxdc < 1.0) {
        /* Less than 1 pixel per call, draw the longest call in a pixel */
        int64_t longest = 0;
        int lastX = 0;

        if (m_type == GraphGpu) {
            for (int i = m_call; i < m_call + m_callWidth; ++i) {
                const Call& call =  m_profile->calls[i];

                if (call.gpuDuration > longest) {
                    longest = call.gpuDuration;
                }

                x += dxdc;

                if (lastX != (int)x) {
                    painter.drawLine(lastX, m_graphHeight, lastX, m_graphHeight - (longest * dydt));
                    longest = 0;
                    lastX = x;
                }
            }
        } else {
            for (int i = m_call; i < m_call + m_callWidth; ++i) {
                const Call& call =  m_profile->calls[i];

                if (call.cpuDuration > longest) {
                    longest = call.cpuDuration;
                }

                x += dxdc;

                if (lastX != (int)x) {
                    painter.drawLine(lastX, m_graphHeight, lastX, m_graphHeight - (longest * dydt));
                    longest = 0;
                    lastX = x;
                }
            }
        }
    } else {
        /* At least 1 pixel per call, draw rects */
        if (m_type == GraphGpu) {
            for (int i = m_call; i < m_call + m_callWidth; ++i) {
                const Call& call =  m_profile->calls[i];

                if (call.pixels >= 0) {
                    int y = qMax<int>(1, call.gpuDuration * dydt);
                    painter.fillRect(QRectF(x, m_graphHeight - y, dxdc, y), brush);
                }

                x += dxdc;
            }
        } else {
            for (int i = m_call; i < m_call + m_callWidth; ++i) {
                const Call& call =  m_profile->calls[i];

                int y = qMax<int>(1, call.cpuDuration * dydt);
                painter.fillRect(QRectF(x, m_graphHeight - y, dxdc, y), brush);

                x += dxdc;
            }
        }
    }
}

#include "graphwidget.moc"
