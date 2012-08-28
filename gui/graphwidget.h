#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QPen>
#include <QWidget>
#include <QPainter>
#include <QLinearGradient>
#include "trace_profiler.hpp"

enum GraphType {
    GraphGpu,
    GraphCpu
};

enum SelectType {
    SelectNone,
    SelectTime,
    SelectProgram
};

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);

    void setProfile(trace::Profile* profile, GraphType type);
    const trace::Profile::Call* callAtPosition(const QPoint& pos);

    void selectNone(bool notify = false);
    void selectTime(int64_t start, int64_t end, bool notify = false);
    void selectProgram(unsigned program, bool notify = false);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    virtual void wheelEvent(QWheelEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

signals:
    void jumpToCall(int no);
    void viewChanged(int call, int width);

    void selectedNone();
    void selectedProgram(unsigned program);
    void selectedTime(int64_t start, int64_t end);

public slots:
    void changeView(int call, int width);

private:
    void update();

    void paintVerticalAxis(QPainter& painter);
    void paintHorizontalAxis(QPainter& painter);

private:
    /* Data */
    trace::Profile* m_profile;
    GraphType m_type;

    /* Vertical Axis */
    int64_t m_timeMax;

    /* Horizontal axis */
    int m_call;
    int m_callMin;
    int m_callMax;
    int m_callWidth;
    int m_callWidthMin;
    int m_callWidthMax;

    /* Viewport */
    int m_graphWidth;
    int m_graphHeight;

    /* Mouse track data */
    int m_mousePressCall;
    QPoint m_mousePressPosition;

    /* Style */
    int m_axisWidth;
    int m_axisHeight;
    QPen m_axisForeground;
    QBrush m_axisBackground;
    QLinearGradient m_graphGradientGpu;
    QLinearGradient m_graphGradientCpu;
    QLinearGradient m_graphGradientDeselected;

    struct {
        SelectType type;

        unsigned program;

        int64_t timeStart;
        int64_t timeEnd;
    } m_selection;
};

#endif // GRAPHWIDGET_H
