#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QList>
#include <QPen>
#include "trace_profiler.hpp"

struct VisibleItem {
    QRect rect;
    const trace::Profile::Frame* frame;
    const trace::Profile::Call* call;
};

class TimelineWidget : public QWidget
{
    Q_OBJECT

    enum MousePressMode {
        NoMousePress,
        DragView,
        RulerZoom,
        RulerSelect
    };

public:
    TimelineWidget(QWidget *parent = 0);

    void setProfile(trace::Profile* profile);

protected:
    virtual void wheelEvent(QWheelEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

public slots:
    void setHorizontalScrollValue(int value);
    void setVerticalScrollValue(int value);

signals:
    void verticalScrollMaxChanged(int max);
    void verticalScrollValueChanged(int value);

    void horizontalScrollMaxChanged(int max);
    void horizontalScrollValueChanged(int value);

    void jumpToCall(int call);

    void selectionChanged(int64_t start, int64_t end);

private:
    void setSelection(int64_t start, int64_t end, bool notify = false);
    void setRowScroll(int position, bool notify = true);
    void setTimeScroll(int64_t time, bool notify = true);

    void paintHeatmapColumn(int x, QPainter& painter, QVector<uint64_t>& rows);

    double timeToPosition(int64_t time);
    int64_t positionToTime(int pos);

    const VisibleItem* itemAtPosition(const QPoint& pos);

    void calculateRows();

private:
    /* Data */
    trace::Profile* m_profile;
    QList<int> m_programRowMap;
    QList<VisibleItem> m_visibleItems;

    /* Scrollbars */
    int m_scrollX;
    int m_scrollY;
    int m_maxScrollX;
    int m_maxScrollY;

    /* Viewport */
    int m_viewWidth;
    int m_viewHeight;

    /* Visible Times */
    int64_t m_time;
    int64_t m_timeMin;
    int64_t m_timeMax;
    int64_t m_timeWidth;
    int64_t m_timeWidthMin;
    int64_t m_timeWidthMax;
    int64_t m_timeSelectionStart;
    int64_t m_timeSelectionEnd;

    /* Visible Rows */
    int m_row;
    int m_rowCount;

    /* Mouse data */
    int m_mousePressRow;
    int64_t m_mousePressTime;
    QPoint m_mousePosition;
    QPoint m_mousePressPosition;
    MousePressMode m_mousePressMode;

    /* Style */
    int m_rowHeight;
    int m_axisWidth;
    int m_axisHeight;
    QPen m_axisBorder;
    QBrush m_axisBackground;
    QPen m_itemBorder;
    QPen m_itemForeground;
    QBrush m_itemBackground;
    QPen m_selectionBorder;
    QBrush m_selectionBackground;
    QPen m_zoomBorder;
    QBrush m_zoomBackground;
};

#endif // TIMELINEWIDGET_H
