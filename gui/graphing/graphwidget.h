#pragma once

#include "graphview.h"
#include "graphaxiswidget.h"
#include "graphlabelwidget.h"

class QScrollBar;

/**
 * The generic GraphWidget class which combines the elements of a graph,
 * the axis, view, scrollbars and label.
 */
class GraphWidget : public QWidget {
    Q_OBJECT
public:
    enum AxisPosition {
        AxisTop,
        AxisLeft,
        AxisRight,
        AxisBottom
    };

public:
    GraphWidget(QWidget* parent = 0);
    virtual ~GraphWidget(){}

    GraphView* view();
    GraphLabelWidget* label();
    GraphAxisWidget* axis(AxisPosition pos);

    void setView(GraphView* view);
    void setLabel(GraphLabelWidget* label);
    void setAxis(AxisPosition pos, GraphAxisWidget* axis);

    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);

    virtual void resizeEvent(QResizeEvent *e) override;

protected:
    /* Used if a selection would be shared between graphs with different axis */
    virtual SelectionState transformSelectionIn(SelectionState state);
    virtual SelectionState transformSelectionOut(SelectionState state);

    /* Update the scrollbars based on current view */
    void updateScrollbars();

    /* Update all signal / slot connections */
    void updateConnections();

    /* Recalculate child widget layout */
    void updateLayout();

public slots:
    void setSelection(SelectionState state);

    /* Set view areas */
    void setHorizontalView(qint64 start, qint64 end);
    void setVerticalView(qint64 start, qint64 end);

protected slots:
    /* View changed by translation / zooming */
    void verticalViewChange(qint64 start, qint64 end);
    void verticalRangeChange(qint64 start, qint64 end);
    void horizontalViewChange(qint64 start, qint64 end);
    void horizontalRangeChange(qint64 start, qint64 end);

    /* User interaction with scroll bars */
    void horizontalScrollAction(int action);
    void verticalScrollAction(int action);

    /* Update child elements when selection changes */
    void updateSelection(bool emitSignal = true);

signals:
    void selectionChanged(SelectionState state);

    void verticalViewChanged(qint64 start, qint64 end);
    void verticalRangeChanged(qint64 start, qint64 end);

    void horizontalViewChanged(qint64 start, qint64 end);
    void horizontalRangeChanged(qint64 start, qint64 end);

protected:
    SelectionState m_selection;

    GraphView* m_view;

    GraphLabelWidget* m_label;

    GraphAxisWidget* m_axisTop;
    GraphAxisWidget* m_axisLeft;
    GraphAxisWidget* m_axisRight;
    GraphAxisWidget* m_axisBottom;

    QScrollBar* m_horizontalScrollbar;
    qint64 m_horizontalMin;
    qint64 m_horizontalMax;
    qint64 m_horizontalStart;
    qint64 m_horizontalEnd;
    Qt::ScrollBarPolicy m_horizontalScrollbarPolicy;

    QScrollBar* m_verticalScrollbar;
    qint64 m_verticalMin;
    qint64 m_verticalMax;
    qint64 m_verticalStart;
    qint64 m_verticalEnd;
    Qt::ScrollBarPolicy m_verticalScrollbarPolicy;
};
