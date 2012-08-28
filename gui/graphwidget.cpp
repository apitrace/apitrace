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
    m_selection.type = SelectNone;

    m_graphGradientGpu.setColorAt(0.9, QColor(210, 0, 0));
    m_graphGradientGpu.setColorAt(0.0, QColor(255, 130, 130));

    m_graphGradientCpu.setColorAt(0.9, QColor(0, 0, 210));
    m_graphGradientCpu.setColorAt(0.0, QColor(130, 130, 255));

    m_graphGradientDeselected.setColorAt(0.9, QColor(200, 200, 200));
    m_graphGradientDeselected.setColorAt(0.0, QColor(220, 220, 220));

    setMouseTracking(true);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);
}


/**
 * Setup graph data from profile results
 */
void GraphWidget::setProfile(trace::Profile* profile, GraphType type)
{
    m_type = type;
    m_profile = profile;
    m_timeMax = 0;

    /* Find longest call to use as y axis */
    if (m_type == GraphGpu) {
        for (std::vector<Call>::const_iterator itr = m_profile->calls.begin(); itr != m_profile->calls.end(); ++itr) {
            const Call& call = *itr;

            if (call.gpuDuration > m_timeMax) {
                m_timeMax = call.gpuDuration;
            }
        }
    } else {
        for (std::vector<Call>::const_iterator itr = m_profile->calls.begin(); itr != m_profile->calls.end(); ++itr) {
            const Call& call = *itr;

            if (call.cpuDuration > m_timeMax) {
                m_timeMax = call.cpuDuration;
            }
        }
    }

    m_callMin = 0;
    m_callMax = m_profile->calls.size();

    m_callWidthMin = 10;
    m_callWidthMax = m_callMax - m_callMin;

    m_call = m_callMin;
    m_callWidth = m_callWidthMax;

    selectNone();
    update();
}


/**
 * Set selection to nothing
 */
void GraphWidget::selectNone(bool notify)
{
    m_selection.type = SelectNone;

    if (notify) {
        emit selectedNone();
    }

    update();
}


/**
 * Set selection to a time period
 */
void GraphWidget::selectTime(int64_t start, int64_t end, bool notify)
{
    m_selection.timeStart = start;
    m_selection.timeEnd = end;
    m_selection.type = (start == end) ? SelectNone : SelectTime;

    if (notify) {
        emit selectedTime(start, end);
    }

    update();
}


/**
 * Set selection to a program
 */
