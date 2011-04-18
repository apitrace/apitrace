#include "saverthread.h"

#include "trace_write.hpp"

#include <QFile>
#include <QHash>
#include <QUrl>

#include <QDebug>


static Trace::FunctionSig *
createFunctionSig(ApiTraceCall *call, unsigned id)
{
    Trace::FunctionSig *sig = new Trace::FunctionSig();

    sig->id = id;
    sig->name = qstrdup(call->name().toLocal8Bit());

    QStringList args = call->argNames();
    sig->num_args = args.count();
    sig->args = new const char*[args.count()];
    for (int i = 0; i < args.count(); ++i) {
        sig->args[i] = qstrdup(args[i].toLocal8Bit());
    }

    return sig;
}

static void
deleteFunctionSig(Trace::FunctionSig *sig)
{
    for (int i = 0; i < sig->num_args; ++i) {
        delete [] sig->args[i];
    }
    delete [] sig->args;
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
    char **members = new char*[aSig.memberNames.count()];
    sig->members = (const char **)members;
    for (int i = 0; i < aSig.memberNames.count(); ++i) {
        members[i] = qstrdup(aSig.memberNames[i].toLocal8Bit());
    }
    return sig;
}

static void
deleteStructSig(Trace::StructSig *sig)
{
    for (int i = 0; i < sig->num_members; ++i) {
        delete [] sig->members[i];
    }
    delete [] sig->members;
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
    Trace::BitmaskVal *values = new Trace::BitmaskVal[bsig.count()];

    sig->id = id;
    sig->count = bsig.count();
    sig->values = values;

    int i = 0;
    for (itr = bsig.constBegin(); itr != bsig.constEnd(); ++itr, ++i) {
        values[i].name = qstrdup(itr->first.toLocal8Bit());
        values[i].value = itr->second;
    }

    return sig;
}

static void
deleteBitmaskSig(Trace::BitmaskSig *sig)
{
    for (int i = 0; i < sig->count; ++i) {
        delete [] sig->values[i].name;
    }
    delete [] sig->values;
    delete sig;
}

static void
writeValue(const QVariant &var, unsigned &id)
{
    int arrayType   = QMetaType::type("ApiArray");
    int bitmaskType = QMetaType::type("ApiBitmask");
    int structType  = QMetaType::type("ApiStruct");
    int pointerType = QMetaType::type("ApiPointer");
    int enumType    = QMetaType::type("ApiEnum");
    int type = var.userType();

    switch(type) {
    case QVariant::Bool:
        Trace::LiteralBool(var.toBool());
        break;
    case QVariant::ByteArray: {
        QByteArray ba = var.toByteArray();
        Trace::LiteralBlob((const void*)ba.constData(), ba.size());
    }
        break;
    case QVariant::Double:
        Trace::LiteralFloat(var.toDouble());
        break;
    case QMetaType::Float:
        Trace::LiteralFloat(var.toFloat());
        break;
    case QVariant::Int:
        Trace::LiteralSInt(var.toInt());
        break;
    case QVariant::LongLong:
        Trace::LiteralSInt(var.toLongLong());
        break;
    case QVariant::String: {
        QString str = var.toString();
        Trace::LiteralString(str.toLocal8Bit().constData(), str.length());
    }
        break;
    case QVariant::UInt:
        Trace::LiteralUInt(var.toInt());
        break;
    case QVariant::ULongLong:
        Trace::LiteralUInt(var.toLongLong());
        break;
    default:
        if (type == arrayType) {
            ApiArray array = var.value<ApiArray>();
            QList<QVariant> vals = array.values();
            Trace::BeginArray(vals.count());
            foreach(QVariant el, vals) {
                Trace::BeginElement();
                writeValue(el, ++id);
                Trace::EndElement();
            }
            Trace::EndArray();
        } else if (type == bitmaskType) {
            ApiBitmask bm = var.value<ApiBitmask>();
            Trace::BitmaskSig *sig = createBitmaskSig(bm, ++id);
            LiteralBitmask(*sig, bm.value());
            deleteBitmaskSig(sig);
        } else if (type == structType) {
            ApiStruct apiStr = var.value<ApiStruct>();
            QList<QVariant> vals = apiStr.values();
            Trace::StructSig *str = createStructSig(apiStr, ++id);
            Trace::BeginStruct(str);
            foreach(QVariant val, vals) {
                writeValue(val, ++id);
            }
            Trace::EndStruct();
            deleteStructSig(str);
        } else if (type == pointerType) {
            ApiPointer apiPtr = var.value<ApiPointer>();
            //Trace::BeginArray(1);
            //Trace::BeginElement();
            Trace::LiteralOpaque((const void*)apiPtr.value());
            //Trace::EndElement();
            //Trace::EndArray();
        } else if (type == enumType) {
            ApiEnum apiEnum = var.value<ApiEnum>();
            Trace::EnumSig *sig = createEnumSig(apiEnum, ++id);
            Trace::LiteralEnum(sig);
            deleteEnumSig(sig);
        } else {
            qWarning()<<"Unsupported write variant : "
                      << QMetaType::typeName(type);
        }
    }
}

SaverThread::SaverThread(QObject *parent)
    : QThread(parent)
{
}

void SaverThread::saveFile(const QString &fileName,
                           const QList<ApiTraceCall*> &calls)
{
    m_fileName = fileName;
    m_calls = calls;
    start();
}

void SaverThread::run()
{
    qputenv("TRACE_PATH", m_fileName.toLocal8Bit());
    unsigned id = 0;

    Trace::Open();
    for (int i = 0; i < m_calls.count(); ++i) {
        ApiTraceCall *call = m_calls[i];
        Trace::FunctionSig *funcSig = createFunctionSig(call, ++id);
        unsigned callNo = Trace::BeginEnter(*funcSig);
        {
            //args
            QVariantList vars = call->arguments();
            int index = 0;
            foreach(QVariant var, vars) {
                Trace::BeginArg(index++);
                writeValue(var, ++id);
                Trace::EndArg();
            }
        }
        Trace::EndEnter();
        Trace::BeginLeave(callNo);
        {
            QVariant ret = call->returnValue();
            if (!ret.isNull()) {
                Trace::BeginReturn();
                writeValue(ret, ++id);
                Trace::EndReturn();
            }
        }
        Trace::EndLeave();

        deleteFunctionSig(funcSig);
    }
    Trace::Close();

    emit traceSaved();
}

#include "saverthread.moc"
