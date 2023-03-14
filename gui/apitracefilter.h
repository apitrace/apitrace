#pragma once

#include <QRegularExpression>
#include <QSortFilterProxyModel>

class ApiTraceCall;

class ApiTraceFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum FilterOption {
        NullFilter        = 0,
        ExtensionsFilter  = 1 << 0,
        ResolutionsFilter = 1 << 1,
        ErrorsQueryFilter = 1 << 2,
        ExtraStateFilter  = 1 << 3,
        CustomFilter      = 1 << 4,
    };
    Q_DECLARE_FLAGS(FilterOptions, FilterOption)
public:
    ApiTraceFilter(QObject *parent = 0);

    FilterOptions filterOptions() const;
    void setFilterOptions(FilterOptions opts);

    void setFilterRegexp(const QRegularExpression &regexp);
    QRegularExpression filterRegexp() const;

    void setCustomFilterRegexp(const QString &str);
    QString customFilterRegexp() const;

    QModelIndex indexForCall(ApiTraceCall *call) const;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QRegularExpression m_regexp;
    FilterOptions m_filters;
    QRegularExpression m_customRegexp;
};
