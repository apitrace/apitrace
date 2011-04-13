#ifndef APITRACEFILTER_H
#define APITRACEFILTER_H

#include <QRegExp>
#include <QSortFilterProxyModel>

class ApiTraceCall;

class ApiTraceFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum FilterOption {
        NullFilter        =  0,
        ExtensionsFilter  = 1 << 0,
        ResolutionsFilter = 1 << 1,
        ErrorsQueryFilter = 1 << 2,
        ExtraStateFilter  = 1 << 3,
    };
    Q_DECLARE_FLAGS(FilterOptions, FilterOption)
public:
    ApiTraceFilter(QObject *parent = 0);

    FilterOptions filterOptions() const;
    void setFilterOptions(FilterOptions opts);

    void setFilterRegexp(const QRegExp &regexp);
    QRegExp filterRegexp() const;

    QModelIndex callIndex(int callNum) const;
    QModelIndex indexForCall(ApiTraceCall *call) const;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QRegExp m_regexp;
    FilterOptions m_filters;
};

#endif
