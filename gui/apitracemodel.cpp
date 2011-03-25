#include "apitracemodel.h"

#include "apitracecall.h"
#include "loaderthread.h"
#include "trace_parser.hpp"

#include <QVariant>

class VariantVisitor : public Trace::Visitor
{
public:
    virtual void visit(Trace::Null *)
    {
        // Nothing
    }
    virtual void visit(Trace::Bool *node)
    {
        m_variant = QVariant(node->value);
    }
    virtual void visit(Trace::SInt *node)
    {
        m_variant = QVariant(node->value);
    }
    virtual void visit(Trace::UInt *node)
    {
        m_variant = QVariant(node->value);
    }
    virtual void visit(Trace::Float *node)
    {
        m_variant = QVariant(node->value);
    }
    virtual void visit(Trace::String *node)
    {
        m_variant = QVariant(QString::fromStdString(node->value));
    }
    virtual void visit(Trace::Enum *e)
    {
        m_variant = QVariant(QString::fromStdString(e->sig->first));
    }
    virtual void visit(Trace::Bitmask *bitmask)
    {
        //XXX we should probably convert it to QImage
        visit(static_cast<Trace::UInt *>(bitmask));
    }
    virtual void visit(Trace::Struct *str)
    {
        //XXX: need a custom QVariant type for this one
        QVariantList lst;
        for (int i = 0; i < str->members.size(); ++i) {
            VariantVisitor vst;
            str->members[i]->visit(vst);
            lst.append(vst.variant());
        }
        m_variant = QVariant(lst);
    }
    virtual void visit(Trace::Array *array)
    {
        QVariantList lst;
        for (int i = 0; i < array->values.size(); ++i) {
            VariantVisitor vst;
            array->values[i]->visit(vst);
            lst.append(vst.variant());
        }
        m_variant = QVariant(lst);
    }
    virtual void visit(Trace::Blob *blob)
    {
        QByteArray barray = QByteArray::fromRawData(blob->buf, blob->size);
        m_variant = QVariant(barray);
    }
    virtual void visit(Trace::Pointer *ptr)
    {
        m_variant = QVariant(ptr->value);
    }

    QVariant variant() const
    {
        return m_variant;
    }
private:
    QVariant m_variant;
};

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
    case 0:
        return item->name;
    case 1: {
        QString str;
        for (int i = 0; i < item->argNames.count(); ++i) {
            str += item->argNames[i];
            str += QString::fromLatin1(" = ");
            str += item->argValues[i].toString();
            if (i < item->argNames.count() - 1)
                str += QString::fromLatin1(", ");
        }
        return str;
    }
    case 2:
        return item->returnValue.toString();
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
        case 1:
            return tr("Arguments");
        case 2:
            return tr("Return");
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
    return parent.isValid() ? 0 : 3;
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
        VariantVisitor retVisitor;
        if (call->ret) {
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
