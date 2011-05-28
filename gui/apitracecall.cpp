#include "apitracecall.h"

#include "apitrace.h"
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
    "    font-size: 1.2em;\n"
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

ApiPointer::ApiPointer(unsigned long long val)
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

QString apiVariantToString(const QVariant &variant, bool multiLine)
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
        return variant.value<ApiStruct>().toString();
    }
    if (variant.canConvert<ApiArray>()) {
        return variant.value<ApiArray>().toString();
    }
    if (variant.canConvert<ApiEnum>()) {
        return variant.value<ApiEnum>().toString();
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
    for (const Trace::BitmaskFlag *it = bitmask->sig->flags;
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

ApiStruct::ApiStruct(const Trace::Struct *s)
{
    init(s);
}

QString ApiStruct::toString() const
{
    QString str;

    str += QLatin1String("{");
    for (unsigned i = 0; i < m_members.count(); ++i) {
        str += m_sig.memberNames[i] %
               QLatin1Literal(" = ") %
               apiVariantToString(m_members[i]);
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
    for (unsigned i = 0; i < s->sig->num_members; ++i) {
        VariantVisitor vis;
        m_sig.memberNames.append(
            QString::fromStdString(s->sig->member_names[i]));
        s->members[i]->visit(vis);
        m_members.append(vis.variant());
    }
}

void VariantVisitor::visit(Trace::Null *)
{
    m_variant = QVariant::fromValue(ApiPointer(0));
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
    QVariant val = QVariant(e->sig->value);

    m_variant = QVariant::fromValue(
        ApiEnum(QString::fromStdString(e->sig->name), val));
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
    //XXX
    //FIXME: this is a nasty hack. Trace::Blob's can't
    //   delete the contents in the destructor because
    //   the data is being used by other calls. We piggy back
    //   on that assumption and don't deep copy the data. If
    //   Blob's will start deleting the data we will need to
    //   start deep copying it or switch to using something like
    //   Boost's shared_ptr or Qt's QSharedPointer to handle it
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

ApiArray::ApiArray(const QList<QVariant> &vals)
    : m_array(vals)
{
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
    if (m_staticText && !m_staticText->text().isEmpty())
        return *m_staticText;

    QVariantList argValues = arguments();

    QString richText = QString::fromLatin1(
        "<span style=\"font-weight:bold\">%1</span>(").arg(m_name);
    for (int i = 0; i < m_argNames.count(); ++i) {
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
        if (i < m_argNames.count() - 1)
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

    m_richText = QLatin1String("<div class=\"call\">");

    if (m_helpUrl.isEmpty()) {
        m_richText += QString::fromLatin1(
            "%1) <span class=\"callName\">%2</span>(")
                      .arg(m_index)
                      .arg(m_name);
    } else {
        m_richText += QString::fromLatin1(
            "%1) <span class=\"callName\"><a href=\"%2\">%3</a></span>(")
                      .arg(m_index)
                      .arg(m_helpUrl.toString())
                      .arg(m_name);
    }

    QVariantList argValues = arguments();
    for (int i = 0; i < m_argNames.count(); ++i) {
        m_richText +=
            QLatin1String("<span class=\"arg-name\">") +
            m_argNames[i] +
            QLatin1String("</span>") +
            QLatin1Literal(" = ") +
            QLatin1Literal("<span class=\"arg-value\">") +
            apiVariantToString(argValues[i], true) +
            QLatin1Literal("</span>");
        if (i < m_argNames.count() - 1)
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

QString ApiTraceCall::filterText() const
{
    if (!m_filterText.isEmpty())
        return m_filterText;

    QVariantList argValues = arguments();
    m_filterText = m_name + QLatin1Literal("(");
    for (int i = 0; i < m_argNames.count(); ++i) {
        m_filterText += m_argNames[i] +
                        QLatin1Literal(" = ") +
                        apiVariantToString(argValues[i]);
        if (i < m_argNames.count() - 1)
            m_filterText += QLatin1String(", ");
    }
    m_filterText += QLatin1String(")");

    if (m_returnValue.isValid()) {
        m_filterText += QLatin1Literal(" = ") +
                        apiVariantToString(m_returnValue);
    }
    m_filterText.squeeze();
    return m_filterText;
}

QStaticText ApiTraceFrame::staticText() const
{
    if (m_staticText && !m_staticText->text().isEmpty())
        return *m_staticText;

    QString richText;

    //mark the frame if it uploads more than a meg a frame
    if (m_binaryDataSize > (1024*1024)) {
        richText =
            QObject::tr(
                "<span style=\"font-weight:bold;\">"
                "Frame&nbsp;%1</span>"
                "<span style=\"font-style:italic;\">"
                "&nbsp;&nbsp;&nbsp;&nbsp;(%2MB)</span>")
            .arg(number)
            .arg(double(m_binaryDataSize / (1024.*1024.)), 0, 'g', 2);
    } else {
        richText =
            QObject::tr(
                "<span style=\"font-weight:bold\">Frame %1</span>")
            .arg(number);
    }

    if (!m_staticText)
        m_staticText = new QStaticText(richText);

    QTextOption opt;
    opt.setWrapMode(QTextOption::NoWrap);
    m_staticText->setTextOption(opt);
    m_staticText->prepare();

    return *m_staticText;
}

int ApiTraceCall::numChildren() const
{
    return 0;
}

int ApiTraceFrame::numChildren() const
{
    return m_calls.count();
}

ApiTraceFrame::ApiTraceFrame()
    : ApiTraceEvent(ApiTraceEvent::Frame),
      m_parentTrace(0),
      m_binaryDataSize(0)
{
}

ApiTraceCall::ApiTraceCall()
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_hasBinaryData(false),
      m_binaryDataIndex(0)
{
}

ApiTraceEvent::ApiTraceEvent()
    : m_type(ApiTraceEvent::None),
      m_staticText(0)
{
}

ApiTraceEvent::ApiTraceEvent(Type t)
    : m_type(t),
      m_staticText(0)
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
    QVariantMap attachedShaders =
        parsedJson[QLatin1String("shaders")].toMap();
    QVariantMap::const_iterator itr;


    for (itr = attachedShaders.constBegin(); itr != attachedShaders.constEnd();
         ++itr) {
        QString type = itr.key();
        QString source = itr.value().toString();
        m_shaderSources[type] = source;
    }

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

        Q_ASSERT(type == QLatin1String("uint8"));
        Q_ASSERT(normalized == true);

        QByteArray dataArray =
            image[QLatin1String("__data__")].toByteArray();

        ApiTexture tex;
        tex.setSize(size);
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

        Q_ASSERT(type == QLatin1String("uint8"));
        Q_ASSERT(normalized == true);

        QByteArray dataArray =
            buffer[QLatin1String("__data__")].toByteArray();

        ApiFramebuffer fbo;
        fbo.setSize(size);
        fbo.setNumChannels(numChannels);
        fbo.setType(itr.key());
        fbo.contentsFromBase64(dataArray);
        m_framebuffers.append(fbo);
    }
}

QVariantMap ApiTraceState::parameters() const
{
    return m_parameters;
}

QMap<QString, QString> ApiTraceState::shaderSources() const
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

QList<QVariant> ApiArray::values() const
{
    return m_array;
}

int ApiTraceCall::index() const
{
    return m_index;
}

QString ApiTraceCall::name() const
{
    return m_name;
}

QStringList ApiTraceCall::argNames() const
{
    return m_argNames;
}

QVariantList ApiTraceCall::arguments() const
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

QUrl ApiTraceCall::helpUrl() const
{
    return m_helpUrl;
}

ApiTraceCall::ApiTraceCall(const Trace::Call *call)
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_hasBinaryData(false),
      m_binaryDataIndex(0)
{
    m_name = QString::fromStdString(call->sig->name);
    m_index = call->no;

    QString argumentsText;
    for (int i = 0; i < call->sig->num_args; ++i) {
        m_argNames +=
            QString::fromStdString(call->sig->arg_names[i]);
    }
    if (call->ret) {
        VariantVisitor retVisitor;
        call->ret->visit(retVisitor);
        m_returnValue = retVisitor.variant();
    }
    for (int i = 0; i < call->args.size(); ++i) {
        VariantVisitor argVisitor;
        call->args[i]->visit(argVisitor);
        m_argValues += argVisitor.variant();
        if (m_argValues[i].type() == QVariant::ByteArray) {
            m_hasBinaryData = true;
            m_binaryDataIndex = i;
        }
    }
}

void ApiTraceCall::setHelpUrl(const QUrl &url)
{
    m_helpUrl = url;
}

void ApiTraceCall::setParentFrame(ApiTraceFrame *frame)
{
    m_parentFrame = frame;
}

ApiTraceFrame * ApiTraceCall::parentFrame()const
{
    return m_parentFrame;
}

ApiTraceEvent::~ApiTraceEvent()
{
    delete m_staticText;
}

void ApiTraceCall::revert()
{
    setEditedValues(QVariantList());
}

ApiTrace * ApiTraceFrame::parentTrace() const
{
    return m_parentTrace;
}

void ApiTraceFrame::setParentTrace(ApiTrace *trace)
{
    m_parentTrace = trace;
}

QVariantList ApiTraceCall::originalValues() const
{
    return m_argValues;
}

void ApiTraceCall::setEditedValues(const QVariantList &lst)
{
    ApiTrace *trace = parentTrace();

    m_editedValues = lst;
    //lets regenerate data
    m_richText = QString();
    m_filterText = QString();
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

QVariantList ApiTraceCall::editedValues() const
{
    return m_editedValues;
}

bool ApiTraceCall::edited() const
{
    return !m_editedValues.isEmpty();
}

ApiEnum::ApiEnum(const QString &name, const QVariant &val)
    : m_name(name),
      m_value(val)
{
}

QString ApiEnum::toString() const
{
    return m_name;
}

QVariant ApiEnum::value() const
{
    return m_value;
}

QString ApiEnum::name() const
{
    return m_name;
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

unsigned long long ApiPointer::value() const
{
    return m_value;
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
        ApiTrace *trace = parentTrace();
        m_error = msg;
        m_richText = QString();
        if (trace)
            trace->callError(this);
    }
}

ApiTrace * ApiTraceCall::parentTrace() const
{
    if (m_parentFrame)
        return m_parentFrame->parentTrace();
    return NULL;
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

QList<ApiTraceCall*> ApiTraceFrame::calls() const
{
    return m_calls;
}

ApiTraceCall * ApiTraceFrame::call(int idx) const
{
    return m_calls.value(idx);
}

int ApiTraceFrame::callIndex(ApiTraceCall *call) const
{
    return m_calls.indexOf(call);
}

bool ApiTraceFrame::isEmpty() const
{
    return m_calls.isEmpty();
}

int ApiTraceFrame::binaryDataSize() const
{
    return m_binaryDataSize;
}

