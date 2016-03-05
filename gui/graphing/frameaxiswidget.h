#pragma once

#include "graphaxiswidget.h"

class FrameDataProvider {
public:
    virtual ~FrameDataProvider() {}

    /* Number of frames */
    virtual unsigned size() const = 0;

    /* Start and end values of frame */
    virtual qint64 frameStart(unsigned frame) const = 0;
    virtual qint64 frameEnd(unsigned frame) const = 0;
};

/**
 * A generic axis which will draw frame numbers over a period of values.
 * Frames designated by start / end values.
 */
class FrameAxisWidget : public GraphAxisWidget {
public:
    FrameAxisWidget(QWidget* parent = 0);

    void setDataProvider(FrameDataProvider* data);

    virtual void paintEvent(QPaintEvent *e) override;

protected:
    FrameDataProvider* m_data;
};
