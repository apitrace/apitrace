#include "apitracemodel.h"

#include "apitracecall.h"
#include "loaderthread.h"
#include "trace_parser.hpp"

#include <QDebug>
#include <QVariant>


ApiTraceModel::ApiTraceModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_loader = new LoaderThread();

    connect(m_loader, SIGNAL(parsedCalls(const QList<Trace::Call*>&)),
            SLOT(appendCalls(const QList<Trace::Call*>&)));
}

ApiTraceModel::~ApiTraceModel()
{
    qDeleteAll(m_calls);
    delete m_loader;
}

QVariant ApiTraceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    ApiTraceCall *item = m_calls.value(index.row());

    if (!item)
        return QVariant();

    switch (index.column()) {
    case 0: {
        return item->richText();
    }
    default:
        return QVariant();
    }
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
            return tr("Function");
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

    ApiTraceCall *call = m_calls.value(row);

    if (call)
        return createIndex(row, column, call);
    else
        return QModelIndex();
}

bool ApiTraceModel::hasChildren(const QModelIndex &parent) const
{
    return parent.isValid() ? false : (rowCount() > 0);
}

QModelIndex ApiTraceModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    //list for now
    return QModelIndex();
}

int ApiTraceModel::rowCount(const QModelIndex &parent) const
{
    return m_calls.count();
}

int ApiTraceModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
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

    if (m_calls.count() <= position)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    for (int i = 0; i < rows; ++i) {
        ApiTraceCall *call = m_calls.value(i);
        m_calls.removeAt(i);
        delete call;
    }
    endRemoveRows();

    return success;
}

void ApiTraceModel::loadTraceFile(const QString &fileName)
{
    if (m_loader->isRunning()) {
        m_loader->terminate();
        m_loader->wait();
    }
    removeRows(0, m_calls.count(), QModelIndex());

    m_loader->loadFile(fileName);
}

void ApiTraceModel::appendCalls(const QList<Trace::Call*> traceCalls)
{
    beginInsertRows(QModelIndex(), m_calls.count(),
                    m_calls.count() + traceCalls.count());
    foreach(Trace::Call *call, traceCalls) {
        ApiTraceCall *apiCall = new ApiTraceCall;
        apiCall->name = QString::fromStdString(call->sig->name);

        QString argumentsText;
        for (int i = 0; i < call->sig->arg_names.size(); ++i) {
            apiCall->argNames +=
                QString::fromStdString(call->sig->arg_names[i]);
        }
        if (call->ret) {
            VariantVisitor retVisitor;
            call->ret->visit(retVisitor);
            apiCall->returnValue = retVisitor.variant();
        }
        for (int i = 0; i < call->args.size(); ++i) {
            VariantVisitor argVisitor;
            call->args[i]->visit(argVisitor);
            apiCall->argValues += argVisitor.variant();
        }

        m_calls.append(apiCall);
    }
    endInsertRows();

    qDeleteAll(traceCalls);
}

#include "apitracemodel.moc"
