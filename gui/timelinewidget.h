#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QList>
#include <QPen>
#include "trace_profiler.hpp"

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

    void drawHeat(QPainter& painter, int x, int64_t heat, bool isCpu);

    double timeToPosition(int64_t time);
    int64_t positionToTime(int pos);

    void calculateRows();

    const trace::Profile::Frame* frameAtTime(int64_t time);
    const trace::Profile::Call* cpuCallAtTime(int64_t time);
    const trace::Profile::Call* drawCallAtTime(int64_t time, int program);

private:
    /* Data */
    trace::Profile* m_profile;
    std::vector<int> m_rowPrograms;

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
    QPen m_axisLine;
    QPen m_axisBorder;
    QPen m_axisForeground;
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
