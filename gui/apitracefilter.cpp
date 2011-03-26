#include "apitracefilter.h"

#include "apitracecall.h"

ApiTraceFilter::ApiTraceFilter(QObject *parent )
    : QSortFilterProxyModel()
{
}

bool ApiTraceFilter::filterAcceptsRow(int sourceRow,
                                      const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QString function = sourceModel()->data(index0).toString();
    QString arguments = sourceModel()->data(index1).toString();

    //XXX make it configurable
    if (function.contains(QLatin1String("glXGetProcAddress")))
        return false;
    if (function.contains(QLatin1String("wglGetProcAddress")))
        return false;

    if (function.contains(QLatin1String("glGetString")) &&
        arguments.contains(QLatin1String("GL_EXTENSIONS")))
        return false;

    return true;
}


#include "apitracefilter.moc"
