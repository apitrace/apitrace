#include "timelinewidget.h"
#include "profiledialog.h"
#include "trace_profiler.hpp"

#include <qmath.h>
#include <QColor>
#include <QLocale>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>

typedef trace::Profile::Call Call;
typedef trace::Profile::Frame Frame;
typedef trace::Profile::Program Program;

TimelineWidget::TimelineWidget(QWidget *parent)
    : QWidget(parent),
      m_profile(NULL),
      m_rowHeight(20),
      m_axisWidth(50),
      m_axisHeight(30),
      m_axisLine(QColor(240, 240, 240)),
      m_axisBorder(Qt::black),
      m_axisForeground(Qt::black),
      m_axisBackground(QColor(210, 210, 210)),
      m_itemBorder(Qt::red),
      m_itemGpuForeground(Qt::cyan),
      m_itemGpuBackground(Qt::red),
      m_itemCpuForeground(QColor(255, 255, 0)),
      m_itemCpuBackground(QColor(0, 0, 255)),
      m_itemDeselectedForeground(Qt::white),
      m_itemDeselectedBackground(QColor(155, 155, 155)),
      m_selectionBorder(Qt::green),
      m_selectionBackground(QColor(100, 255, 100, 8)),
      m_zoomBorder(QColor(255, 0, 255)),
      m_zoomBackground(QColor(255, 0, 255, 30))
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

    m_selection.type = SelectNone;
}


/**
 * Update horizontal view scroll based on scroll value
 */
void TimelineWidget::setHorizontalScrollValue(int scrollValue)
{
    if (!m_profile) {
        return;
    }

    /* Calculate time from scroll value */
    double time = scrollValue;
    time /= m_maxScrollX;
    time *= (m_timeMax - m_timeWidth) - m_timeMin;
    time += m_timeMin;

    setTimeScroll(time, false);
}


/**
 * Update vertical view scroll based on scroll value
 */
void TimelineWidget::setVerticalScrollValue(int value)
{
    if (!m_profile) {
        return;
    }

    setRowScroll(value, false);
}


/**
 * Set selection to nothing
 */
void TimelineWidget::selectNone(bool notify)
{
    m_selection.type = SelectNone;

    if (notify) {
        emit selectedNone();
    }

    update();
}


/**
 * Set selection to a program
 */
void TimelineWidget::selectProgram(unsigned program, bool notify)
{
    m_selection.program = program;
    m_selection.type = SelectProgram;

    if (notify) {
        emit selectedProgram(program);
    }

    update();
}


/**
 * Set selection to a period of time
 */
void TimelineWidget::selectTime(int64_t start, int64_t end, bool notify)
{
    m_selection.timeStart = start;
    m_selection.timeEnd = end;
    m_selection.type = SelectTime;

    if (notify) {
        emit selectedTime(start, end);
    }

    update();
}


/**
 * Convert time to view position
 */
double TimelineWidget::timeToPosition(int64_t time)
{
    double pos = time;
    pos -= m_time;
    pos /= m_timeWidth;
    pos *= m_viewWidth;
    return pos;
}


/**
 * Convert view position to time
 */
int64_t TimelineWidget::positionToTime(int pos)
{
    double time = pos;
    time /= m_viewWidth;
    time *= m_timeWidth;
    time += m_time;
    return (int64_t)time;
}


/**
 * Binary Search for a time in start+durations
 */
template<typename val_ty, int64_t val_ty::* mem_ptr_start, int64_t val_ty::* mem_ptr_dura>
typename std::vector<val_ty>::const_iterator binarySearchTimespan(
        typename std::vector<val_ty>::const_iterator begin,
        typename std::vector<val_ty>::const_iterator end,
        int64_t time)
{
    int lower = 0;
    int upper = end - begin;
    int pos = (lower + upper) / 2;
    typename std::vector<val_ty>::const_iterator itr = begin + pos;

    while (!((*itr).*mem_ptr_start <= time && (*itr).*mem_ptr_start + (*itr).*mem_ptr_dura > time) && (lower <= upper)) {
        if ((*itr).*mem_ptr_start > time) {
            upper = pos - 1;
        } else {
            lower = pos + 1;
        }

        pos = (lower + upper) / 2;
        itr = begin + pos;
    }

    if (lower <= upper) {
        return itr;
    } else {
        return end;
    }
}


/**
 * Binary Search for a time in start+durations on an array of indices
 */
