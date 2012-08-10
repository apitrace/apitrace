#include "timelinewidget.h"
#include "trace_profiler.hpp"

#include <math.h>
#include <QColor>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>

typedef trace::Profile::Call Call;
typedef trace::Profile::Frame Frame;

TimelineWidget::TimelineWidget(QWidget *parent)
    : QWidget(parent),
      m_profile(NULL),
      m_timeSelectionStart(0),
      m_timeSelectionEnd(0),
      m_rowHeight(20),
      m_axisWidth(50),
      m_axisHeight(20),
      m_axisBorder(Qt::black),
      m_axisBackground(Qt::lightGray),
      m_itemBorder(Qt::red),
      m_itemForeground(Qt::cyan),
      m_itemBackground(Qt::red),
      m_selectionBorder(QColor(50, 50, 255)),
      m_selectionBackground(QColor(245, 245, 255)),
      m_zoomBorder(Qt::green),
      m_zoomBackground(QColor(100, 255, 100, 80))
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
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
 * Update the time selection
 */
void TimelineWidget::setSelection(int64_t start, int64_t end, bool notify)
{
    m_timeSelectionStart = start;
    m_timeSelectionEnd = end;

    if (notify) {
        emit selectionChanged(m_timeSelectionStart, m_timeSelectionEnd);
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
 * Return the item at position
 */
const VisibleItem* TimelineWidget::itemAtPosition(const QPoint& pos)
{
    foreach (const VisibleItem& item, m_visibleItems) {
        if (pos.x() < item.rect.left() || pos.y() < item.rect.top())
            continue;

        if (pos.x() > item.rect.right() || pos.y() > item.rect.bottom())
            continue;

        return &item;
    }

    return NULL;
}


/**
 * Calculate the row order by total gpu time per shader
 */
void TimelineWidget::calculateRows()
{
    typedef QPair<uint64_t, unsigned> HeatProgram;
    QList<HeatProgram> heats;
    int idx;

    m_programRowMap.clear();
    m_rowCount = 0;

    for (Frame::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
        const Frame& frame = *itr;

        for (Call::const_iterator jtr = frame.calls.begin(); jtr != frame.calls.end(); ++jtr) {
            const Call& call = *jtr;

            while (call.program >= heats.size()) {
                heats.append(HeatProgram(0, heats.size()));
                m_programRowMap.append(m_programRowMap.size());
            }

            heats[call.program].first += call.gpuDuration;
        }
    }

    qSort(heats);
    idx = heats.size() - 1;

    for (QList<HeatProgram>::iterator itr = heats.begin(); itr != heats.end(); ++itr, --idx) {
        HeatProgram& pair = *itr;

        if (pair.first == 0) {
            m_programRowMap[pair.second] = -1;
        } else {
            m_programRowMap[pair.second] = idx;
            m_rowCount++;
        }
    }
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

    m_timeMin = m_profile->frames.front().gpuStart;
    m_timeMax = m_profile->frames.back().gpuStart + m_profile->frames.back().gpuDuration;

    m_time = m_timeMin;
    m_timeWidth = m_timeMax - m_timeMin;

    m_timeWidthMin = 1000;
    m_timeWidthMax = m_timeWidth;

    m_maxScrollX = 0;
    m_maxScrollY = qMax(0, (m_rowCount * m_rowHeight) - m_viewHeight);

    setTimeScroll(m_time);
    setRowScroll(0);

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
    m_viewWidth = width() - m_axisWidth;
    m_viewHeight = height() - m_axisHeight;

    /* Update vertical scroll bar */
    if (m_profile) {
        m_maxScrollY = qMax(0, (m_rowCount * m_rowHeight) - m_viewHeight);
        emit verticalScrollMaxChanged(m_maxScrollY);
        setRowScroll(m_scrollY);
    }
}


void TimelineWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    /* Display tooltip if necessary */
    if (e->buttons() == Qt::NoButton) {
        const VisibleItem* item = itemAtPosition(e->pos());

        if (item) {
            const trace::Profile::Call* call = item->call;

            QString text;
            text  = QString::fromStdString(call->name);
            text += QString("\nCall: %1").arg(call->no);
            text += QString("\nGPU Time: %1").arg(call->gpuDuration);
            text += QString("\nCPU Time: %1").arg(call->cpuDuration);
            text += QString("\nPixels Drawn: %1").arg(call->pixels);

            QToolTip::showText(e->globalPos(), text);
        }
    }

    m_mousePosition = e->pos();

    if (e->buttons().testFlag(Qt::LeftButton)) {
        QToolTip::hideText();

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
            int64_t down  = positionToTime(m_mousePressPosition.x() - m_axisWidth);
            int64_t up    = positionToTime(e->pos().x() - m_axisWidth);

            setSelection(qMin(down, up), qMax(down, up));
        }

        update();
    }
}


void TimelineWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        if (e->pos().y() < m_axisHeight) {
            if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                m_mousePressMode = RulerZoom;
            } else {
                m_mousePressMode = RulerSelect;

                int64_t time = positionToTime(e->pos().x() - m_axisWidth);
                m_timeSelectionStart = time;
                m_timeSelectionEnd = time;
            }
        } else {
            m_mousePressMode = DragView;
        }

        m_mousePressPosition  = e->pos();
        m_mousePressTime = m_time;
        m_mousePressRow   = m_scrollY;

        update();
    }
}


void TimelineWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_profile) {
        return;
    }

    /* Calculate new time view based on selected area */
    int64_t down  = positionToTime(m_mousePressPosition.x() - m_axisWidth);
    int64_t up    = positionToTime(e->pos().x() - m_axisWidth);

    int64_t left  = qMin(down, up);
    int64_t right = qMax(down, up);

    if (m_mousePressMode == RulerZoom) {
        m_timeWidth = right - left;
        m_timeWidth = qBound(m_timeWidthMin, m_timeWidth, m_timeWidthMax);

        m_mousePressMode = NoMousePress;
        setTimeScroll(left);
    } else if (m_mousePressMode == RulerSelect) {
        setSelection(m_timeSelectionStart, m_timeSelectionEnd, true);
    }
}


void TimelineWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    int64_t time = positionToTime(e->pos().x() - m_axisWidth);

    if (e->pos().y() < m_axisHeight) {
        int64_t time = positionToTime(e->pos().x() - m_axisWidth);

        for (Frame::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
            const Frame& frame = *itr;

            if (frame.gpuStart + frame.gpuDuration < time)
                continue;

            if (frame.gpuStart > time)
                break;

            setSelection(frame.gpuStart, frame.gpuStart + frame.gpuDuration, true);
            return;
        }
    }

    if (const VisibleItem* item = itemAtPosition(e->pos())) {
        emit jumpToCall(item->call->no);
    } else if (time < m_timeSelectionStart || time > m_timeSelectionEnd) {
        setSelection(0, 0, true);
    }
}


