#include "apitracecall.h"

#include "apitrace.h"
#include "traceloader.h"
#include "trace_model.hpp"

#include <QDebug>
#include <QLocale>
#include <QObject>
#define QT_USE_FAST_OPERATOR_PLUS
#include <QStringBuilder>
#include <QTextDocument>

const char * const styleSheet =
    ".call {\n"
    "    font-weight:bold;\n"
    // text shadow looks great but doesn't work well in qtwebkit 4.7
    "    /*text-shadow: 0px 2px 3px #555;*/\n"
    "    font-size: 1.1em;\n"
    "}\n"
    ".arg-name {\n"
    "    border: 1px solid rgb(238,206,0);\n"
    "    border-radius: 4px;\n"
    "    background: yellow;\n"
    "    padding: 2px;\n"
    "    box-shadow: 0px 1px 3px dimgrey;\n"
    "    -webkit-transition: background 1s linear;\n"
    "}\n"
    ".arg-name:hover {\n"
    "    background: white;\n"
    "}\n"
    ".arg-value {\n"
    "    color: #0000ff;\n"
    "}\n"
    ".error {\n"
    "    border: 1px solid rgb(255,0,0);\n"
    "    margin: 10px;\n"
    "    padding: 1;\n"
    "    border-radius: 4px;\n"
    // also looks great but qtwebkit doesn't support it
    //"    background: #6fb2e5;\n"
    //"    box-shadow: 0 1px 5px #0061aa, inset 0 10px 20px #b6f9ff;\n"
    //"    -o-box-shadow: 0 1px 5px #0061aa, inset 0 10px 20px #b6f9ff;\n"
    //"    -webkit-box-shadow: 0 1px 5px #0061aa, inset 0 10px 20px #b6f9ff;\n"
    //"    -moz-box-shadow: 0 1px 5px #0061aa, inset 0 10px 20px #b6f9ff;\n"
    "}\n";


// Qt::convertFromPlainText doesn't do precisely what we want
static QString
plainTextToHTML(const QString & plain, bool multiLine)
{
    int col = 0;
    bool quote = false;
    QString rich;
    for (int i = 0; i < plain.length(); ++i) {
        if (plain[i] == QLatin1Char('\n')){
            if (multiLine) {
                rich += QLatin1String("<br>\n");
            } else {
                rich += QLatin1String("\\n");
            }
            col = 0;
            quote = true;
        } else {
            if (plain[i] == QLatin1Char('\t')){
                if (multiLine) {
                    rich += QChar(0x00a0U);
                    ++col;
                    while (col % 8) {
                        rich += QChar(0x00a0U);
                        ++col;
                    }
                } else {
                    rich += QLatin1String("\\t");
                }
                quote = true;
            } else if (plain[i].isSpace()) {
                rich += QChar(0x00a0U);
                quote = true;
            } else if (plain[i] == QLatin1Char('<')) {
                rich += QLatin1String("&lt;");
            } else if (plain[i] == QLatin1Char('>')) {
                rich += QLatin1String("&gt;");
            } else if (plain[i] == QLatin1Char('&')) {
                rich += QLatin1String("&amp;");
            } else {
                rich += plain[i];
            }
            ++col;
        }
    }

    if (quote) {
        return QLatin1Literal("\"") + rich + QLatin1Literal("\"");
    }

    return rich;
}

