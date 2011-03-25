#ifndef APITRACECALL_H
#define APITRACECALL_H

#include <QStringList>
#include <QVariant>

class ApiTraceCall
{
public:
    QString name;
    QStringList argNames;
    QVariantList argValues;
    QVariant returnValue;
};

#endif