std::vector<unsigned>::const_iterator binarySearchTimespanIndexed(
        const std::vector<Call>& calls,
        std::vector<unsigned>::const_iterator begin,
        std::vector<unsigned>::const_iterator end,
        int64_t time)
{
    int lower = 0;
    int upper = end - begin - 1;
    int pos = (lower + upper) / 2;
    std::vector<unsigned>::const_iterator itr = begin + pos;

    while (lower <= upper) {
        const Call& call = calls[*itr];

        if (call.gpuStart <= time && call.gpuStart + call.gpuDuration > time) {
            break;
        }

        if (call.gpuStart > time) {
            upper = pos - 1;
        } else {
            lower = pos + 1;
        }

        pos = (lower + upper) / 2;
        itr = begin + pos;
    }

    if (lower <= upper) {
        return itr;
    } else {
        return end;
    }
}


/**
 * Find the frame at time
 */
const Frame* TimelineWidget::frameAtTime(int64_t time)
{
    if (!m_profile) {
        return NULL;
    }

    std::vector<Frame>::const_iterator res
            = binarySearchTimespan<Frame, &Frame::cpuStart, &Frame::cpuDuration>(
                m_profile->frames.begin(),
                m_profile->frames.end(),
                time);

    if (res != m_profile->frames.end()) {
        return &*res;
    }

    return NULL;
}


/**
 * Find the CPU call at time
 */
const Call* TimelineWidget::cpuCallAtTime(int64_t time)
{
    if (!m_profile) {
        return NULL;
    }

    std::vector<Call>::const_iterator res
            = binarySearchTimespan<Call, &Call::cpuStart, &Call::cpuDuration>(
                m_profile->calls.begin(),
                m_profile->calls.end(),
                time);

    if (res != m_profile->calls.end()) {
        return &*res;
    }

    return NULL;
}


/**
 * Find the draw call at time
 */
const Call* TimelineWidget::drawCallAtTime(int64_t time)
{
    if (!m_profile) {
        return NULL;
    }

    for (int i = 0; i < m_rowPrograms.size(); ++i) {
        const Call* call = drawCallAtTime(time, m_rowPrograms[i]);

        if (call) {
            return call;
        }
    }

    return NULL;
}


/**
 * Find the draw call at time for a selected program
 */
const Call* TimelineWidget::drawCallAtTime(int64_t time, int program)
{
    if (!m_profile) {
        return NULL;
    }

    std::vector<unsigned>::const_iterator res
            = binarySearchTimespanIndexed(
                m_profile->calls,
                m_profile->programs[program].calls.begin(),
                m_profile->programs[program].calls.end(),
                time);

    if (res != m_profile->programs[program].calls.end()) {
        return &m_profile->calls[*res];
    }

    return NULL;
}


/**
 * Calculate the row order by total gpu time per shader
 */
void TimelineWidget::calculateRows()
{
    typedef QPair<uint64_t, unsigned> Pair;
    std::vector<Pair> gpu;

    /* Map shader to visible row */
    for (std::vector<Program>::const_iterator itr = m_profile->programs.begin(); itr != m_profile->programs.end(); ++itr) {
        const Program& program = *itr;
        unsigned no = itr -  m_profile->programs.begin();

        if (program.gpuTotal > 0) {
            gpu.push_back(Pair(program.gpuTotal, no));
        }
    }

    /* Sort the shaders by most used gpu */
    qSort(gpu);

    /* Create row order */
    m_rowPrograms.clear();

    for (std::vector<Pair>::const_reverse_iterator itr = gpu.rbegin(); itr != gpu.rend(); ++itr) {
        m_rowPrograms.push_back(itr->second);
    }

    m_rowCount = m_rowPrograms.size();
}


/**
 * Set the trace profile to use for the timeline
 */
void TimelineWidget::setProfile(trace::Profile* profile)
{
    if (!profile->frames.size())
        return;

    m_profile = profile;
    calculateRows();

    m_timeMin = m_profile->frames.front().cpuStart;
    m_timeMax = m_profile->frames.back().cpuStart + m_profile->frames.back().cpuDuration;

    m_time = m_timeMin;
    m_timeWidth = m_timeMax - m_timeMin;

    m_timeWidthMin = 1000;
    m_timeWidthMax = m_timeWidth;

    m_maxScrollX = 0;
    m_maxScrollY = qMax(0, (m_rowCount * m_rowHeight) - m_viewHeight);

    setTimeScroll(m_time);
    setRowScroll(0);
    selectNone();
    update();
}


