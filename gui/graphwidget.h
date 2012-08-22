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

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);

    void setProfile(trace::Profile* profile, GraphType type);
    const trace::Profile::Call* callAtPosition(const QPoint& pos);

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

signals:
    void jumpToCall(int no);
    void viewChanged(int call, int width);

public slots:
    void changeView(int call, int width);

private:
    void update();

    void paintVerticalAxis(QPainter& painter);
    void paintHorizontalAxis(QPainter& painter);

private:
    trace::Profile* m_profile;
    GraphType m_type;

    int64_t m_maxTime;

    int m_minCall;
    int m_maxCall;

    int m_minCallWidth;
    int m_maxCallWidth;

    int m_call;
    int m_callWidth;

    QPoint m_mousePressPosition;
    int m_mousePressCall;

    int m_axisWidth;
    int m_axisHeight;

    int m_graphWidth;
    int m_graphHeight;

    QPen m_axisForeground;
    QBrush m_axisBackground;

    QLinearGradient m_graphGradientGpu;
    QLinearGradient m_graphGradientCpu;
};

#endif // GRAPHWIDGET_H
