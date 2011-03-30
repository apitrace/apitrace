#include "apitracemodel.h"

#include "apitracecall.h"
#include "loaderthread.h"
#include "trace_parser.hpp"

#include <QDebug>
#include <QVariant>


ApiTraceModel::ApiTraceModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_trace(0)
{
}

ApiTraceModel::~ApiTraceModel()
{
    m_trace = 0;
}

QVariant ApiTraceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    //data only in the first column
    if (index.column() != 0)
        return QVariant();

    ApiTraceEvent *itm = item(index);
    if (itm) {
        if (itm->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame =
                static_cast<ApiTraceFrame *>(itm);
            return QVariant::fromValue(frame);
        } else if (itm->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call =
                static_cast<ApiTraceCall *>(itm);
            return QVariant::fromValue(call);
        }
    }

    return QVariant();
}

Qt::ItemFlags ApiTraceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ApiTraceModel::headerData(int section, Qt::Orientation orientation,
                                   int role ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Event");
        default:
            //fall through
            break;
        }
    }

    return QVariant();
}

QModelIndex ApiTraceModel::index(int row, int column,
                                 const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    if (parent.isValid()) {
        QVariant data = parent.data();
        ApiTraceFrame *frame = data.value<ApiTraceFrame*>();
        if (!frame) {
            qDebug()<<"got a valid parent but it's not a frame "<<data;
            return QModelIndex();
        }
        ApiTraceCall *call = frame->calls.value(row);
        if (call)
            return createIndex(row, column, call);
        else
            return QModelIndex();
    } else {
        ApiTraceFrame *frame = m_trace->frameAt(row);
        if (frame)
            return createIndex(row, column, frame);
        else
            return QModelIndex();
    }
    return QModelIndex();
}

bool ApiTraceModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        ApiTraceFrame *frame = parent.data().value<ApiTraceFrame*>();
        if (frame)
            return !frame->calls.isEmpty();
        else
            return false;
    } else {
        return (rowCount() > 0);
    }
}

QModelIndex ApiTraceModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ApiTraceCall *call = index.data().value<ApiTraceCall*>();
    if (call) {
        Q_ASSERT(call->parentFrame);
        return createIndex(call->parentFrame->number,
                           0, call->parentFrame);
    }
    return QModelIndex();
}

int ApiTraceModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_trace->numFrames();

    ApiTraceCall *call = parent.data().value<ApiTraceCall*>();
    if (call)
        return 0;

    ApiTraceFrame *frame = parent.data().value<ApiTraceFrame*>();
    if (frame)
        return frame->calls.count();

    return 0;
}

int ApiTraceModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}


bool ApiTraceModel::insertRows(int position, int rows,
                               const QModelIndex &parent)
{
    return false;
}

bool ApiTraceModel::removeRows(int position, int rows,
                               const QModelIndex &parent)
{
    bool success = true;

    Q_UNUSED(parent);

    beginRemoveRows(parent, position, position + rows - 1);
    //XXX remove it from ApiTrace
    endRemoveRows();

    return success;
}

void ApiTraceModel::setApiTrace(ApiTrace *trace)
{
    if (m_trace == trace)
        return;
    if (m_trace)
        disconnect(m_trace);
    m_trace = trace;
    connect(m_trace, SIGNAL(framesInvalidated()),
            this, SLOT(invalidateFrames()));
    connect(m_trace, SIGNAL(framesAdded(int, int)),
            this, SLOT(appendFrames(int, int)));
}

const ApiTrace * ApiTraceModel::apiTrace() const
{
    return m_trace;
}

void ApiTraceModel::invalidateFrames()
{
    beginResetModel();
    endResetModel();
}

void ApiTraceModel::appendFrames(int oldCount, int numAdded)
{
    beginInsertRows(QModelIndex(), oldCount,
                    oldCount + numAdded);
    endInsertRows();
}

ApiTraceEvent * ApiTraceModel::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return static_cast<ApiTraceEvent*>(index.internalPointer());
}

#include "apitracemodel.moc"
