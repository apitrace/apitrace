#ifndef APITRACEMODEL_H
#define APITRACEMODEL_H


#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class ApiTrace;
class ApiTraceCall;
class ApiTraceEvent;

class ApiTraceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Roles {
        EventRole = Qt::UserRole + 1
    };
public:
    ApiTraceModel(QObject *parent = 0);
    ~ApiTraceModel();

    void setApiTrace(ApiTrace *trace);
    const ApiTrace *apiTrace() const;
    void stateSetOnEvent(ApiTraceEvent *event);

    QModelIndex callIndex(int callNum) const;
    QModelIndex indexForCall(ApiTraceCall *call) const;

public:
    /* QAbstractItemModel { */
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex());
    /* } QAbstractItemModel; */

private slots:
    void invalidateFrames();
    void beginAddingFrames(int oldCount, int numAdded);
    void endAddingFrames();
    void callChanged(ApiTraceCall *call);

private:
    ApiTraceEvent *item(const QModelIndex &index) const;

private:
    ApiTrace *m_trace;
};

#endif
