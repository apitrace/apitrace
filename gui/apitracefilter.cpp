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
    QString function = call->name;

    if (!m_text.isEmpty()) {
        return function.contains(m_text);
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
    }

    if (m_filters & ExtensionsFilter) {
        if (function.contains(QLatin1String("glXGetClientString")))
            return false;
        if (function.contains(QLatin1String("glXQueryExtensionsString")))
            return false;
        QString fullText = call->filterText();
        if (function.contains(QLatin1String("glGetString")) &&
            fullText.contains(QLatin1String("GL_EXTENSIONS")))
            return false;
    }


    return true;
}


void ApiTraceFilter::setFilterString(const QString &text)
{
    if (text != m_text) {
        m_text = text;
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

#include "apitracefilter.moc"
