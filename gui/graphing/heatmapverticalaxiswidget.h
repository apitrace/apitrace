#pragma once

#include "heatmapview.h"
#include "graphaxiswidget.h"

/**
 * Vertical axis specifically for heatmap displaying header and data rows
 */
class HeatmapVerticalAxisWidget : public GraphAxisWidget {
public:
    HeatmapVerticalAxisWidget(QWidget* parent);

    void setDataProvider(HeatmapDataProvider* data);

    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
    virtual void paintEvent(QPaintEvent *) override;

protected:
    int m_rowHeight;
    HeatmapDataProvider* m_data;
};
