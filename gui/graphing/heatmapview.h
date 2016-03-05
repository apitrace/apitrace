#pragma once

#include "graphview.h"

/**
 * The heatmap iterator will only return data when there is something to draw,
 * this allows much faster access to the data in the case where the view is
 * zoomed out to the point of where there is multiple calls in one pixel,
 * it automagically calculates the heat for that pixel.
 */
class HeatmapRowIterator {
public:
    virtual ~HeatmapRowIterator(){}

    /* Go to the next visible heat map */
    virtual bool next() = 0;

    /* Is the current value GPU or CPU heat */
    virtual bool isGpu() const = 0;

    /* Current step (normally x coordinate) */
    virtual int step() const = 0;

    /* Current width (in steps) */
    virtual int width() const = 0;

    /* Current heat */
    virtual float heat() const = 0;

    /* Heat value for selected calls */
    virtual float selectedHeat() const = 0;

    /* Label only used when there is a single call in this heat */
    virtual QString label() const = 0;
};


/**
 * Data provider for the whole heatmap
 */
class HeatmapDataProvider {
public:
    virtual ~HeatmapDataProvider(){}

    /* The start and end values (time on x-axis) for the heatmap */
    virtual qint64 start() const = 0;
    virtual qint64 end() const = 0;

    /*
     * Header rows (fixed at top of heatmap view)
     */

    /* Header row count */
    virtual unsigned headerRows() const = 0;

    /* Label to be used in the vertical axis */
    virtual QString headerLabel(unsigned row) const = 0;

    /* Get identifier (program no) for row */
    virtual qint64 headerRowAt(unsigned row) const = 0;

    /* Get item at row and time */
    virtual qint64 headerItemAt(unsigned row, qint64 time) const = 0;

    /* Get iterator for a row between start and end time for steps */
    virtual HeatmapRowIterator* headerRowIterator(int row, qint64 start, qint64 end, int steps) const = 0;

    /*
     * Data rows (scrollable in heatmap view)
     */

    /* Data row count */
    virtual unsigned dataRows() const = 0;

    /* Label to be used in the vertical axis */
    virtual QString dataLabel(unsigned row) const = 0;

    /* Get identifier (program no) for row */
    virtual qint64 dataRowAt(unsigned row) const = 0;

    /* Get item at row and time */
    virtual qint64 dataItemAt(unsigned row, qint64 time) const = 0;

    /* Get iterator for a row between start and end time for steps */
    virtual HeatmapRowIterator* dataRowIterator(int row, qint64 start, qint64 end, int steps) const = 0;

    /* Handle double click on item */
    virtual void itemDoubleClicked(qint64 index) const = 0;

    /* Get mouse over tooltip for item */
    virtual QString itemTooltip(qint64 index) const = 0;

    /* Set the selection */
    virtual void setSelectionState(SelectionState* state) = 0;
};


/**
 * A not very generic heatmap for row based data
 */
class HeatmapView : public GraphView {
public:
    HeatmapView(QWidget* parent);

    void setDataProvider(HeatmapDataProvider* data);
    void setSelectionState(SelectionState* state) override;

    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

    virtual void paintEvent(QPaintEvent *e) override;
    virtual void paintRow(QPainter& painter, HeatmapRowIterator* itr);


protected:
    qint64 itemAtPosition(QPoint pos);

protected:
    int m_rowHeight;
    HeatmapDataProvider* m_data;
};
