#include "apitracecall.h"

#include "trace_model.hpp"

#include <QDebug>
#include <QObject>

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
    if (variant.userType() == QVariant::ByteArray) {
        if (variant.toByteArray().size() < 1024) {
            int bytes = variant.toByteArray().size();
            return QObject::tr("[binary data, size = %1 bytes]").arg(bytes);
        } else {
            float kb = variant.toByteArray().size()/1024.;
            return QObject::tr("[binary data, size = %1 kb]").arg(kb);
        }
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

QStaticText ApiTraceCall::staticText() const
{
    if (!m_staticText.text().isEmpty())
        return m_staticText;

    QString richText = QString::fromLatin1("<span style=\"font-weight:bold\">%1</span>(").arg(name);
    for (int i = 0; i < argNames.count(); ++i) {
        richText += QLatin1String("<span style=\"color:#0000ff\">");
        QString argText = apiVariantToString(argValues[i]);

        //if arguments are really long (e.g. shader text), cut them
        // and elide it
        if (argText.length() > 40) {
            QString shortened = argText.mid(0, 40);
            shortened[argText.length() - 5] = '.';
            shortened[argText.length() - 4] = '.';
            shortened[argText.length() - 3] = '.';
            shortened[argText.length() - 2] = argText[argText.length() - 2];
            shortened[argText.length() - 1] = argText[argText.length() - 1];
            richText += shortened;
        } else {
            richText += argText;
        }
        richText += QLatin1String("</span>");
        if (i < argNames.count() - 1)
            richText += QString::fromLatin1(", ");
    }
    richText += QLatin1String(")");
    if (returnValue.isValid()) {
        richText += QLatin1String(" = ");
        richText += QLatin1String("<span style=\"color:#0000ff\">");
        richText += apiVariantToString(returnValue);
        richText += QLatin1String("</span>");
    }

    m_staticText.setText(richText);
    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    m_staticText.setTextOption(opt);
    m_staticText.prepare();

    return m_staticText;
}

QString ApiTraceCall::toHtml() const
{
    if (!m_richText.isEmpty())
        return m_richText;

    if (helpUrl.isEmpty()) {
        m_richText = QString::fromLatin1(
            "%1) <span style=\"font-weight:bold\">%2</span>(")
                     .arg(index)
                     .arg(name);
    } else {
        m_richText = QString::fromLatin1(
            "%1) <span style=\"font-weight:bold\"><a href=\"%2\">%3</a></span>(")
                      .arg(index)
                      .arg(helpUrl.toString())
                      .arg(name);
    }

    for (int i = 0; i < argNames.count(); ++i) {
        m_richText += argNames[i];
        m_richText += QString::fromLatin1(" = ");
        m_richText += QLatin1String("<span style=\"color:#0000ff\">");
        m_richText += apiVariantToString(argValues[i]);
        m_richText += QLatin1String("</span>");
        if (i < argNames.count() - 1)
            m_richText += QString::fromLatin1(", ");
    }
    m_richText += QLatin1String(")");

    if (returnValue.isValid()) {
        m_richText += QLatin1String(" = ");
        m_richText += QLatin1String("<span style=\"color:#0000ff\">");
        m_richText += apiVariantToString(returnValue);
        m_richText += QLatin1String("</span>");
    }
    return m_richText;
}

QString ApiTraceCall::filterText() const
{
    if (!m_filterText.isEmpty())
        return m_filterText;

    m_filterText = name;
    for (int i = 0; i < argNames.count(); ++i) {
        m_filterText += argNames[i];
        m_filterText += QString::fromLatin1(" = ");

        if (argValues[i].type() == QVariant::ByteArray) {
            m_hasBinaryData = true;
            m_binaryDataIndex = i;
        }
        m_filterText += apiVariantToString(argValues[i]);
        if (i < argNames.count() - 1)
            m_filterText += QString::fromLatin1(", ");
    }
    m_filterText += QLatin1String(")");

    if (returnValue.isValid()) {
        m_filterText += QLatin1String(" = ");
        m_filterText += apiVariantToString(returnValue);
    }
    return m_filterText;
}

QStaticText ApiTraceFrame::staticText() const
{
    if (!m_staticText.text().isEmpty())
        return m_staticText;

    QString richText =
        QString::fromLatin1("<span style=\"font-weight:bold\">Frame %1</span>").arg(number);

    m_staticText.setText(richText);
    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    m_staticText.setTextOption(opt);
    m_staticText.prepare();

    return m_staticText;
}

int ApiTraceCall::numChildren() const
{
    return 0;
}

int ApiTraceFrame::numChildren() const
{
    return calls.count();
}

ApiTraceFrame::ApiTraceFrame()
    : ApiTraceEvent(ApiTraceEvent::Frame)
{
}

ApiTraceCall::ApiTraceCall()
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_hasBinaryData(false),
      m_binaryDataIndex(0)
{
}

ApiTraceEvent::ApiTraceEvent()
    : m_type(ApiTraceEvent::None)
{
}

ApiTraceEvent::ApiTraceEvent(Type t)
    : m_type(t)
{
}

ApiTraceCall::~ApiTraceCall()
{
}

QVariantMap ApiTraceEvent::stateParameters() const
{
    return m_state.parameters();
}

ApiTraceState ApiTraceEvent::state() const
{
    return m_state;
}

void ApiTraceEvent::setState(const ApiTraceState &state)
{
    m_state = state;
}

bool ApiTraceCall::hasBinaryData() const
{
    return m_hasBinaryData;
}

int ApiTraceCall::binaryDataIndex() const
{
    return m_binaryDataIndex;
}

ApiTraceState::ApiTraceState()
{
}

ApiTraceState::ApiTraceState(const QVariantMap &parsedJson)
{
    m_parameters = parsedJson[QLatin1String("parameters")].toMap();
    QVariantMap currentProgram =
        parsedJson[QLatin1String("current_program")].toMap();
    QVariantList attachedShaders =
        currentProgram[QLatin1String("attached_shaders")].toList();


    for (int i = 0; i < attachedShaders.count(); ++i) {
        QVariantMap var = attachedShaders[i].toMap();
        m_shaderSources.append(
            var[QLatin1String("source")].toString());
    }

    QVariantList textureUnits =
        parsedJson[QLatin1String("textures")].toList();
    for (int i = 0; i < textureUnits.count(); ++i) {
        QVariantMap unit = textureUnits[i].toMap();
        QVariantMap::const_iterator itr;
        for (itr = unit.constBegin(); itr != unit.constEnd(); ++itr) {
            QVariantMap target = itr.value().toMap();
            if (target.count()) {
                QVariantList levels = target[QLatin1String("levels")].toList();
                for (int j = 0; j < levels.count(); ++j) {
                    QVariantMap level = levels[j].toMap();
                    QVariantMap image = level[QLatin1String("image")].toMap();
                    QSize size(image[QLatin1String("__width__")].toInt(),
                               image[QLatin1String("__height__")].toInt());
                    QString cls = image[QLatin1String("__class__")].toString();
                    QString type = image[QLatin1String("__type__")].toString();
                    bool normalized =
                        image[QLatin1String("__normalized__")].toBool();
                    int numChannels =
                        image[QLatin1String("__channels__")].toInt();

                    Q_ASSERT(numChannels == 4);
                    Q_ASSERT(type == QLatin1String("uint8"));
                    Q_ASSERT(normalized == true);

                    QByteArray dataArray =
                        image[QLatin1String("__data__")].toByteArray();

                    ApiTexture tex;
                    tex.setSize(size);
                    tex.setLevel(j);
                    tex.setUnit(i);
                    tex.setTarget(itr.key());
                    tex.contentsFromBase64(dataArray);

                    m_textures.append(tex);
                }
            }
        }
    }

    QVariantMap fbos =
        parsedJson[QLatin1String("framebuffer")].toMap();
    QVariantMap::const_iterator itr;
    for (itr = fbos.constBegin(); itr != fbos.constEnd(); ++itr) {
        QVariantMap buffer = itr.value().toMap();
        QSize size(buffer[QLatin1String("__width__")].toInt(),
                   buffer[QLatin1String("__height__")].toInt());
        QString cls = buffer[QLatin1String("__class__")].toString();
        QString type = buffer[QLatin1String("__type__")].toString();
        bool normalized = buffer[QLatin1String("__normalized__")].toBool();
        int numChannels = buffer[QLatin1String("__channels__")].toInt();

        Q_ASSERT(numChannels == 4);
        Q_ASSERT(type == QLatin1String("uint8"));
        Q_ASSERT(normalized == true);

        QByteArray dataArray =
            buffer[QLatin1String("__data__")].toByteArray();

        ApiFramebuffer fbo;
        fbo.setSize(size);
        fbo.setType(itr.key());
        fbo.contentsFromBase64(dataArray);
        m_framebuffers.append(fbo);
    }
}

QVariantMap ApiTraceState::parameters() const
{
    return m_parameters;
}

QStringList ApiTraceState::shaderSources() const
{
    return m_shaderSources;
}

bool ApiTraceState::isEmpty() const
{
    return m_parameters.isEmpty();
}

QList<ApiTexture> ApiTraceState::textures() const
{
    return m_textures;
}

QList<ApiFramebuffer> ApiTraceState::framebuffers() const
{
    return m_framebuffers;
}


