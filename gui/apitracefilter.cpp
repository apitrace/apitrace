#include "apitracefilter.h"

#include "apitracecall.h"
#include "apitracemodel.h"

#include <QDebug>

ApiTraceFilter::ApiTraceFilter(QObject *parent)
    : QSortFilterProxyModel()
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

    //XXX make it configurable
    if (function.contains(QLatin1String("glXGetProcAddress")))
        return false;
    if (function.contains(QLatin1String("wglGetProcAddress")))
        return false;

    QString fullText = call->filterText();
    if (function.contains(QLatin1String("glGetString")) &&
        fullText.contains(QLatin1String("GL_EXTENSIONS")))
        return false;

    return true;
}


void ApiTraceFilter::setFilterString(const QString &text)
{
    if (text != m_text) {
        m_text = text;
        invalidate();
    }
}

#include "apitracefilter.moc"
