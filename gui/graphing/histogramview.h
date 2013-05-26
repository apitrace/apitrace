#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include "graphview.h"

/**
 * Histogram graph view.
 *
 * When the view is zoomed such that there is more than one item occupying
 * a single pixel the one with the highest value will be displayed.
 */
class HistogramView : public GraphView {
public:
    HistogramView(QWidget* parent);

    void setDataProvider(GraphDataProvider* data);
    void setSelectionState(SelectionState* state);

    /* Gradient colours for selected and unselected items */
    void setSelectedGradient(const QLinearGradient& gradient);
    void setUnelectedGradient(const QLinearGradient& gradient);

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void update();
    virtual void resizeEvent(QResizeEvent *e);
    virtual void paintEvent(QPaintEvent *e);

protected:
    qint64 itemAtPosition(QPoint pos);
    qint64 valueAtPosition(QPoint pos);

protected:
    QLinearGradient m_gradientSelected;
    QLinearGradient m_gradientUnselected;

    GraphDataProvider* m_data;
};

#endif