QString
apiVariantToString(const QVariant &variant, bool multiLine)
{
    if (variant.isNull()) {
        return QLatin1String("?");
    }

    if (variant.userType() == QMetaType::Float) {
        return QString::number(variant.toFloat());
    }
    if (variant.userType() == QVariant::Double) {
        return QString::number(variant.toDouble());
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

    if (variant.userType() == QVariant::String) {
        return plainTextToHTML(variant.toString(), multiLine);
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
        return variant.value<ApiStruct>().toString(multiLine);
    }
    if (variant.canConvert<ApiArray>()) {
        return variant.value<ApiArray>().toString(multiLine);
    }
    if (variant.canConvert<ApiEnum>()) {
        return variant.value<ApiEnum>().toString();
    }

    return QString();
}


void VariantVisitor::visit(trace::Null *)
{
    m_variant = QVariant::fromValue(ApiPointer(0));
}

void VariantVisitor::visit(trace::Bool *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(trace::SInt *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(trace::UInt *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(trace::Float *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(trace::Double *node)
{
    m_variant = QVariant(node->value);
}

void VariantVisitor::visit(trace::String *node)
{
    m_variant = QVariant(QString::fromStdString(node->value));
}

void VariantVisitor::visit(trace::Enum *e)
{
    ApiTraceEnumSignature *sig = 0;

    if (m_loader) {
        sig = m_loader->enumSignature(e->sig->id);
    }
    if (!sig) {
        sig = new ApiTraceEnumSignature(e->sig);
        if (m_loader) {
            m_loader->addEnumSignature(e->sig->id, sig);
        }
    }

    m_variant = QVariant::fromValue(ApiEnum(sig, e->value));
}

void VariantVisitor::visit(trace::Bitmask *bitmask)
{
    m_variant = QVariant::fromValue(ApiBitmask(bitmask));
}

void VariantVisitor::visit(trace::Struct *str)
{
    m_variant = QVariant::fromValue(ApiStruct(str));
}

void VariantVisitor::visit(trace::Array *array)
{
    m_variant = QVariant::fromValue(ApiArray(array));
}

void VariantVisitor::visit(trace::Blob *blob)
{
    QByteArray barray = QByteArray(blob->buf, blob->size);
    m_variant = QVariant(barray);
}

void VariantVisitor::visit(trace::Pointer *ptr)
{
    m_variant = QVariant::fromValue(ApiPointer(ptr->value));
}

ApiTraceEnumSignature::ApiTraceEnumSignature(const trace::EnumSig *sig)
{
    for (const trace::EnumValue *it = sig->values;
         it != sig->values + sig->num_values; ++it) {
        QPair<QString, signed long long> pair;

        pair.first = QString::fromStdString(it->name);
        pair.second = it->value;

        m_names.append(pair);
    }
}

QString ApiTraceEnumSignature::name(signed long long value) const
{
    for (ValueList::const_iterator it = m_names.begin();
         it != m_names.end(); ++it) {
        if (value == it->second) {
            return it->first;
        }
    }
    return QString::fromLatin1("%1").arg(value);
}

ApiEnum::ApiEnum(ApiTraceEnumSignature *sig, signed long long value)
    : m_sig(sig), m_value(value)
{
}

QString ApiEnum::toString() const
{
    if (m_sig) {
        return m_sig->name(m_value);
    }
    Q_ASSERT(!"should never happen");
    return QString();
}

QVariant ApiEnum::value() const
{
    if (m_sig) {
        return QVariant::fromValue(m_value);
    }
    Q_ASSERT(!"should never happen");
    return QVariant();
}

QString ApiEnum::name() const
{
    if (m_sig) {
        return m_sig->name(m_value);
    }
    Q_ASSERT(!"should never happen");
    return QString();
}

unsigned long long ApiBitmask::value() const
{
    return m_value;
}

ApiBitmask::Signature ApiBitmask::signature() const
{
    return m_sig;
}

ApiStruct::Signature ApiStruct::signature() const
{
    return m_sig;
}

QList<QVariant> ApiStruct::values() const
{
    return m_members;
}

ApiPointer::ApiPointer(unsigned long long val)
    : m_value(val)
{
}


unsigned long long ApiPointer::value() const
{
    return m_value;
}

QString ApiPointer::toString() const
{
    if (m_value)
        return QString("0x%1").arg(m_value, 0, 16);
    else
        return QLatin1String("NULL");
}

ApiBitmask::ApiBitmask(const trace::Bitmask *bitmask)
    : m_value(0)
{
    init(bitmask);
}

void ApiBitmask::init(const trace::Bitmask *bitmask)
{
    if (!bitmask)
        return;

    m_value = bitmask->value;
    for (const trace::BitmaskFlag *it = bitmask->sig->flags;
         it != bitmask->sig->flags + bitmask->sig->num_flags; ++it) {
        assert(it->value);
        QPair<QString, unsigned long long> pair;

        pair.first = QString::fromStdString(it->name);
        pair.second = it->value;

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

ApiStruct::ApiStruct(const trace::Struct *s)
{
    init(s);
}

QString ApiStruct::toString(bool multiLine) const
{
    QString str;

    str += QLatin1String("{");
    for (unsigned i = 0; i < m_members.count(); ++i) {
        str += m_sig.memberNames[i] %
               QLatin1Literal(" = ") %
               apiVariantToString(m_members[i], multiLine);
        if (i < m_members.count() - 1)
            str += QLatin1String(", ");
    }
    str += QLatin1String("}");

    return str;
}

void ApiStruct::init(const trace::Struct *s)
{
    if (!s)
        return;

    m_sig.name = QString::fromStdString(s->sig->name);
    for (unsigned i = 0; i < s->sig->num_members; ++i) {
        VariantVisitor vis(0);
        m_sig.memberNames.append(
            QString::fromStdString(s->sig->member_names[i]));
        s->members[i]->visit(vis);
        m_members.append(vis.variant());
    }
}

ApiArray::ApiArray(const trace::Array *arr)
{
    init(arr);
}

ApiArray::ApiArray(const QVector<QVariant> &vals)
    : m_array(vals)
{
}

QVector<QVariant> ApiArray::values() const
{
    return m_array;
}

QString ApiArray::toString(bool multiLine) const
{
    QString str;
    str += QLatin1String("[");
    for(int i = 0; i < m_array.count(); ++i) {
        const QVariant &var = m_array[i];
        str += apiVariantToString(var, multiLine);
        if (i < m_array.count() - 1)
            str += QLatin1String(", ");
    }
    str += QLatin1String("]");

    return str;
}

void ApiArray::init(const trace::Array *arr)
{
    if (!arr)
        return;

    m_array.reserve(arr->values.size());
    for (int i = 0; i < arr->values.size(); ++i) {
        VariantVisitor vis(0);
        arr->values[i]->visit(vis);

        m_array.append(vis.variant());
    }
    m_array.squeeze();
}

ApiTraceState::ApiTraceState()
{
}

ApiTraceState::ApiTraceState(const QVariantMap &parsedJson)
{
    m_parameters = parsedJson[QLatin1String("parameters")].toMap();
    QVariantMap attachedShaders =
        parsedJson[QLatin1String("shaders")].toMap();
    QVariantMap::const_iterator itr;


    for (itr = attachedShaders.constBegin(); itr != attachedShaders.constEnd();
         ++itr) {
        QString type = itr.key();
        QString source = itr.value().toString();
        m_shaderSources[type] = source;
    }

    m_uniforms = parsedJson[QLatin1String("uniforms")].toMap();

    QVariantMap textures =
        parsedJson[QLatin1String("textures")].toMap();
    for (itr = textures.constBegin(); itr != textures.constEnd(); ++itr) {
        QVariantMap image = itr.value().toMap();
        QSize size(image[QLatin1String("__width__")].toInt(),
                   image[QLatin1String("__height__")].toInt());
        QString cls = image[QLatin1String("__class__")].toString();
        QString type = image[QLatin1String("__type__")].toString();
        bool normalized =
            image[QLatin1String("__normalized__")].toBool();
        int numChannels =
            image[QLatin1String("__channels__")].toInt();
        int depth =
            image[QLatin1String("__depth__")].toInt();
        QString formatName =
            image[QLatin1String("__format__")].toString();

        Q_ASSERT(type == QLatin1String("uint8"));
        Q_ASSERT(normalized == true);
        Q_UNUSED(normalized);

        QByteArray dataArray =
            image[QLatin1String("__data__")].toByteArray();

        ApiTexture tex;
        tex.setSize(size);
        tex.setDepth(depth);
        tex.setFormatName(formatName);
        tex.setNumChannels(numChannels);
        tex.setLabel(itr.key());
        tex.contentsFromBase64(dataArray);

        m_textures.append(tex);
    }

    QVariantMap fbos =
        parsedJson[QLatin1String("framebuffer")].toMap();
    for (itr = fbos.constBegin(); itr != fbos.constEnd(); ++itr) {
        QVariantMap buffer = itr.value().toMap();
        QSize size(buffer[QLatin1String("__width__")].toInt(),
                   buffer[QLatin1String("__height__")].toInt());
        QString cls = buffer[QLatin1String("__class__")].toString();
        QString type = buffer[QLatin1String("__type__")].toString();
        bool normalized = buffer[QLatin1String("__normalized__")].toBool();
        int numChannels = buffer[QLatin1String("__channels__")].toInt();
        int depth = buffer[QLatin1String("__depth__")].toInt();
        QString formatName = buffer[QLatin1String("__format__")].toString();

        Q_ASSERT(type == QLatin1String("uint8"));
        Q_ASSERT(normalized == true);
        Q_UNUSED(normalized);

        QByteArray dataArray =
            buffer[QLatin1String("__data__")].toByteArray();

        ApiFramebuffer fbo;
        fbo.setSize(size);
        fbo.setDepth(depth);
        fbo.setFormatName(formatName);
        fbo.setNumChannels(numChannels);
        fbo.setType(itr.key());
        fbo.contentsFromBase64(dataArray);
        m_framebuffers.append(fbo);
    }
}

const QVariantMap & ApiTraceState::parameters() const
{
    return m_parameters;
}

const QMap<QString, QString> & ApiTraceState::shaderSources() const
{
    return m_shaderSources;
}

const QVariantMap & ApiTraceState::uniforms() const
{
    return m_uniforms;
}

bool ApiTraceState::isEmpty() const
{
    return m_parameters.isEmpty();
}

const QList<ApiTexture> & ApiTraceState::textures() const
{
    return m_textures;
}

const QList<ApiFramebuffer> & ApiTraceState::framebuffers() const
{
    return m_framebuffers;
}

ApiFramebuffer ApiTraceState::colorBuffer() const
{
    foreach (ApiFramebuffer fbo, m_framebuffers) {
        if (fbo.type() == QLatin1String("GL_BACK")) {
            return fbo;
        }
    }
    foreach (ApiFramebuffer fbo, m_framebuffers) {
        if (fbo.type() == QLatin1String("GL_FRONT")) {
            return fbo;
        }
    }
    return ApiFramebuffer();
}


ApiTraceCallSignature::ApiTraceCallSignature(const QString &name,
                                             const QStringList &argNames)
    : m_name(name),
      m_argNames(argNames)
{
}

ApiTraceCallSignature::~ApiTraceCallSignature()
{
}

QUrl ApiTraceCallSignature::helpUrl() const
{
    return m_helpUrl;
}

void ApiTraceCallSignature::setHelpUrl(const QUrl &url)
{
    m_helpUrl = url;
}

ApiTraceEvent::ApiTraceEvent()
    : m_type(ApiTraceEvent::None),
      m_hasBinaryData(false),
      m_binaryDataIndex(0),
      m_state(0),
      m_staticText(0)
{
}

ApiTraceEvent::ApiTraceEvent(Type t)
    : m_type(t),
      m_hasBinaryData(false),
      m_binaryDataIndex(0),
      m_state(0),
      m_staticText(0)
{
}

ApiTraceEvent::~ApiTraceEvent()
{
    delete m_state;
    delete m_staticText;
}

QVariantMap ApiTraceEvent::stateParameters() const
{
    if (m_state) {
        return m_state->parameters();
    } else {
        return QVariantMap();
    }
}

ApiTraceState *ApiTraceEvent::state() const
{
    return m_state;
}

void ApiTraceEvent::setState(ApiTraceState *state)
{
    m_state = state;
}

ApiTraceCall::ApiTraceCall(ApiTraceFrame *parentFrame,
                           TraceLoader *loader,
                           const trace::Call *call)
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_parentFrame(parentFrame)
{
    m_index = call->no;

    m_signature = loader->signature(call->sig->id);

    if (!m_signature) {
        QString name = QString::fromStdString(call->sig->name);
        QStringList argNames;
        argNames.reserve(call->sig->num_args);
        for (int i = 0; i < call->sig->num_args; ++i) {
            argNames += QString::fromStdString(call->sig->arg_names[i]);
        }
        m_signature = new ApiTraceCallSignature(name, argNames);
        loader->addSignature(call->sig->id, m_signature);
    }
    if (call->ret) {
        VariantVisitor retVisitor(loader);
        call->ret->visit(retVisitor);
        m_returnValue = retVisitor.variant();
    }
    m_argValues.reserve(call->args.size());
    for (int i = 0; i < call->args.size(); ++i) {
        if (call->args[i].value) {
            VariantVisitor argVisitor(loader);
            call->args[i].value->visit(argVisitor);
            m_argValues.append(argVisitor.variant());
            if (m_argValues[i].type() == QVariant::ByteArray) {
                m_hasBinaryData = true;
                m_binaryDataIndex = i;
            }
        } else {
            m_argValues.append(QVariant());
        }
    }
    m_argValues.squeeze();
    m_flags = call->flags;
}

ApiTraceCall::~ApiTraceCall()
{
}


bool ApiTraceCall::hasError() const
{
    return !m_error.isEmpty();
}

QString ApiTraceCall::error() const
{
    return m_error;
}

void ApiTraceCall::setError(const QString &msg)
{
    if (m_error != msg) {
        m_error = msg;
        m_richText = QString();
    }
}

ApiTrace * ApiTraceCall::parentTrace() const
{
    if (m_parentFrame)
        return m_parentFrame->parentTrace();
    return NULL;
}

QVector<QVariant> ApiTraceCall::originalValues() const
{
    return m_argValues;
}

void ApiTraceCall::setEditedValues(const QVector<QVariant> &lst)
{
    ApiTrace *trace = parentTrace();

    m_editedValues = lst;
    //lets regenerate data
    m_richText = QString();
    m_searchText = QString();
    delete m_staticText;
    m_staticText = 0;

    if (trace) {
        if (!lst.isEmpty()) {
            trace->callEdited(this);
        } else {
            trace->callReverted(this);
        }
    }
}

QVector<QVariant> ApiTraceCall::editedValues() const
{
    return m_editedValues;
}

bool ApiTraceCall::edited() const
{
    return !m_editedValues.isEmpty();
}

void ApiTraceCall::revert()
{
    setEditedValues(QVector<QVariant>());
}

void ApiTraceCall::setHelpUrl(const QUrl &url)
{
    m_signature->setHelpUrl(url);
}

void ApiTraceCall::setParentFrame(ApiTraceFrame *frame)
{
    m_parentFrame = frame;
}

ApiTraceFrame * ApiTraceCall::parentFrame()const
{
    return m_parentFrame;
}

int ApiTraceCall::index() const
{
    return m_index;
}

QString ApiTraceCall::name() const
{
    return m_signature->name();
}

QStringList ApiTraceCall::argNames() const
{
    return m_signature->argNames();
}

QVector<QVariant> ApiTraceCall::arguments() const
{
    if (m_editedValues.isEmpty())
        return m_argValues;
    else
        return m_editedValues;
}

QVariant ApiTraceCall::returnValue() const
{
    return m_returnValue;
}

trace::CallFlags ApiTraceCall::flags() const
{
    return m_flags;
}

QUrl ApiTraceCall::helpUrl() const
{
    return m_signature->helpUrl();
}

bool ApiTraceCall::hasBinaryData() const
{
    return m_hasBinaryData;
}

int ApiTraceCall::binaryDataIndex() const
{
    return m_binaryDataIndex;
}

QStaticText ApiTraceCall::staticText() const
{
    if (m_staticText && !m_staticText->text().isEmpty())
        return *m_staticText;

    QVector<QVariant> argValues = arguments();

    QString richText = QString::fromLatin1(
        "<span style=\"font-weight:bold\">%1</span>(").arg(
            m_signature->name());
    QStringList argNames = m_signature->argNames();
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
            shortened[argText.length() - 2] = argText.at(argText.length() - 2);
            shortened[argText.length() - 1] = argText.at(argText.length() - 1);
            richText += shortened;
        } else {
            richText += argText;
        }
        richText += QLatin1String("</span>");
        if (i < argNames.count() - 1)
            richText += QLatin1String(", ");
    }
    richText += QLatin1String(")");
    if (m_returnValue.isValid()) {
        richText +=
            QLatin1Literal(" = ") %
            QLatin1Literal("<span style=\"color:#0000ff\">") %
            apiVariantToString(m_returnValue) %
            QLatin1Literal("</span>");
    }

    if (!m_staticText)
        m_staticText = new QStaticText(richText);
    else
        m_staticText->setText(richText);
    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    m_staticText->setTextOption(opt);
    m_staticText->prepare();

    return *m_staticText;
}

QString ApiTraceCall::toHtml() const
{
    if (!m_richText.isEmpty())
        return m_richText;

    m_richText += QLatin1String("<div class=\"call\">");


    m_richText +=
        QString::fromLatin1("%1) ")
        .arg(m_index);
    QString parentTip;
    if (m_parentFrame) {
        parentTip =
            QString::fromLatin1("Frame %1")
            .arg(m_parentFrame->number);
    }
    QUrl helpUrl = m_signature->helpUrl();
    if (helpUrl.isEmpty()) {
        m_richText += QString::fromLatin1(
            "<span class=\"callName\" title=\"%1\">%2</span>(")
                      .arg(parentTip)
                      .arg(m_signature->name());
    } else {
        m_richText += QString::fromLatin1(
         "<span class=\"callName\" title=\"%1\"><a href=\"%2\">%3</a></span>(")
                      .arg(parentTip)
                      .arg(helpUrl.toString())
                      .arg(m_signature->name());
    }

    QVector<QVariant> argValues = arguments();
    QStringList argNames = m_signature->argNames();
    for (int i = 0; i < argNames.count(); ++i) {
        m_richText +=
            QLatin1String("<span class=\"arg-name\">") +
            argNames[i] +
            QLatin1String("</span>") +
            QLatin1Literal(" = ") +
            QLatin1Literal("<span class=\"arg-value\">") +
            apiVariantToString(argValues[i], true) +
            QLatin1Literal("</span>");
        if (i < argNames.count() - 1)
            m_richText += QLatin1String(", ");
    }
    m_richText += QLatin1String(")");

    if (m_returnValue.isValid()) {
        m_richText +=
            QLatin1String(" = ") +
            QLatin1String("<span style=\"color:#0000ff\">") +
            apiVariantToString(m_returnValue, true) +
            QLatin1String("</span>");
    }
    m_richText += QLatin1String("</div>");

    if (hasError()) {
        QString errorStr =
            QString::fromLatin1(
                "<div class=\"error\">%1</div>")
            .arg(m_error);
        m_richText += errorStr;
    }

    m_richText =
        QString::fromLatin1(
            "<html><head><style type=\"text/css\" media=\"all\">"
            "%1</style></head><body>%2</body></html>")
        .arg(styleSheet)
        .arg(m_richText);
    m_richText.squeeze();

    //qDebug()<<m_richText;
    return m_richText;
}

QString ApiTraceCall::searchText() const
{
    if (!m_searchText.isEmpty())
        return m_searchText;

    QVector<QVariant> argValues = arguments();
    m_searchText = m_signature->name() + QLatin1Literal("(");
    QStringList argNames = m_signature->argNames();
    for (int i = 0; i < argNames.count(); ++i) {
        m_searchText += argNames[i] +
                        QLatin1Literal(" = ") +
                        apiVariantToString(argValues[i]);
        if (i < argNames.count() - 1)
            m_searchText += QLatin1String(", ");
    }
    m_searchText += QLatin1String(")");

    if (m_returnValue.isValid()) {
        m_searchText += QLatin1Literal(" = ") +
                        apiVariantToString(m_returnValue);
    }
    m_searchText.squeeze();
    return m_searchText;
}

int ApiTraceCall::numChildren() const
{
    return 0;
}

bool ApiTraceCall::contains(const QString &str,
                            Qt::CaseSensitivity sensitivity) const
{
    QString txt = searchText();
    return txt.contains(str, sensitivity);
}


ApiTraceFrame::ApiTraceFrame(ApiTrace *parentTrace)
    : ApiTraceEvent(ApiTraceEvent::Frame),
      m_parentTrace(parentTrace),
      m_binaryDataSize(0),
      m_loaded(false),
      m_callsToLoad(0),
      m_lastCallIndex(0)
{
}

ApiTraceFrame::~ApiTraceFrame()
{
    qDeleteAll(m_calls);
}

QStaticText ApiTraceFrame::staticText() const
{
    if (m_staticText && !m_staticText->text().isEmpty())
        return *m_staticText;

    QString richText = QObject::tr(
                "<span style=\"font-weight:bold\">Frame %1</span>"
                "&nbsp;&nbsp;&nbsp;"
                "<span style=\"font-style:italic;font-size:small;font-weight:lighter;\"> "
                "(%2 calls)</span>")
            .arg(number)
            .arg(m_loaded ? m_calls.count() : m_callsToLoad);

    //mark the frame if it uploads more than a meg a frame
    if (m_binaryDataSize > (1024*1024)) {
        richText =
            QObject::tr(
                "%1"
                "<span style=\"font-style:italic;\">"
                "&nbsp;&nbsp;&nbsp;&nbsp;(%2MB)</span>")
            .arg(richText)
            .arg(double(m_binaryDataSize / (1024.*1024.)), 0, 'g', 2);
    }

    if (!m_staticText)
        m_staticText = new QStaticText(richText);

    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    m_staticText->setTextOption(opt);
    m_staticText->prepare();

    return *m_staticText;
}

int ApiTraceFrame::numChildren() const
{
    return m_calls.count();
}

ApiTrace * ApiTraceFrame::parentTrace() const
{
    return m_parentTrace;
}

void ApiTraceFrame::addCall(ApiTraceCall *call)
{
    m_calls.append(call);
    if (call->hasBinaryData()) {
        QByteArray data =
            call->arguments()[call->binaryDataIndex()].toByteArray();
        m_binaryDataSize += data.size();
    }
}

QVector<ApiTraceCall*> ApiTraceFrame::calls() const
{
    return m_calls;
}

ApiTraceCall * ApiTraceFrame::call(int idx) const
{
    return m_calls.value(idx);
}


ApiTraceCall * ApiTraceFrame::callWithIndex(int index) const
{
    QVector<ApiTraceCall*>::const_iterator itr;
    for (itr = m_calls.constBegin(); itr != m_calls.constEnd(); ++itr) {
        if ((*itr)->index() == index) {
            return *itr;
        }
    }
    return 0;
}

int ApiTraceFrame::callIndex(ApiTraceCall *call) const
{
    return m_calls.indexOf(call);
}

bool ApiTraceFrame::isEmpty() const
{
    if (m_loaded) {
        return m_calls.isEmpty();
    } else {
        return m_callsToLoad == 0;
    }
}

int ApiTraceFrame::binaryDataSize() const
{
    return m_binaryDataSize;
}

void ApiTraceFrame::setCalls(const QVector<ApiTraceCall*> &calls,
                             quint64 binaryDataSize)
{
    m_calls = calls;
    m_binaryDataSize = binaryDataSize;
    m_loaded = true;
    delete m_staticText;
    m_staticText = 0;
}

bool ApiTraceFrame::isLoaded() const
{
    return m_loaded;
}

void ApiTraceFrame::setLoaded(bool l)
{
    m_loaded = l;
}

void ApiTraceFrame::setNumChildren(int num)
{
    m_callsToLoad = num;
}

void ApiTraceFrame::setParentTrace(ApiTrace *parent)
{
    m_parentTrace = parent;
}

int ApiTraceFrame::numChildrenToLoad() const
{
    return m_callsToLoad;
}

ApiTraceCall *
ApiTraceFrame::findNextCall(ApiTraceCall *from,
                            const QString &str,
                            Qt::CaseSensitivity sensitivity) const
{
    Q_ASSERT(m_loaded);

    int callIndex = 0;

    if (from) {
        callIndex = m_calls.indexOf(from) + 1;
    }

    for (int i = callIndex; i < m_calls.count(); ++i) {
        ApiTraceCall *call = m_calls[i];
        if (call->contains(str, sensitivity)) {
            return call;
        }
    }
    return 0;
}

ApiTraceCall *
ApiTraceFrame::findPrevCall(ApiTraceCall *from,
                            const QString &str,
                            Qt::CaseSensitivity sensitivity) const
{
    Q_ASSERT(m_loaded);

    int callIndex = m_calls.count() - 1;

    if (from) {
        callIndex = m_calls.indexOf(from) - 1;
    }

    for (int i = callIndex; i >= 0; --i) {
        ApiTraceCall *call = m_calls[i];
        if (call->contains(str, sensitivity)) {
            return call;
        }
    }
    return 0;
}

void ApiTraceFrame::setLastCallIndex(unsigned index)
{
    m_lastCallIndex = index;
}

unsigned ApiTraceFrame::lastCallIndex() const
{
    if (m_loaded && !m_calls.isEmpty()) {
        return m_calls.last()->index();
    } else {
        return m_lastCallIndex;
    }
}
