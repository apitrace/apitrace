#include "saverthread.h"

#include "trace_writer.hpp"
#include "trace_model.hpp"
#include "trace_parser.hpp"

#include <QFile>
#include <QHash>
#include <QUrl>

#include <QDebug>

#if 0
static Trace::FunctionSig *
createFunctionSig(ApiTraceCall *call, unsigned id)
{
    Trace::FunctionSig *sig = new Trace::FunctionSig();

    sig->id = id;
    sig->name = qstrdup(call->name().toLocal8Bit());

    QStringList args = call->argNames();
    sig->num_args = args.count();
    sig->arg_names = new const char*[args.count()];
    for (int i = 0; i < args.count(); ++i) {
        sig->arg_names[i] = qstrdup(args[i].toLocal8Bit());
    }

    return sig;
}

static void
deleteFunctionSig(Trace::FunctionSig *sig)
{
    for (int i = 0; i < sig->num_args; ++i) {
        delete [] sig->arg_names[i];
    }
    delete [] sig->arg_names;
    delete [] sig->name;
    delete sig;
}

static Trace::StructSig *
createStructSig(const ApiStruct &str, unsigned id)
{
    ApiStruct::Signature aSig = str.signature();

    Trace::StructSig *sig = new Trace::StructSig();
    sig->id = id;
    sig->name = qstrdup(aSig.name.toLocal8Bit());
    sig->num_members = aSig.memberNames.count();
    char **member_names = new char*[aSig.memberNames.count()];
    sig->member_names = (const char **)member_names;
    for (int i = 0; i < aSig.memberNames.count(); ++i) {
        member_names[i] = qstrdup(aSig.memberNames[i].toLocal8Bit());
    }
    return sig;
}

static void
deleteStructSig(Trace::StructSig *sig)
{
    for (int i = 0; i < sig->num_members; ++i) {
        delete [] sig->member_names[i];
    }
    delete [] sig->member_names;
    delete [] sig->name;
    delete sig;
}

static Trace::EnumSig *
createEnumSig(const ApiEnum &en, unsigned id)
{
    Trace::EnumSig *sig = new Trace::EnumSig();

    sig->id = id;
    sig->name = qstrdup(en.name().toLocal8Bit());
    sig->value = en.value().toLongLong();

    return sig;
}

static void
deleteEnumSig(Trace::EnumSig *sig)
{
    delete [] sig->name;
    delete sig;
}

static Trace::BitmaskSig *
createBitmaskSig(const ApiBitmask &bt, unsigned id)
{
    ApiBitmask::Signature bsig = bt.signature();
    ApiBitmask::Signature::const_iterator itr;

    Trace::BitmaskSig *sig = new Trace::BitmaskSig();
    Trace::BitmaskFlag *flags = new Trace::BitmaskFlag[bsig.count()];

    sig->id = id;
    sig->num_flags = bsig.count();
    sig->flags = flags;

    int i = 0;
    for (itr = bsig.constBegin(); itr != bsig.constEnd(); ++itr, ++i) {
        flags[i].name = qstrdup(itr->first.toLocal8Bit());
        flags[i].value = itr->second;
    }

    return sig;
}

static void
deleteBitmaskSig(Trace::BitmaskSig *sig)
{
    for (int i = 0; i < sig->num_flags; ++i) {
        delete [] sig->flags[i].name;
    }
    delete [] sig->flags;
    delete sig;
}

static void
writeValue(Trace::Writer &writer, const QVariant &var, unsigned &id)
{
    int arrayType   = QMetaType::type("ApiArray");
    int bitmaskType = QMetaType::type("ApiBitmask");
    int structType  = QMetaType::type("ApiStruct");
    int pointerType = QMetaType::type("ApiPointer");
    int enumType    = QMetaType::type("ApiEnum");
    int type = var.userType();

    switch(type) {
    case QVariant::Bool:
        writer.writeBool(var.toBool());
        break;
    case QVariant::ByteArray: {
        QByteArray ba = var.toByteArray();
        writer.writeBlob((const void*)ba.constData(), ba.size());
    }
        break;
    case QVariant::Double:
        writer.writeDouble(var.toDouble());
        break;
    case QMetaType::Float:
        writer.writeFloat(var.toFloat());
        break;
    case QVariant::Int:
        writer.writeSInt(var.toInt());
        break;
    case QVariant::LongLong:
        writer.writeSInt(var.toLongLong());
        break;
    case QVariant::String: {
        QString str = var.toString();
        writer.writeString(str.toLocal8Bit().constData(), str.length());
    }
        break;
    case QVariant::UInt:
        writer.writeUInt(var.toInt());
        break;
    case QVariant::ULongLong:
        writer.writeUInt(var.toLongLong());
        break;
    default:
        if (type == arrayType) {
            ApiArray array = var.value<ApiArray>();
            QVector<QVariant> vals = array.values();
            writer.beginArray(vals.count());
            foreach(QVariant el, vals) {
                writer.beginElement();
                writeValue(writer, el, ++id);
                writer.endElement();
            }
            writer.endArray();
        } else if (type == bitmaskType) {
            ApiBitmask bm = var.value<ApiBitmask>();
            Trace::BitmaskSig *sig = createBitmaskSig(bm, ++id);
            writer.writeBitmask(sig, bm.value());
            deleteBitmaskSig(sig);
        } else if (type == structType) {
            ApiStruct apiStr = var.value<ApiStruct>();
            QList<QVariant> vals = apiStr.values();
            Trace::StructSig *str = createStructSig(apiStr, ++id);
            writer.beginStruct(str);
            foreach(QVariant val, vals) {
                writeValue(writer, val, ++id);
            }
            writer.endStruct();
            deleteStructSig(str);
        } else if (type == pointerType) {
            ApiPointer apiPtr = var.value<ApiPointer>();
            //writer.beginArray(1);
            //writer.beginElement();
            writer.writeOpaque((const void*)apiPtr.value());
            //writer.endElement();
            //writer.endArray();
        } else if (type == enumType) {
            ApiEnum apiEnum = var.value<ApiEnum>();
            Trace::EnumSig *sig = createEnumSig(apiEnum, ++id);
            writer.writeEnum(sig);
            deleteEnumSig(sig);
        } else {
            qWarning()<<"Unsupported write variant : "
                      << QMetaType::typeName(type);
        }
    }
}
#endif