/**
 * Set the horizontal scroll position to time
 */
void TimelineWidget::setTimeScroll(int64_t time, bool notify)
{
    time = qBound(m_timeMin, time, m_timeMax - m_timeWidth);

    m_time = time;

    if (m_timeWidth == m_timeWidthMax) {
        m_maxScrollX = 0;
    } else {
        m_maxScrollX = 10000;
    }

    if (notify) {
        double value = time - m_timeMin;
        value /= m_timeMax - m_timeWidth - m_timeMin;
        value *= m_maxScrollX;
        m_scrollX = value;

        emit horizontalScrollMaxChanged(m_maxScrollX);
        emit horizontalScrollValueChanged(m_scrollX);
    }

    update();
}


/**
 * Set the vertical scroll position to position
 */
void TimelineWidget::setRowScroll(int position, bool notify)
{
    position = qBound(0, position, m_maxScrollY);

    m_scrollY = position;
    m_row  = m_scrollY / m_rowHeight;

    if (notify) {
        emit verticalScrollMaxChanged(m_maxScrollY);
        emit verticalScrollValueChanged(m_scrollY);
    }

    update();
}


void TimelineWidget::resizeEvent(QResizeEvent *e)
{
    /* Update viewport size */
    m_viewWidth = qMax(0, width() - m_axisWidth);
    m_viewHeight = qMax(0, height() - m_axisHeight - m_rowHeight * 2);

    /* Update vertical scroll bar */
    if (m_profile) {
        m_maxScrollY = qMax(0, (m_rowCount * m_rowHeight) - m_viewHeight);
        emit verticalScrollMaxChanged(m_maxScrollY);
        setRowScroll(m_scrollY);
    }
}


void TimelineWidget::mouseMoveEvent(QMouseEvent *e)
{
    bool tooltip = false;
    m_mousePosition = e->pos();

    if (!m_profile) {
        return;
    }

    /* Display tooltip if necessary */
    if (e->buttons() == Qt::NoButton) {
        if (m_mousePosition.x() > m_axisWidth && m_mousePosition.y() > m_axisHeight) {
            int64_t time = positionToTime(m_mousePosition.x() - m_axisWidth);
            int y = m_mousePosition.y() - m_axisHeight;

            if (y < m_rowHeight) {
                const Call* call = cpuCallAtTime(time);

                if (call) {
                    QString text;
                    text  = QString::fromStdString(call->name);
                    text += QString("\nCall: %1").arg(call->no);
                    text += QString("\nCPU Start: %1").arg(getTimeString(call->cpuStart));
                    text += QString("\nCPU Duration: %1").arg(getTimeString(call->cpuDuration));

                    QToolTip::showText(e->globalPos(), text);
                    tooltip = true;
                }
            } else {
                const Call* call = NULL;

                if (y < m_rowHeight * 2) {
                    call = drawCallAtTime(time);
                } else {
                    int row = (y - m_rowHeight * 2 + m_scrollY) / m_rowHeight;

                    if (row < m_rowPrograms.size()) {
                        call = drawCallAtTime(time, m_rowPrograms[row]);
                    }
                }

                if (call) {
                    QString text;
                    text  = QString::fromStdString(call->name);
                    text += QString("\nCall: %1").arg(call->no);
                    text += QString("\nCPU Start: %1").arg(getTimeString(call->cpuStart));
                    text += QString("\nGPU Start: %1").arg(getTimeString(call->gpuStart));
                    text += QString("\nCPU Duration: %1").arg(getTimeString(call->cpuDuration));
                    text += QString("\nGPU Duration: %1").arg(getTimeString(call->gpuDuration));
                    text += QString("\nPixels Drawn: %1").arg(QLocale::system().toString((qlonglong)call->pixels));

                    QToolTip::showText(e->globalPos(), text);
                    tooltip = true;
                }
            }
        } else if (m_mousePosition.x() < m_axisWidth && m_mousePosition.y() > m_axisHeight) {
            int y = m_mousePosition.y() - m_axisHeight;

            if (y < m_rowHeight) {
                QToolTip::showText(e->globalPos(), "All CPU calls");
                tooltip = true;
            } else if (y < m_rowHeight * 2) {
                QToolTip::showText(e->globalPos(), "All GPU calls");
                tooltip = true;
            } else {
                int row = (y - m_rowHeight * 2 + m_scrollY) / m_rowHeight;

                if (row < m_rowPrograms.size()) {
                    QToolTip::showText(e->globalPos(), QString("All calls in Shader Program %1").arg(m_rowPrograms[row]));
                    tooltip = true;
                }
            }
        }
    } else if (e->buttons().testFlag(Qt::LeftButton)) {
        if (m_mousePressMode == DragView) {
            /* Horizontal scroll */
            double dt = m_timeWidth;
            dt /= m_viewWidth;
            dt *= m_mousePressPosition.x() - e->pos().x();
            setTimeScroll(m_mousePressTime + dt);

            /* Vertical scroll */
            int dy = m_mousePressPosition.y() - e->pos().y();
            setRowScroll(m_mousePressRow + dy);
        } else if (m_mousePressMode == RulerSelect) {
            /* Horizontal selection */
            int64_t down  = positionToTime(m_mousePressPosition.x() - m_axisWidth);
            int64_t up    = positionToTime(qMax(e->pos().x() - m_axisWidth, 0));

            selectTime(qMin(down, up), qMax(down, up), true);
        }

        update();
    }

    if (!tooltip) {
        QToolTip::hideText();
    }
}


void TimelineWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        if (e->pos().y() < m_axisHeight && e->pos().x() >= m_axisWidth) {
            if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                m_mousePressMode = RulerZoom;
            } else {
                m_mousePressMode = RulerSelect;
            }
        } else if (e->pos().x() >= m_axisWidth) {
            m_mousePressMode = DragView;
        } else {
            m_mousePressMode = NoMousePress;
        }

        m_mousePressPosition  = e->pos();
        m_mousePressTime = m_time;
        m_mousePressRow  = m_scrollY;

        update();
    }
}


void TimelineWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    /* Calculate new time view based on selected area */
    int dxy = qAbs(m_mousePressPosition.x() - e->pos().x()) + qAbs(m_mousePressPosition.y() - e->pos().y());

    int64_t down  = positionToTime(m_mousePressPosition.x() - m_axisWidth);
    int64_t up    = positionToTime(qMax(e->pos().x() - m_axisWidth, 0));

    int64_t left  = qMin(down, up);
    int64_t right = qMax(down, up);

    if (m_mousePressMode == RulerZoom) {
        m_timeWidth = right - left;
        m_timeWidth = qBound(m_timeWidthMin, m_timeWidth, m_timeWidthMax);

        m_mousePressMode = NoMousePress;
        setTimeScroll(left);
    } else {
        if (dxy <= 2) {
            if (m_selection.type == SelectTime) {
                if (left < m_selection.timeStart || right > m_selection.timeEnd || e->pos().x() < m_axisWidth) {
                    selectNone(true);
                }
            } else if (m_selection.type == SelectProgram) {
                int y = e->pos().y() - m_axisHeight;
                int row = (y - m_rowHeight * 2 + m_scrollY) / m_rowHeight;

                if (row < 0 || m_rowPrograms[row] != m_selection.program) {
                    selectNone(true);
                }
            }
        } else if (m_mousePressMode == RulerSelect) {
            selectTime(left, right, true);
        }
    }
}


void TimelineWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    int64_t time = positionToTime(e->pos().x() - m_axisWidth);

    if (e->pos().x() > m_axisWidth) {
        int row = (e->pos().y() - m_axisHeight) / m_rowHeight;

        if (e->pos().y() < m_axisHeight) {
            /* Horizontal axis */
            const Frame* frame = frameAtTime(time);

            if (frame) {
                selectTime(frame->cpuStart, frame->cpuStart + frame->cpuDuration, true);
                return;
            }
        } else if (row == 0) {
            /* CPU Calls */
            const Call* call = cpuCallAtTime(time);

            if (call) {
                emit jumpToCall(call->no);
                return;
            }
        } else if (row > 0) {
            /* Draw Calls */
            const Call* call = drawCallAtTime(time, 0);

            if (call) {
                emit jumpToCall(call->no);
                return;
            }
        }
    } else {
        int y = e->pos().y() - m_axisHeight;
        int row = (y - m_rowHeight * 2 + m_scrollY) / m_rowHeight;

        if (row >= 0 && row < m_rowPrograms.size()) {
            selectProgram(m_rowPrograms[row], true);
        }
    }
}


