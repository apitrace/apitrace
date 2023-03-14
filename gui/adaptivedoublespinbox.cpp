#include "adaptivedoublespinbox.h"

#include <cmath>

AdaptiveDoubleSpinBox::AdaptiveDoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
}

void AdaptiveDoubleSpinBox::stepBy(int steps)
{
    const double oldValue = value();
    const double oldAbsValue = qAbs(oldValue);
    const double oldStep = std::pow(10, std::floor(std::log10(oldAbsValue)));

    const double newValueWithOldStep = oldValue + steps * oldStep;
    const double newAbsValueWithOldStep = qAbs(newValueWithOldStep);
    const double newStep = std::pow(10, std::floor(std::log10(newAbsValueWithOldStep)));

    if (newStep < oldStep) {
        if (oldAbsValue != oldStep) {
            setValue(oldStep);
        } else {
            setValue(oldValue + steps * (oldStep / 10));
        }
    } else {
        setValue(newValueWithOldStep);
    }
}