class EditVisitor : public Trace::Visitor
{
public:
    EditVisitor(const QVariant &variant)
        : m_variant(variant),
          m_editedValue(0)
    {}
    virtual void visit(Trace::Null *val)
    {
        m_editedValue = val;
    }

    virtual void visit(Trace::Bool *node)
    {
//        Q_ASSERT(m_variant.userType() == QVariant::Bool);
        bool var = m_variant.toBool();
        m_editedValue = new Trace::Bool(var);
    }

    virtual void visit(Trace::SInt *node)
    {
//        Q_ASSERT(m_variant.userType() == QVariant::Int);
        m_editedValue = new Trace::SInt(m_variant.toInt());
    }

    virtual void visit(Trace::UInt *node)
    {
//        Q_ASSERT(m_variant.userType() == QVariant::UInt);
        m_editedValue = new Trace::SInt(m_variant.toUInt());
    }

    virtual void visit(Trace::Float *node)
    {
        m_editedValue = new Trace::Float(m_variant.toFloat());
    }

    virtual void visit(Trace::String *node)
    {
        QString str = m_variant.toString();
        m_editedValue = new Trace::String(str.toLocal8Bit().constData());
    }

    virtual void visit(Trace::Enum *e)
    {
        m_editedValue = e;
    }

    virtual void visit(Trace::Bitmask *bitmask)
    {
        m_editedValue = bitmask;
    }

    virtual void visit(Trace::Struct *str)
    {
        m_editedValue = str;
    }

    virtual void visit(Trace::Array *array)
    {
        ApiArray apiArray = m_variant.value<ApiArray>();
        QVector<QVariant> vals = apiArray.values();

        Trace::Array *newArray = new Trace::Array(vals.count());
        for (int i = 0; i < vals.count(); ++i) {
            EditVisitor visitor(vals[i]);

            array->values[i]->visit(visitor);
            if (array->values[i] == visitor.value()) {
                //non-editabled
                delete newArray;
                m_editedValue = array;
                return;
            }

            newArray->values.push_back(visitor.value());
        }
        m_editedValue = newArray;
    }

    virtual void visit(Trace::Blob *blob)
    {
        m_editedValue = blob;
    }

    virtual void visit(Trace::Pointer *ptr)
    {
        m_editedValue = ptr;
    }

    Trace::Value *value() const
    {
        return m_editedValue;
    }
private:
    QVariant m_variant;
    Trace::Value *m_editedValue;
};

static void
overwriteValue(Trace::Call *call, const QVariant &val, int index)
{
    EditVisitor visitor(val);
    Trace::Value *origValue = call->args[index];
    origValue->visit(visitor);

    if (visitor.value() && origValue != visitor.value()) {
        delete origValue;
        call->args[index] = visitor.value();
    }
}

SaverThread::SaverThread(QObject *parent)
    : QThread(parent)
{
}

void SaverThread::saveFile(const QString &writeFileName,
                           const QString &readFileName,
                           const QSet<ApiTraceCall*> &editedCalls)
{
    m_writeFileName = writeFileName;
    m_readFileName = readFileName;
    m_editedCalls = editedCalls;
    start();
}

void SaverThread::run()
{
    qDebug() << "Saving  " << m_readFileName
             << ", to " << m_writeFileName;
    QMap<int, ApiTraceCall*> callIndexMap;

    foreach(ApiTraceCall *call, m_editedCalls) {
        callIndexMap.insert(call->index(), call);
    }

    Trace::Writer writer;
    writer.open(m_writeFileName.toLocal8Bit());

    Trace::Parser parser;
    parser.open(m_readFileName.toLocal8Bit());

    Trace::Call *call;
    while ((call = parser.parse_call())) {
        if (callIndexMap.contains(call->no)) {
            QVector<QVariant> values = callIndexMap[call->no]->editedValues();
            for (int i = 0; i < values.count(); ++i) {
                const QVariant &val = values[i];
                overwriteValue(call, val, i);
            }
            writer.writeCall(call);
        } else {
            writer.writeCall(call);
        }
    }

    writer.close();

    emit traceSaved();
}

#include "saverthread.moc"