void TimelineWidget::wheelEvent(QWheelEvent *e)
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
    double dt = m_timeWidth;
    double size = m_timeWidth;
    size *= -e->delta();

    /* Zoom deltas normally come in increments of 120 */
    size /= 120 * (100 / zoomPercent);

    m_timeWidth += size;
    m_timeWidth  = qBound(m_timeWidthMin, m_timeWidth, m_timeWidthMax);

    /* Scroll view to zoom around mouse */
    dt -= m_timeWidth;
    dt *= e->x() - m_axisWidth;
    dt /= m_viewWidth;
    setTimeScroll(dt + m_time);

    update();
}


/**
 * Paints a single pixel column of the heat map
 */
void TimelineWidget::drawHeat(QPainter& painter, int x, int64_t heat, bool gpu, bool selected)
{
    if (heat == 0) {
        return;
    }

    if (m_selection.type == SelectTime) {
        selected = x >= m_selectionLeft && x <= m_selectionRight;
    }

    double timePerPixel = m_timeWidth / (double)m_viewWidth;
    double colour = heat / timePerPixel;

    /* Gamma correction */
    colour = qPow(colour, 1.0 / 2.0);

    if (!selected) {
        colour = qBound(0.0, colour * 100.0, 100.0);
        painter.setPen(QColor(255 - colour, 255 - colour, 255 - colour));
    } else if (gpu) {
        colour = qBound(0.0, colour * 255.0, 255.0);
        painter.setPen(QColor(255, 255 - colour, 255 - colour));
    } else {
        colour = qBound(0.0, colour * 255.0, 255.0);
        painter.setPen(QColor(255 - colour, 255 - colour, 255));
    }

    painter.drawLine(x, 0, x, m_rowHeight - 1);
}


/**
 * Draws a call on the heatmap
 */
bool TimelineWidget::drawCall(QPainter& painter, const trace::Profile::Call& call, int& lastX, int64_t& heat, bool gpu)
{
    int64_t start, duration, end;

    if (gpu) {
        start = call.gpuStart;
        duration = call.gpuDuration;
    } else {
        start = call.cpuStart;
        duration = call.cpuDuration;
    }

    end = start + duration;

    if (start > m_timeEnd) {
        return false;
    }

    if (end < m_time) {
        return true;
    }

    double left  = timeToPosition(start);
    double right = timeToPosition(end);

    int leftX  = left;
    int rightX = right;

    bool selected = true;

    if (m_selection.type == SelectProgram) {
        selected = call.program == m_selection.program;
    }

    /* Draw last heat if needed */
    if (leftX != lastX) {
        drawHeat(painter, lastX, heat, gpu, selected);
        lastX = leftX;
        heat = 0;
    }

    if (rightX <= leftX + 1) {
        if (rightX == lastX) {
            /* Fully contained in this X */
            heat += duration;
        } else {
            /* Split call time between the two pixels it occupies */
            int64_t time = positionToTime(rightX);
            heat += time - start;

            drawHeat(painter, lastX, heat, gpu, selected);

            heat = end - time;
            lastX = rightX;
        }
    } else {
        QRect rect;
        rect.setLeft(left + 0.5);
        rect.setWidth(right - left);
        rect.setTop(0);
        rect.setHeight(m_rowHeight);

        if (m_selection.type == SelectTime) {
            selected = (start >= m_selection.timeStart && start <= m_selection.timeEnd)
                    || (end >= m_selection.timeStart && end <= m_selection.timeEnd);
        }

        /* Draw background rect */
        if (selected) {
            if (gpu) {
                painter.fillRect(rect, m_itemGpuBackground);
            } else {
                painter.fillRect(rect, m_itemCpuBackground);
            }
        } else {
            painter.fillRect(rect, m_itemDeselectedBackground);
        }

        /* If wide enough, draw text */
        if (rect.width() > 6) {
            rect.adjust(1, 0, -1, -2);

            if (selected) {
                if (gpu) {
                    painter.setPen(m_itemGpuForeground);
                } else {
                    painter.setPen(m_itemCpuForeground);
                }
            } else {
                painter.setPen(m_itemDeselectedForeground);
            }

            painter.drawText(rect,
                             Qt::AlignLeft | Qt::AlignVCenter,
                             painter.fontMetrics().elidedText(QString::fromStdString(call.name), Qt::ElideRight, rect.width()));
        }
    }

    return true;
}


/**
 * Render the whole widget
 */
