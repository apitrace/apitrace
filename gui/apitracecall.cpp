#include "apitracecall.h"

#include "trace_model.hpp"

#include <QDebug>

ApiPointer::ApiPointer(int val)
    : m_value(val)
{
}

QString ApiPointer::toString() const
{
    if (m_value)
        return QString("0x%1").arg(m_value, 0, 16);
    else
        return QLatin1String("NULL");
}

QString apiVariantToString(const QVariant &variant)
{
    if (variant.userType() == QVariant::Double) {
        return QString::number(variant.toFloat());
    }

    if (variant.userType() < QVariant::UserType) {
        return variant.toString();
    }

    if (variant.canConvert<ApiPointer>()) {
        return variant.value<ApiPointer>().toString();
    }
    if (variant.canConvert<ApiBitmask>()) {
        return variant.value<ApiBitmask>().toString();
    }
    if (variant.canConvert<ApiStruct>()) {
        return variant.value<ApiStruct>().toString();
    }
    if (variant.canConvert<ApiArray>()) {
        return variant.value<ApiArray>().toString();
    }

    return QString();
}

ApiBitmask::ApiBitmask(const Trace::Bitmask *bitmask)
    : m_value(0)
{
    init(bitmask);
}


void ApiBitmask::init(const Trace::Bitmask *bitmask)
{
    if (!bitmask)
        return;

    m_value = bitmask->value;
    for (Trace::Bitmask::Signature::const_iterator it = bitmask->sig->begin();
         it != bitmask->sig->end(); ++it) {
        assert(it->second);
        QPair<QString, unsigned long long> pair;

        pair.first = QString::fromStdString(it->first);
        pair.second = it->second;

        m_sig.append(pair);
    }
}

QString ApiBitmask::toString() const
{
    QString str;
    unsigned long long value = m_value;
    bool first = true;
    for (Signature::const_iterator it = m_sig.begin();
         value != 0 && it != m_sig.end(); ++it) {
        Q_ASSERT(it->second);
        if ((value & it->second) == it->second) {
            if (!first) {
                str += QLatin1String(" | ");
            }
            str += it->first;
            value &= ~it->second;
            first = false;
        }
    }
    if (value || first) {
        if (!first) {
            str += QLatin1String(" | ");
        }
        str += QString::fromLatin1("0x%1").arg(value, 0, 16);
    }
    return str;
}

ApiStruct::ApiStruct(const Trace::Struct *s)
{
    init(s);
}

QString ApiStruct::toString() const
{
    QString str;

    str += QLatin1String("{");
    for (unsigned i = 0; i < m_members.count(); ++i) {
        str += m_sig.memberNames[i];
        str += QLatin1String(" = ");
        str += apiVariantToString(m_members[i]);
        if (i < m_members.count() - 1)
            str += QLatin1String(", ");
    }
    str += QLatin1String("}");

    return str;
}

void ApiStruct::init(const Trace::Struct *s)
{
    if (!s)
        return;

    m_sig.name = QString::fromStdString(s->sig->name);
    for (unsigned i = 0; i < s->members.size(); ++i) {
        VariantVisitor vis;
        m_sig.memberNames.append(
            QString::fromStdString(s->sig->member_names[i]));
        s->members[i]->visit(vis);
        m_members.append(vis.variant());
    }
}

void VariantVisitor::visit(Trace::Null *)
{
    m_variant = QVariant(QLatin1String("NULL"));
}

void VariantVisitor::visit(Trace::Bool *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(Trace::SInt *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(Trace::UInt *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(Trace::Float *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(Trace::String *node)
{
    m_variant = QVariant(QString::fromStdString(node->value));
}

void VariantVisitor::visit(Trace::Enum *e)
{
    m_variant = QVariant(QString::fromStdString(e->sig->first));
}

void VariantVisitor::visit(Trace::Bitmask *bitmask)
{
    m_variant = QVariant::fromValue(ApiBitmask(bitmask));
}

void VariantVisitor::visit(Trace::Struct *str)
{
    m_variant = QVariant::fromValue(ApiStruct(str));
}

void VariantVisitor::visit(Trace::Array *array)
{
    m_variant = QVariant::fromValue(ApiArray(array));
}

void VariantVisitor::visit(Trace::Blob *blob)
{
    QByteArray barray = QByteArray::fromRawData(blob->buf, blob->size);
    m_variant = QVariant(barray);
}

void VariantVisitor::visit(Trace::Pointer *ptr)
{
    m_variant = QVariant::fromValue(ApiPointer(ptr->value));
}

ApiArray::ApiArray(const Trace::Array *arr)
{
    init(arr);
}

QString ApiArray::toString() const
{
    QString str;
    str += QLatin1String("[");
    for(int i = 0; i < m_array.count(); ++i) {
        const QVariant &var = m_array[i];
        str += apiVariantToString(var);
        if (i < m_array.count() - 1)
            str += QLatin1String(", ");
    }
    str += QLatin1String("]");

    return str;
}

void ApiArray::init(const Trace::Array *arr)
{
    if (!arr)
        return;

    for (int i = 0; i < arr->values.size(); ++i) {
        VariantVisitor vis;
        arr->values[i]->visit(vis);

        m_array.append(vis.variant());
    }
}
