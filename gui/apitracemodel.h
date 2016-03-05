#pragma once


#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSet>
#include <QVariant>

class ApiTrace;
class ApiTraceCall;
class ApiTraceEvent;
class ApiTraceFrame;

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

    QModelIndex indexForCall(ApiTraceCall *call) const;

public:
    /* QAbstractItemModel { */
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    virtual bool canFetchMore(const QModelIndex & parent) const override;
    virtual void fetchMore(const QModelIndex &parent) override;
    /* } QAbstractItemModel; */

private slots:
    void invalidateFrames();
    void beginAddingFrames(int oldCount, int numAdded);
    void endAddingFrames();
    void changed(ApiTraceEvent *event);
    void callChanged(ApiTraceCall *call);
    void frameChanged(ApiTraceFrame *frame);
    void beginLoadingFrame(ApiTraceFrame *frame, int numAdded);
    void endLoadingFrame(ApiTraceFrame *frame);

private:
    ApiTraceEvent *item(const QModelIndex &index) const;

private:
    ApiTrace *m_trace;
    QSet<ApiTraceFrame*> m_loadingFrames;
};