void GraphWidget::selectProgram(unsigned program, bool notify)
{
    m_selection.program = program;
    m_selection.type = SelectProgram;

    if (notify) {
        emit selectedProgram(program);
    }

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
    m_timeMax = 0;

    if (m_type == GraphGpu) {
        for (int i = m_call; i < m_call + m_callWidth; ++i) {
            const Call& call =  m_profile->calls[i];

            if (call.gpuDuration > m_timeMax) {
                m_timeMax = call.gpuDuration;
            }
        }
    } else {
        for (int i = m_call; i < m_call + m_callWidth; ++i) {
            const Call& call =  m_profile->calls[i];

            if (call.cpuDuration > m_timeMax) {
                m_timeMax = call.cpuDuration;
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

    time  = ((m_graphHeight - posY) / (double)m_graphHeight) * m_timeMax;
    time -= (2 * m_timeMax) / m_graphHeight;

    size  = m_callWidth / (double)m_graphWidth;

    left  = m_call + (posX / (double)m_graphWidth) * m_callWidth;
    left  = qMax(m_callMin, left - size);

    right = qMin(m_callMax - 1, left + size * 2);

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


void GraphWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    if (e->button() == Qt::LeftButton) {
        int dxy = qAbs(m_mousePressPosition.x() - e->pos().x()) + qAbs(m_mousePressPosition.y() - e->pos().y());

        if (dxy <= 2) {
            int x = qMax(m_axisWidth, e->pos().x());
            double dcdx = m_callWidth / (double)m_graphWidth;

            int call = m_mousePressCall + dcdx * (x - m_axisWidth);

            int64_t start = m_profile->calls[call].cpuStart;
            int64_t end = m_profile->calls[call].cpuStart + m_profile->calls[call].cpuDuration;

            if (start < m_selection.timeStart || end > m_selection.timeEnd) {
                selectNone(true);
            }
        }
    }
}


void GraphWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    const Call* call = callAtPosition(e->pos());

    if (call) {
        emit jumpToCall(call->no);
    }
}


void GraphWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    if (e->buttons().testFlag(Qt::LeftButton)) {
        double dcdx = m_callWidth / (double)m_graphWidth;

        if (m_mousePressPosition.y() > m_axisHeight) {
            dcdx *= m_mousePressPosition.x() - e->pos().x();

            /* Horizontal scroll */
            m_call = m_mousePressCall + dcdx;
            m_call = qBound(m_callMin, m_call, m_callMax - m_callWidth);

            emit viewChanged(m_call, m_callWidth);
            update();
        } else {
            int x = qMax(m_axisWidth, e->pos().x());

            int down = m_mousePressCall + dcdx * (m_mousePressPosition.x() - m_axisWidth);
            int up = m_mousePressCall + dcdx * (x - m_axisWidth);

            int left = qMax(qMin(down, up), 0);
            int right = qMin<int>(qMax(down, up), m_profile->calls.size() - 1);

            selectTime(m_profile->calls[left].cpuStart, m_profile->calls[right].cpuStart + m_profile->calls[right].cpuDuration, true);
        }
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
            text += QString("\nProgram: %1").arg(call->program);
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

    if (e->pos().x() < m_axisWidth) {
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
    m_callWidth = qBound(m_callWidthMin, m_callWidth, m_callWidthMax);

    /* Scroll view to zoom around mouse */
    dt -= m_callWidth;
    dt *= e->x() - m_axisWidth;
    dt /= m_graphWidth;

    m_call = dt + m_call;
    m_call = qBound(m_callMin, m_call, m_callMax - m_callWidth);

    emit viewChanged(m_call, m_callWidth);
    update();
}


void GraphWidget::resizeEvent(QResizeEvent *e)
{
    m_graphWidth = qMax(0, width() - m_axisWidth);
    m_graphHeight = qMax(0, height() - m_axisHeight);

    m_graphGradientGpu.setStart(0, m_graphHeight);
    m_graphGradientCpu.setStart(0, m_graphHeight);
    m_graphGradientDeselected.setStart(0, m_graphHeight);
}


/**
 * Draw the vertical axis of time
 */
void GraphWidget::paintVerticalAxis(QPainter& painter)
{
    int height = painter.fontMetrics().height();
    int ticks  = m_graphHeight / (height * 2);

    double step   = m_timeMax / (double)ticks;
    double step10 = qPow(10.0, qFloor(qLn(step) / qLn(10.0)));
    step = qFloor((step / step10) * 2) * (step10 / 2);

    if (step <= 0) {
        return;
    }

    painter.resetTransform();
    painter.translate(0, m_axisHeight);
    painter.setPen(m_axisForeground);

    for (double tick = 0; tick <= m_timeMax; tick += step) {
        int y = m_graphHeight - ((tick / m_timeMax) * m_graphHeight);

        painter.drawLine(m_axisWidth - 8, y, m_axisWidth - 1, y);

        painter.drawText(0,
                         qBound(0, y - height / 2, m_graphHeight - height),
                         m_axisWidth - 10,
                         height,
                         Qt::AlignRight | Qt::AlignVCenter,
                         getTimeString(tick, m_timeMax));
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
    QBrush deselectBrush;
    QPen deselectPen;
    QBrush brush;
    QPen pen;

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
    deselectBrush = QBrush(m_graphGradientDeselected);

    if (m_type == GraphGpu) {
        brush = QBrush(m_graphGradientGpu);
    } else {
        brush = QBrush(m_graphGradientCpu);
    }

    pen = QPen(brush, 1);
    deselectPen = QPen(deselectBrush, 1);

    painter.setBrush(brush);
    painter.setPen(pen);
    painter.translate(m_axisWidth, m_axisHeight);

    double x = 0;
    double dydt = m_graphHeight / (double)m_timeMax;
    double dxdc = m_graphWidth / (double)m_callWidth;

    int selectLeft = m_call + m_callWidth;
    int selectRight = -1;

    if (m_selection.type == SelectProgram) {
        painter.setPen(deselectPen);
    }

    if (dxdc < 1.0) {
        /* Draw the longest call in a pixel */
        int64_t selectedLongest = 0;
        int64_t pixelLongest = 0;
        int lastX = 0;

        for (int i = m_call; i < m_call + m_callWidth; ++i) {
            const Call& call =  m_profile->calls[i];
            int ix;

            if (m_type == GraphGpu) {
                if (call.gpuDuration > pixelLongest) {
                    pixelLongest = call.gpuDuration;
                }

                if (m_selection.type == SelectProgram && call.program == m_selection.program) {
                    if (call.gpuDuration > selectedLongest) {
                        selectedLongest = call.gpuDuration;
                    }
                }
            } else {
                if (call.cpuDuration > pixelLongest) {
                    pixelLongest = call.cpuDuration;
                }

                if (m_selection.type == SelectProgram && call.program == m_selection.program) {
                    if (call.cpuDuration > selectedLongest) {
                        selectedLongest = call.cpuDuration;
                    }
                }
            }

            x += dxdc;
            ix = (int)x;

            if (lastX != ix) {
                if (m_selection.type == SelectTime) {
                    if (call.cpuStart < m_selection.timeStart || call.cpuStart > m_selection.timeEnd) {
                        painter.setPen(deselectPen);
                    } else {
                        if (ix < selectLeft) {
                            selectLeft = ix;
                        }

                        if (ix > selectRight) {
                            selectRight = ix;
                        }

                        painter.setPen(pen);
                    }
                }

                painter.drawLine(lastX, m_graphHeight, lastX, m_graphHeight - (pixelLongest * dydt));
                pixelLongest = 0;

                if (selectedLongest > 0) {
                    painter.setPen(pen);
                    painter.drawLine(lastX, m_graphHeight, lastX, m_graphHeight - (selectedLongest * dydt));
                    painter.setPen(deselectPen);
                    selectedLongest = 0;
                }

                lastX = ix;
            }
        }
    } else {
        /* Draw rectangles for graph */
        for (int i = m_call; i < m_call + m_callWidth; ++i, x += dxdc) {
            const Call& call =  m_profile->calls[i];
            int y;

            if (m_type == GraphGpu) {
                y = qMax<int>(1, call.gpuDuration * dydt);
            } else {
                y = qMax<int>(1, call.cpuDuration * dydt);
            }

            if (m_selection.type == SelectTime) {
                if (call.cpuStart < m_selection.timeStart || call.cpuStart > m_selection.timeEnd) {
                    if (m_type == GraphCpu || call.pixels >= 0) {
                        painter.fillRect(QRectF(x, m_graphHeight - y, dxdc, y), deselectBrush);
                    }

                    continue;
                } else {
                    if (x < selectLeft) {
                        selectLeft = x;
                    }

                    if (x + dxdc > selectRight) {
                        selectRight = x + dxdc;
                    }
                }
            }

            if (m_type == GraphCpu || call.pixels >= 0) {
                if (m_selection.type == SelectProgram && call.program != m_selection.program) {
                    painter.fillRect(QRectF(x, m_graphHeight - y, dxdc, y), deselectBrush);
                } else {
                    painter.fillRect(QRectF(x, m_graphHeight - y, dxdc, y), brush);
                }
            }
        }
    }

    /* Draw the selection borders */
    if (m_selection.type == SelectTime && selectLeft < selectRight) {
        selectLeft += m_axisWidth;
        selectRight += m_axisWidth;

        painter.resetTransform();
        painter.setPen(Qt::green);

        if (m_profile->calls[m_call].cpuStart <= m_selection.timeStart) {
            painter.drawLine(selectLeft, 0, selectLeft, height());
        }

        if (m_profile->calls[m_call + m_callWidth - 1].cpuStart >= m_selection.timeEnd) {
            painter.drawLine(selectRight, 0, selectRight, height());
        }

        selectLeft = qBound(m_axisWidth, selectLeft, width());
        selectRight = qBound(m_axisWidth, selectRight, width());
        painter.drawLine(selectLeft, m_axisHeight - 1, selectRight, m_axisHeight - 1);
    }
}

#include "graphwidget.moc"
