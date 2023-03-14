#pragma once

#include <QDoubleSpinBox>

class AdaptiveDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit AdaptiveDoubleSpinBox(QWidget *parent = nullptr);

    virtual void stepBy(int steps) override;
};
