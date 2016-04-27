#include "apitracemodel.h"

#include "apitracecall.h"
#include "traceloader.h"
#include "trace_parser.hpp"

#include <QBuffer>
#include <QDebug>
#include <QImage>
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

    if (index.column() != 0)
        return QVariant();

    ApiTraceEvent *itm = item(index);
    if (!itm) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return itm->staticText().text();
    case Qt::DecorationRole:
        return QImage();
    case Qt::ToolTipRole: {
        const QString stateText = tr("State info available.");
        if (itm->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call = static_cast<ApiTraceCall*>(itm);
            if (!call->hasState())
                return QString::fromLatin1("%1)&nbsp;<b>%2</b>")
                    .arg(call->index())
                    .arg(call->name());
            else
                return QString::fromLatin1("%1)&nbsp;<b>%2</b><br/>%3")
                    .arg(call->index())
                    .arg(call->name())
                    .arg(stateText);
        } else {
            const char *htmlTempl =
                    "<div>\n"
                    "<div>\n"
                    "%1"
                    "<span style=\"font-weight:bold; font-size:large; vertical-align:center; padding-bottom: 30px \">\n"
                    "Frame %2</span>\n"
                    "</div>\n"
                    "<div >%3 calls%4</div>\n"
                    "</div>\n";


            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(itm);
            QString thumbStr, sizeStr;

            if (frame->hasState()) {
                static const char *imgTempl =
                        "<img style=\"float:left;\" "
                        "src=\"data:image/png;base64,%1\"/>\n";
                static const char *sizeTempl =
                        ", %1kb";

                ApiFramebuffer fbo = frame->state()->colorBuffer();
                QImage thumb = fbo.calculateThumbnail(false, false);
                if (!thumb.isNull()) {
                    QByteArray ba;
                    QBuffer buffer(&ba);
                    buffer.open(QIODevice::WriteOnly);
                    thumb.save(&buffer, "PNG");
                    thumbStr = tr(imgTempl).arg(
                                QString(buffer.data().toBase64()));
                }

                int binaryDataSize = frame->binaryDataSize() / 1024;
                if (binaryDataSize > 0) {
                    sizeStr = tr(sizeTempl).arg(binaryDataSize);
                }
            }

            int numCalls = frame->isLoaded()
                    ? frame->numTotalCalls()
                    : frame->numChildrenToLoad();

            return tr(htmlTempl)
                    .arg(thumbStr)
                    .arg(frame->number)
                    .arg(numCalls)
                    .arg(sizeStr);
        }
    }
    case ApiTraceModel::EventRole:
        return QVariant::fromValue(itm);
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
            return tr("Events");
        case 1:
            return tr("Flags");
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
    if ((parent.isValid() && parent.column() != 0) || column != 0)
        return QModelIndex();

    //qDebug()<<"At row = "<<row<<", column = "<<column<<", parent "<<parent;
    ApiTraceEvent *parentEvent = item(parent);
    if (parentEvent) {
        ApiTraceEvent *event = parentEvent->eventAtRow(row);
        if (event) {
            Q_ASSERT(event->type() == ApiTraceEvent::Call);
            return createIndex(row, column, event);
        } else {
            return QModelIndex();
        }
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
        ApiTraceEvent *event = item(parent);
        if (!event)
            return false;
        if (event->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);
            return !frame->isEmpty();
        } else {
            Q_ASSERT(event->type() == ApiTraceEvent::Call);
            ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
            return call->numChildren() != 0;
        }
    } else {
        return (rowCount() > 0);
    }
}

QModelIndex ApiTraceModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ApiTraceEvent *event = item(index);

    if (event->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(event);

        if (call->parentCall()) {
            ApiTraceCall *parentCall = call->parentCall();
            ApiTraceEvent *topEvent = parentCall->parentEvent();
            return createIndex(topEvent->callIndex(parentCall), 0, parentCall);
        } else {
            Q_ASSERT(call->parentFrame());
            return createIndex(call->parentFrame()->number,
                               0, call->parentFrame());
        }
    }
    return QModelIndex();
}

int ApiTraceModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_trace->numFrames();

    ApiTraceEvent *event = item(parent);
    if (!event)
        return 0;

    return event->numChildren();
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
    connect(m_trace, SIGNAL(invalidated()),
            this, SLOT(invalidateFrames()));
    connect(m_trace, SIGNAL(framesInvalidated()),
            this, SLOT(invalidateFrames()));
    connect(m_trace, SIGNAL(beginAddingFrames(int, int)),
            this, SLOT(beginAddingFrames(int, int)));
    connect(m_trace, SIGNAL(endAddingFrames()),
            this, SLOT(endAddingFrames()));
    connect(m_trace, SIGNAL(changed(ApiTraceEvent*)),
            this, SLOT(changed(ApiTraceEvent*)));
    connect(m_trace, SIGNAL(beginLoadingFrame(ApiTraceFrame*,int)),
            this, SLOT(beginLoadingFrame(ApiTraceFrame*,int)));
    connect(m_trace, SIGNAL(endLoadingFrame(ApiTraceFrame*)),
            this, SLOT(endLoadingFrame(ApiTraceFrame*)));

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

void ApiTraceModel::beginAddingFrames(int oldCount, int numAdded)
{
    beginInsertRows(QModelIndex(), oldCount,
                    oldCount + numAdded - 1);
}

ApiTraceEvent * ApiTraceModel::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return static_cast<ApiTraceEvent*>(index.internalPointer());
}

void ApiTraceModel::stateSetOnEvent(ApiTraceEvent *event)
{
    if (!event)
        return;

    if (event->type() == ApiTraceEvent::Call) {
        ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
        QModelIndex index = indexForCall(call);
        emit dataChanged(index, index);
    } else if (event->type() == ApiTraceEvent::Frame) {
        ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);
        const QList<ApiTraceFrame*> & frames = m_trace->frames();
        int row = frames.indexOf(frame);
        QModelIndex index = createIndex(row, 0, frame);
        emit dataChanged(index, index);
    }
}

QModelIndex ApiTraceModel::indexForCall(ApiTraceCall *call) const
{
    if (!call) {
        return QModelIndex();
    }

    ApiTraceEvent *parentEvent = call->parentEvent();
    Q_ASSERT(parentEvent);

    int row = parentEvent->callIndex(call);
    if (row < 0) {
        qDebug() << "Couldn't find call num "<<call->index()<<" inside parent!";
        return QModelIndex();
    }
    return createIndex(row, 0, call);
}

void ApiTraceModel::changed(ApiTraceEvent *event)
{
    if (event->type() == ApiTraceEvent::Call) {
        callChanged(static_cast<ApiTraceCall*>(event));
    } else if (event->type() == ApiTraceEvent::Frame) {
        frameChanged(static_cast<ApiTraceFrame*>(event));
    }
}

void ApiTraceModel::callChanged(ApiTraceCall *call)
{
    ApiTrace *trace = call->parentFrame()->parentTrace();

#if 0
    qDebug()<<"Call changed = "<<call->edited();
    qDebug()<<"\ttrace edited = "<<trace->edited();
    qDebug()<<"\ttrace file = "<<trace->fileName();
    qDebug()<<"\ttrace needs saving = "<<trace->needsSaving();
#endif

    Q_ASSERT(trace);
    if (trace->needsSaving())
        trace->save();

    QModelIndex index = indexForCall(call);
    emit dataChanged(index, index);
}

void ApiTraceModel::frameChanged(ApiTraceFrame *frame)
{
    const QList<ApiTraceFrame*> & frames = m_trace->frames();
    int row = frames.indexOf(frame);
    QModelIndex index = createIndex(row, 0, frame);
    emit dataChanged(index, index);
}

void ApiTraceModel::endAddingFrames()
{
    endInsertRows();
}

bool ApiTraceModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        ApiTraceEvent *event = item(parent);
        if (event && event->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);
            return !frame->isLoaded() && !m_loadingFrames.contains(frame);
        } else
            return false;
    } else {
        return false;
    }
}

void ApiTraceModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid()) {
        ApiTraceEvent *event = item(parent);
        if (event && event->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);

            Q_ASSERT(!frame->isLoaded());
            m_loadingFrames.insert(frame);

            m_trace->loadFrame(frame);
        }
    }
}

void ApiTraceModel::beginLoadingFrame(ApiTraceFrame *frame, int numAdded)
{
    QModelIndex index = createIndex(frame->number, 0, frame);
    beginInsertRows(index, 0, numAdded - 1);
}

void ApiTraceModel::endLoadingFrame(ApiTraceFrame *frame)
{
    QModelIndex index = createIndex(frame->number, 0, frame);
#if 0
    qDebug()<<"Frame loaded = "<<frame->loaded();
    qDebug()<<"\tframe idx = "<<frame->number;
    qDebug()<<"\tis empty = "<<frame->isEmpty();
    qDebug()<<"\tnum children = "<<frame->numChildren();
    qDebug()<<"\tindex is "<<index;
#endif

    endInsertRows();

    emit dataChanged(index, index);

    m_loadingFrames.remove(frame);
}

#include "apitracemodel.moc"
