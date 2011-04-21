#include "apitracefilter.h"

#include "apitracecall.h"
#include "apitracemodel.h"

#include <QDebug>

ApiTraceFilter::ApiTraceFilter(QObject *parent)
    : QSortFilterProxyModel(),
      m_filters(ExtensionsFilter | ResolutionsFilter |
                ErrorsQueryFilter | ExtraStateFilter)
{
}

bool ApiTraceFilter::filterAcceptsRow(int sourceRow,
                                      const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QVariant varientData = sourceModel()->data(index0, ApiTraceModel::EventRole);
    ApiTraceEvent *event = varientData.value<ApiTraceEvent*>();

    Q_ASSERT(event);
    if (!event)
        return false;

    //we don't filter frames
    if (event->type() == ApiTraceEvent::Frame) {
        return true;
    }

    ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
    QString function = call->name();

    if (!m_regexp.isEmpty() && m_regexp.isValid()) {
        return function.contains(m_regexp);
    }

    if (m_filters & ResolutionsFilter) {
        if (function.contains(QLatin1String("glXGetProcAddress")))
            return false;
        if (function.contains(QLatin1String("wglGetProcAddress")))
            return false;
    }

    if (m_filters & ErrorsQueryFilter) {
        if (function.contains(QLatin1String("glGetError")))
            return false;
    }

    if (m_filters & ExtraStateFilter) {
        if (function.contains(QLatin1String("glXGetCurrentDisplay")))
            return false;
        if (function.contains(QLatin1String("wglDescribePixelFormat")))
            return false;
    }

    if (m_filters & ExtensionsFilter) {
        if (function.contains(QLatin1String("glXGetClientString")))
            return false;
        if (function.contains(QLatin1String("glXQueryExtensionsString")))
            return false;
        if (function.contains(QLatin1String("glGetString")))
            return false;
    }


    return true;
}


void ApiTraceFilter::setFilterRegexp(const QRegExp &regexp)
{
    if (regexp != m_regexp) {
        m_regexp = regexp;
        invalidate();
    }
}

ApiTraceFilter::FilterOptions ApiTraceFilter::filterOptions() const
{
    return m_filters;
}

void ApiTraceFilter::setFilterOptions(ApiTraceFilter::FilterOptions opts)
{
    if (opts != m_filters) {
        m_filters = opts;
        invalidate();
    }
}

QModelIndex ApiTraceFilter::callIndex(int callIdx) const
{
    ApiTraceModel *model = static_cast<ApiTraceModel *>(sourceModel());
    QModelIndex index = model->callIndex(callIdx);
    return mapFromSource(index);
}

QModelIndex ApiTraceFilter::indexForCall(ApiTraceCall *call) const
{
    ApiTraceModel *model = static_cast<ApiTraceModel *>(sourceModel());
    QModelIndex index = model->indexForCall(call);
    return mapFromSource(index);
}

QRegExp ApiTraceFilter::filterRegexp() const
{
    return m_regexp;
}

#include "apitracefilter.moc"
