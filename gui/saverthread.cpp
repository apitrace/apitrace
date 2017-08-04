#include "saverthread.h"

#include "trace_writer.hpp"
#include "trace_model.hpp"
#include "trace_parser.hpp"

#include <QFile>
#include <QHash>
#include <QUrl>

#include <QDebug>

#if 0
static trace::FunctionSig *
createFunctionSig(ApiTraceCall *call, unsigned id)
{
    trace::FunctionSig *sig = new trace::FunctionSig();

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
deleteFunctionSig(trace::FunctionSig *sig)
{
    for (int i = 0; i < sig->num_args; ++i) {
        delete [] sig->arg_names[i];
    }
    delete [] sig->arg_names;
    delete [] sig->name;
    delete sig;
}

static trace::StructSig *
createStructSig(const ApiStruct &str, unsigned id)
{
    ApiStruct::Signature aSig = str.signature();

    trace::StructSig *sig = new trace::StructSig();
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
deleteStructSig(trace::StructSig *sig)
{
    for (int i = 0; i < sig->num_members; ++i) {
        delete [] sig->member_names[i];
    }
    delete [] sig->member_names;
    delete [] sig->name;
    delete sig;
}

static trace::EnumSig *
createEnumSig(const ApiEnum &en, unsigned id)
{
    trace::EnumSig *sig = new trace::EnumSig();

    sig->id = id;
    sig->name = qstrdup(en.name().toLocal8Bit());
    sig->value = en.value().toLongLong();

    return sig;
}

static void
deleteEnumSig(trace::EnumSig *sig)
{
    delete [] sig->name;
    delete sig;
}

static trace::BitmaskSig *
createBitmaskSig(const ApiBitmask &bt, unsigned id)
{
    ApiBitmask::Signature bsig = bt.signature();
    ApiBitmask::Signature::const_iterator itr;

    trace::BitmaskSig *sig = new trace::BitmaskSig();
    trace::BitmaskFlag *flags = new trace::BitmaskFlag[bsig.count()];

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
deleteBitmaskSig(trace::BitmaskSig *sig)
{
    for (int i = 0; i < sig->num_flags; ++i) {
        delete [] sig->flags[i].name;
    }
    delete [] sig->flags;
    delete sig;
}

static void
writeValue(trace::Writer &writer, const QVariant &var, unsigned &id)
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
            trace::BitmaskSig *sig = createBitmaskSig(bm, ++id);
            writer.writeBitmask(sig, bm.value());
            deleteBitmaskSig(sig);
        } else if (type == structType) {
            ApiStruct apiStr = var.value<ApiStruct>();
            QList<QVariant> vals = apiStr.values();
            trace::StructSig *str = createStructSig(apiStr, ++id);
            writer.beginStruct(str);
            foreach(QVariant val, vals) {
                writeValue(writer, val, ++id);
            }
            writer.endStruct();
            deleteStructSig(str);
        } else if (type == pointerType) {
            ApiPointer apiPtr = var.value<ApiPointer>();
            writer.writePointer(apiPtr.value());
        } else if (type == enumType) {
            ApiEnum apiEnum = var.value<ApiEnum>();
            trace::EnumSig *sig = createEnumSig(apiEnum, ++id);
            writer.writeEnum(sig);
            deleteEnumSig(sig);
        } else {
            qWarning()<<"Unsupported write variant : "
                      << QMetaType::typeName(type);
        }
    }
}
#endif

class EditVisitor : public trace::Visitor
{
public:
    EditVisitor(const QVariant &variant)
        : m_variant(variant),
          m_editedValue(0)
    {}
    virtual void visit(trace::Null *val) override
    {
        m_editedValue = val;
    }

    virtual void visit(trace::Bool *node) override
    {
//        Q_ASSERT(m_variant.userType() == QVariant::Bool);
        bool var = m_variant.toBool();
        m_editedValue = new trace::Bool(var);
    }

    virtual void visit(trace::SInt *node) override
    {
//        Q_ASSERT(m_variant.userType() == QVariant::Int);
        m_editedValue = new trace::SInt(m_variant.toInt());
    }

    virtual void visit(trace::UInt *node) override
    {
//        Q_ASSERT(m_variant.userType() == QVariant::UInt);
        m_editedValue = new trace::SInt(m_variant.toUInt());
    }

    virtual void visit(trace::Float *node) override
    {
        m_editedValue = new trace::Float(m_variant.toFloat());
    }

    virtual void visit(trace::Double *node) override
    {
        m_editedValue = new trace::Double(m_variant.toDouble());
    }

    virtual void visit(trace::String *node) override
    {
        QString str = m_variant.toString();
        char *newString = new char[str.length() + 1];
        QByteArray ba = str.toLocal8Bit();
        strcpy(newString, ba.constData());
        m_editedValue = new trace::String(newString);
    }

    virtual void visit(trace::WString *node) override
    {
        QString str = m_variant.toString();
        size_t len = str.length();
        wchar_t *newString = new wchar_t[len + 1];
        str.toWCharArray(newString);
        newString[len] = 0;
        m_editedValue = new trace::WString(newString);
    }

    virtual void visit(trace::Enum *e) override
    {
        m_editedValue = e;
    }

    virtual void visit(trace::Bitmask *bitmask) override
    {
        m_editedValue = bitmask;
    }

    virtual void visit(trace::Struct *str) override
    {
        m_editedValue = str;
    }

    virtual void visit(trace::Array *array) override
    {
        ApiArray apiArray = m_variant.value<ApiArray>();
        QVector<QVariant> vals = apiArray.values();

        trace::Array *newArray = new trace::Array(vals.count());
        for (int i = 0; i < vals.count(); ++i) {
            EditVisitor visitor(vals[i]);
            array->values[i]->visit(visitor);
            if (array->values[i] == visitor.value()) {
                //non-editabled
                delete newArray;
                m_editedValue = array;
                return;
            }

            newArray->values[i] = visitor.value();
        }
        m_editedValue = newArray;
    }

    virtual void visit(trace::Blob *blob) override
    {
        m_editedValue = blob;
    }

    virtual void visit(trace::Pointer *ptr) override
    {
        m_editedValue = ptr;
    }

    trace::Value *value() const
    {
        return m_editedValue;
    }
private:
    QVariant m_variant;
    trace::Value *m_editedValue;
};

static void
overwriteValue(trace::Call *call, const QVariant &val, int index)
{
    EditVisitor visitor(val);
    trace::Value *origValue = call->args[index].value;
    origValue->visit(visitor);

    if (visitor.value() && origValue != visitor.value()) {
        delete origValue;
        call->args[index].value = visitor.value();
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

    trace::Parser parser;
    parser.open(m_readFileName.toLocal8Bit());

    trace::Writer writer;
    writer.open(m_writeFileName.toLocal8Bit(), parser.getVersion(), parser.getProperties());

    trace::Call *call;
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
        delete call;
    }

    writer.close();

    emit traceSaved();
}

#include "saverthread.moc"