void TimelineWidget::paintEvent(QPaintEvent *e)
{
    if (!m_profile)
        return;

    QPainter painter(this);

    int rowEnd = qMin(m_row + qCeil(m_viewHeight / (double)m_rowHeight) + 1, m_rowCount);
    int64_t heatGPU = 0, heatCPU = 0;
    int lastCpuX = 0, lastGpuX = 0;
    int widgetHeight = height();
    int widgetWidth = width();

    m_timeEnd = m_time + m_timeWidth;
    m_selectionLeft  = timeToPosition(m_selection.timeStart);
    m_selectionRight = (timeToPosition(m_selection.timeEnd) + 0.5);


    /* Draw program rows */
    painter.translate(m_axisWidth, m_axisHeight + m_rowHeight * 2 - (m_scrollY % m_rowHeight));

    for (int row = m_row; row < rowEnd; ++row) {
        Program& program = m_profile->programs[m_rowPrograms[row]];
        lastGpuX = 0;
        heatGPU = 0;

        for (std::vector<unsigned>::const_iterator itr = program.calls.begin(); itr != program.calls.end(); ++itr) {
            const Call& call = m_profile->calls[*itr];

            if (!drawCall(painter, call, lastGpuX, heatGPU, true)) {
                break;
            }
        }

        painter.translate(0, m_rowHeight);
    }


    /* Draw CPU/GPU rows */
    painter.resetTransform();
    painter.translate(m_axisWidth, m_axisHeight);
    painter.fillRect(0, 0, m_viewWidth, m_rowHeight * 2, Qt::white);

    lastCpuX = lastGpuX = 0;
    heatCPU = heatGPU = 0;

    for (std::vector<Call>::const_iterator itr = m_profile->calls.begin(); itr != m_profile->calls.end(); ++itr) {
        const Call& call = *itr;

        /* Draw gpu row */
        if (call.pixels >= 0) {
            painter.translate(0, m_rowHeight);
            drawCall(painter, call, lastGpuX, heatGPU, true);
            painter.translate(0, -m_rowHeight);
        }

        /* Draw cpu row */
        if (!drawCall(painter, call, lastCpuX, heatCPU, false)) {
            break;
        }
    }


    /* Draw axis */
    painter.resetTransform();
    painter.setPen(m_axisBorder);

    /* Top Rect */
    painter.fillRect(m_axisWidth - 1, 0, widgetWidth, m_axisHeight - 1, m_axisBackground);
    painter.drawLine(0, m_axisHeight - 1, widgetWidth, m_axisHeight - 1);

    /* Left Rect */
    painter.fillRect(0, m_axisHeight - 1, m_axisWidth - 1, widgetHeight, m_axisBackground);
    painter.drawLine(m_axisWidth - 1, 0, m_axisWidth - 1, widgetHeight);


    /* Draw the program numbers */
    painter.translate(0, m_axisHeight + m_rowHeight * 2);

    for (int row = m_row; row < rowEnd; ++row) {
        int y = (row - m_row) * m_rowHeight - (m_scrollY % m_rowHeight);

        painter.setPen(m_axisForeground);
        painter.drawText(0, y, m_axisWidth, m_rowHeight, Qt::AlignHCenter | Qt::AlignVCenter, QString("%1").arg(m_rowPrograms[row]));

        if (m_selection.type == SelectProgram && m_selection.program == m_rowPrograms[row]) {
            painter.setPen(m_selectionBorder);
            painter.drawLine(0, qMax(0, y - 1), widgetWidth, qMax(0, y - 1));
            painter.drawLine(0, y + m_rowHeight - 1, widgetWidth, y + m_rowHeight - 1);
            painter.drawLine(m_axisWidth - 1, y - 1, m_axisWidth - 1, y + m_rowHeight - 1);
        } else {
            painter.setPen(m_axisBorder);
            painter.drawLine(0, y + m_rowHeight - 1, m_axisWidth - 1, y + m_rowHeight - 1);

            painter.setPen(m_axisLine);
            painter.drawLine(m_axisWidth, y + m_rowHeight - 1, widgetWidth, y + m_rowHeight - 1);
        }
    }


    /* Draw the "CPU" axis label */
    painter.resetTransform();
    painter.translate(0, m_axisHeight);

    painter.setPen(m_axisBorder);
    painter.setBrush(m_axisBackground);
    painter.drawRect(-1, -1, m_axisWidth, m_rowHeight);

    painter.setPen(m_axisForeground);
    painter.drawText(0, 0, m_axisWidth - 1, m_rowHeight - 1, Qt::AlignHCenter | Qt::AlignVCenter, "CPU");

    painter.setPen(m_axisBorder);
    painter.drawLine(m_axisWidth, m_rowHeight - 1, widgetWidth, m_rowHeight - 1);


    /* Draw the "GPU" axis label */
    painter.translate(0, m_rowHeight);

    painter.setPen(m_axisBorder);
    painter.setBrush(m_axisBackground);
    painter.drawRect(-1, -1, m_axisWidth, m_rowHeight);

    painter.setPen(m_axisForeground);
    painter.drawText(0, 0, m_axisWidth - 1, m_rowHeight - 1, Qt::AlignHCenter | Qt::AlignVCenter, "GPU");

    painter.setPen(m_axisBorder);
    painter.drawLine(m_axisWidth, m_rowHeight - 1, widgetWidth, m_rowHeight - 1);


    /* Draw the frame numbers */
    painter.resetTransform();

    painter.setPen(m_axisForeground);
    painter.translate(m_axisWidth, 0);

    int lastLabel = -999; /* Ensure first label gets drawn */

    double scroll = m_time;
    scroll /= m_timeWidth;
    scroll *= m_viewWidth;

    for (std::vector<Frame>::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
        static const int padding = 4;
        const Frame& frame = *itr;
        bool draw = true;
        int width;

        if (frame.cpuStart > m_timeEnd) {
            break;
        }

        if (frame.cpuStart + frame.cpuDuration < m_time) {
            draw = false;
        }

        double left = frame.cpuStart;
        left /= m_timeWidth;
        left *= m_viewWidth;

        double right = frame.cpuStart + frame.cpuDuration;
        right /= m_timeWidth;
        right *= m_viewWidth;

        QString text = QString("%1").arg(frame.no);

        width = painter.fontMetrics().width(text) + padding * 2;

        if (left + width > scroll)
            draw = true;

        /* Draw a frame number if we have space since the last one */
        if (left - lastLabel > width) {
            lastLabel = left + width;

            if (draw) {
                int textX;
                painter.setPen(m_axisForeground);

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


    /* Draw "Frame" axis label */
    painter.resetTransform();

    painter.setPen(m_axisBorder);
    painter.setBrush(m_axisBackground);
    painter.drawRect(-1, -1, m_axisWidth, m_axisHeight);

    painter.setPen(m_axisForeground);
    painter.drawText(0, 0, m_axisWidth - 1, m_axisHeight - 1, Qt::AlignHCenter | Qt::AlignVCenter, "Frame");


    /* Draw the active selection border */
    if (m_selection.type == SelectTime) {
        painter.setPen(m_selectionBorder);

        m_selectionLeft += m_axisWidth;
        m_selectionRight += m_axisWidth;

        if (m_selectionLeft >= m_axisWidth && m_selectionLeft < widgetWidth) {
            painter.drawLine(m_selectionLeft, 0, m_selectionLeft, widgetHeight);
        }

        if (m_selectionRight >= m_axisWidth && m_selectionRight < widgetWidth) {
            painter.drawLine(m_selectionRight, 0, m_selectionRight, widgetHeight);
        }

        m_selectionLeft = qBound(m_axisWidth, m_selectionLeft, widgetWidth);
        m_selectionRight = qBound(m_axisWidth, m_selectionRight, widgetWidth);

        painter.drawLine(m_selectionLeft, m_axisHeight - 1, m_selectionRight, m_axisHeight - 1);
        painter.fillRect(m_selectionLeft, 0, m_selectionRight - m_selectionLeft, widgetHeight, m_selectionBackground);
    }


    /* Draw the ruler zoom */
    if (m_mousePressMode == RulerZoom) {
        int x1 = m_mousePressPosition.x();
        int x2 = qMax(m_mousePosition.x(), m_axisWidth);

        painter.setPen(m_zoomBorder);
        painter.drawLine(x1, 0, x1, widgetHeight);
        painter.drawLine(x2, 0, x2, widgetHeight);
        painter.drawLine(x1, m_axisHeight - 1, x2, m_axisHeight - 1);
        painter.fillRect(x1, m_axisHeight, x2 - x1, widgetHeight, m_zoomBackground);
    }
}

#include "timelinewidget.moc"
