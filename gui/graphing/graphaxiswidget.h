#pragma once

#include "graphing.h"

#include <QWidget>

/**
 * The generic base class of all graph axes.
 *
 * Handles orientation, simple selections, and view area.
 */
class GraphAxisWidget : public QWidget {
    Q_OBJECT
public:
    enum Orientation {
        Horizontal,
        Vertical
    };

    enum SelectionStyle {
        None,
        Single,
        Range
    };

public:
    GraphAxisWidget(QWidget* parent = 0);
    virtual ~GraphAxisWidget(){}

    /* Is this axis part of the active selection */
    bool hasSelection();

    void setSelectable(SelectionStyle selectable);
    void setSelectionState(SelectionState* state);

    void setOrientation(Orientation v);

    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;

public slots:
    /* The minimum and maximum values of this axis */
    void setRange(qint64 min, qint64 max);

    /* The currently visible range of values */
    void setView(qint64 start, qint64 end);

signals:
    void selectionChanged();

protected:
    Orientation m_orientation;

    /* The min/max value of this axis */
    qint64 m_valueMin;
    qint64 m_valueMax;

    /* The highest and lowest currently visible value */
    qint64 m_valueBegin;
    qint64 m_valueEnd;

    /* Selection */
    SelectionStyle m_selectable;
    SelectionState* m_selectionState;

    /* Mouse tracking */
    QPoint m_mousePressPosition;
    qint64 m_mousePressValue;
};
