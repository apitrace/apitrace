#pragma once

#include <QString>

/**
 * A simple struct to hold a horizontal or vertical selection
 */
struct SelectionState {
    enum SelectionType {
        None,
        Horizontal,
        Vertical
    };

    SelectionType type;
    qint64 start;
    qint64 end;
};


/**
 * Fairly generic data provider for graphs
 */
class GraphDataProvider {
public:
    virtual ~GraphDataProvider(){}

    /* Number of elements in graph */
    virtual qint64 size() const = 0;

    /* Returns value for index */
    virtual qint64 value(qint64 index) const = 0;

    /* Is the item at index selected */
    virtual bool selected(qint64 index) const = 0;

    /* Get mouse hover tooltip for item */
    virtual QString itemTooltip(qint64 index) const = 0;

    /* Called on item double click */
    virtual void itemDoubleClicked(qint64 index) const = 0;

    /* Set pointer to selection state */
    virtual void setSelectionState(SelectionState* state) = 0;
};