void TimelineWidget::wheelEvent(QWheelEvent *e)
{
    if (!m_profile) {
        return;
    }

    int zoomPercent = 10;

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
void TimelineWidget::paintHeatmapColumn(int x, QPainter& painter, QVector<uint64_t>& rows)
{
    double timePerPixel = m_timeWidth;
    timePerPixel /= m_viewWidth;

    for(int i = 0, y = 0; i < rows.size(); ++i, y += m_rowHeight) {
        if (rows[i] == 0)
            continue;

        if (y > m_viewHeight)
            continue;

        double heat = rows[i] / timePerPixel;
        heat = qBound(0.0, heat, 1.0);
        heat *= 255.0;

        painter.setPen(QColor(255, 255 - heat, 255 - heat));
        painter.drawLine(x, y, x, y + m_rowHeight);

        rows[i] = 0;
    }
}


/**
 * Render the whole widget
 */
void TimelineWidget::paintEvent(QPaintEvent *e)
{
    if (!m_profile)
        return;

    QVector<uint64_t> heatMap(m_programRowMap.size(), 0);
    QPainter painter(this);
    int64_t timeEnd;
    int selectionRight;
    int selectionLeft;
    int rowEnd;
    int lastX;


    /*
     * Draw the active selection background
     */
    if (m_timeSelectionStart != m_timeSelectionEnd) {
        selectionLeft  = timeToPosition(m_timeSelectionStart) + m_axisWidth;
        selectionRight = (timeToPosition(m_timeSelectionEnd) + 0.5) + m_axisWidth;

        selectionLeft  = qBound(-1, selectionLeft, width() + 1);
        selectionRight = qBound(-1, selectionRight, width() + 1);

        painter.setPen(Qt::NoPen);
        painter.setBrush(m_selectionBackground);
        painter.drawRect(selectionLeft, m_axisHeight, selectionRight - selectionLeft, m_viewHeight);
    }


    /*
     * Draw profile heatmap
     */
    rowEnd = m_row + (m_viewHeight / m_rowHeight) + 1;
    timeEnd = m_time + m_timeWidth;
    m_visibleItems.clear();
    lastX = 0;

    painter.translate(m_axisWidth + 1, m_axisHeight + 1 - (m_scrollY % m_rowHeight));
    painter.setBrush(m_itemBackground);
    painter.setPen(m_itemBorder);

    for (Frame::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
        const Frame& frame = *itr;

        if (frame.gpuStart > timeEnd)
            break;

        if (frame.gpuStart + frame.gpuDuration < m_time)
            continue;

        for (Call::const_iterator jtr = frame.calls.begin(); jtr != frame.calls.end(); ++jtr) {
            const Call& call = *jtr;
            int row = m_programRowMap[call.program];

            if (call.gpuStart + call.gpuDuration < m_time || call.gpuStart > timeEnd)
                continue;

            if (row < m_row || row > rowEnd)
                continue;

            double left = qMax(0.0, timeToPosition(call.gpuStart));
            double right = timeToPosition(call.gpuStart + call.gpuDuration);

            int leftX = left;
            int rightX = right;

            if (lastX != leftX) {
                paintHeatmapColumn(lastX, painter, heatMap);
                lastX = leftX;
            }

            row -= m_row;

            if (rightX <= lastX + 1) {
                if (lastX == rightX) {
                    /* Fully contained in this X */
                    heatMap[row] += call.gpuDuration;
                } else {
                    /* Split call time between the two pixels it occupies */
                    int64_t time = positionToTime(rightX);

                    heatMap[row] += time - call.gpuStart;
                    paintHeatmapColumn(lastX, painter, heatMap);

                    heatMap[row] += (call.gpuDuration + call.gpuStart) - time;
                    lastX = rightX;
                }
            } else {
                leftX = (left + 0.5);
                rightX = (right + 0.5);

                QRect rect;
                rect.setLeft(leftX);
                rect.setWidth(rightX - leftX);
                rect.setTop(row * m_rowHeight);
                rect.setHeight(m_rowHeight);

                VisibleItem vis;
                vis.rect = painter.transform().mapRect(rect);
                vis.frame = &frame;
                vis.call = &call;
                m_visibleItems.push_back(vis);

                painter.drawRect(rect);

                if (rect.width() > 6) {
                    rect.adjust(1, 0, -1, 0);
                    painter.setPen(m_itemForeground);
                    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(call.name));
                    painter.setPen(m_itemBorder);
                }
            }
        }
    }

    /* Paint the last column if needed */
    paintHeatmapColumn(lastX, painter, heatMap);


    /*
     * Draw the axis border and background
     */
    painter.resetTransform();
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_axisBackground);
    painter.drawRect(0, 0, m_viewWidth + m_axisWidth, m_axisHeight);
    painter.drawRect(0, m_axisHeight, m_axisWidth, m_viewHeight);

    painter.setPen(m_axisBorder);
    painter.drawLine(0, m_axisHeight, m_axisWidth + m_viewWidth, m_axisHeight);
    painter.drawLine(m_axisWidth, 0, m_axisWidth, m_viewHeight + m_axisHeight);


    /*
     * Draw horizontal axis
     */
    for (Frame::const_iterator itr = m_profile->frames.begin(); itr != m_profile->frames.end(); ++itr) {
        const Frame& frame = *itr;
        int left, right, width;
        bool drawText = true;
        QString text;

        if (frame.gpuStart > timeEnd)
            break;

        if (frame.gpuStart + frame.gpuDuration < m_time)
            continue;

        left = timeToPosition(frame.gpuStart);
        right = timeToPosition(frame.gpuStart + frame.gpuDuration) + 0.5;

        left = qBound(0, left, m_viewWidth) + m_axisWidth;
        right = qBound(0, right, m_viewWidth) + m_axisWidth;

        width = right - left;

        text = QString("Frame %1").arg(frame.no);

        if (painter.fontMetrics().width(text) > width) {
            text =  QString("%1").arg(frame.no);

            if (painter.fontMetrics().width(text) > width) {
                drawText = false;
            }
        }

        if (drawText) {
            painter.drawText(left, 0, width, m_axisHeight, Qt::AlignHCenter | Qt::AlignVCenter, text);
        }

        if (width > 5) {
            painter.drawLine(left, 0, left, m_axisHeight);
            painter.drawLine(right, 0, right, m_axisHeight);
        }
    }


    /*
     * Draw vertical axis
     */
    painter.translate(0, -(m_scrollY % m_rowHeight));

    for (int i = 0; i < m_programRowMap.size(); ++i) {
        int y = (m_programRowMap[i] - m_row) * m_rowHeight;

        if (m_programRowMap[i] < 0 || y < -m_rowHeight || y > m_viewHeight)
            continue;

        y += m_axisHeight + 1;
        painter.drawText(0, y, m_axisWidth, m_rowHeight, Qt::AlignHCenter | Qt::AlignVCenter, QString("%1").arg(i));
        painter.drawLine(0, y + m_rowHeight, m_axisWidth, y + m_rowHeight);
    }

    /* Draw the top left square again to cover up any hanging over text */
    painter.resetTransform();
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_axisBackground);
    painter.drawRect(0, 0, m_axisWidth, m_axisHeight);


    /*
     * Draw the active selection border
     */
    if (m_timeSelectionStart != m_timeSelectionEnd) {
        painter.setPen(m_selectionBorder);
        painter.drawLine(selectionLeft, 0, selectionLeft, m_viewHeight + m_axisHeight);
        painter.drawLine(selectionRight, 0, selectionRight, m_viewHeight + m_axisHeight);
        painter.drawLine(selectionLeft, m_axisHeight, selectionRight, m_axisHeight);
    }


    /*
     * Draw the ruler zoom
     */
    if (m_mousePressMode == RulerZoom) {
        int x1 = m_mousePressPosition.x();
        int x2 = m_mousePosition.x();
        int y1 = m_axisHeight;
        int y2 = height();

        painter.setPen(m_zoomBorder);
        painter.drawLine(x1, 0, x1, y2);
        painter.drawLine(x2, 0, x2, y2);
        painter.drawLine(x1, y1, x2, y1);

        painter.setPen(Qt::NoPen);
        painter.setBrush(m_zoomBackground);
        painter.drawRect(x1, y1, x2 - x1, y2);
    }
}

#include "timelinewidget.moc"
