#ifndef HEATMAPVERTICALAXISWIDGET_H
#define HEATMAPVERTICALAXISWIDGET_H

#include "heatmapview.h"
#include "graphaxiswidget.h"

/**
 * Vertical axis specifically for heatmap displaying header and data rows
 */
class HeatmapVerticalAxisWidget : public GraphAxisWidget {
public:
    HeatmapVerticalAxisWidget(QWidget* parent);

    void setDataProvider(HeatmapDataProvider* data);

    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *);

protected:
    int m_rowHeight;
    HeatmapDataProvider* m_data;
};

#endif
