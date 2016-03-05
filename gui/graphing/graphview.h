#pragma once

#include "graphing.h"

#include <QWidget>

/**
 * The generic base class for a graph's view, this is the component that
 * displays the actual data for the graph.
 *
 * - Stores the view area within the graph
 * - Simple user interaction such as translating and zooming with mouse
 * - Selection tracking synchronised with axis
 */
class GraphView : public QWidget {
    Q_OBJECT
public:
    GraphView(QWidget* parent = 0);
    virtual ~GraphView(){}

    virtual void update();

    virtual void resizeEvent(QResizeEvent *) override;

    virtual void wheelEvent(QWheelEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

    virtual void setSelectionState(SelectionState* state);

    void setHorizontalView(qint64 start, qint64 end);
    void setVerticalView(qint64 start, qint64 end);

protected:
    void setDefaultView(qint64 min, qint64 max);

signals:
    void selectionChanged();

    void verticalViewChanged(qint64 start, qint64 end);
    void verticalRangeChanged(qint64 min, qint64 max);

    void horizontalRangeChanged(qint64 min, qint64 max);
    void horizontalViewChanged(qint64 start, qint64 end);

protected:
    /* Viewport area */
    qint64 m_viewLeft;
    qint64 m_viewRight;
    qint64 m_viewBottom;
    qint64 m_viewTop;

    /* Graph limits */
    qint64 m_graphLeft;
    qint64 m_graphRight;
    qint64 m_graphBottom;
    qint64 m_graphTop;

    /* Viewport width (m_viewRight - m_viewLeft), used for zoom */
    qint64 m_viewWidth;
    qint64 m_viewWidthMin;
    qint64 m_viewWidthMax;

    /* Viewport height (m_viewTop - m_viewBottom), used for zoom */
    qint64 m_viewHeight;
    qint64 m_viewHeightMin;
    qint64 m_viewHeightMax;

    /* Mouse tracking */
    QPoint m_mousePressPosition;
    qint64 m_mousePressViewLeft;
    qint64 m_mousePressViewBottom;

    /* Selection */
    SelectionState* m_selectionState;

    /* State from the last update() call */
    struct PreviousUpdate {
        qint64 m_viewLeft;
        qint64 m_viewRight;
        qint64 m_viewBottom;
        qint64 m_viewTop;

        qint64 m_graphLeft;
        qint64 m_graphRight;
        qint64 m_graphBottom;
        qint64 m_graphTop;
    } m_previous;
};
