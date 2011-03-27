#ifndef APITRACEFILTER_H
#define APITRACEFILTER_H

#include <QSortFilterProxyModel>

class ApiTraceFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ApiTraceFilter(QObject *parent = 0);

    void setFilterString(const QString &text);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QString m_text;
};

#endif
