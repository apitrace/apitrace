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
    ".thread-id {\n"
    "    color: #aaaaaa;\n"
    "    min-width: 3em;\n"
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
plainTextToHTML(const QString & plain, bool multiLine, bool forceNoQuote = false)
{
    int col = 0;
    bool quote = false;
    QString rich;
    for (auto & ch : plain) {
        if (ch == QLatin1Char('\n')){
            if (multiLine) {
                rich += QLatin1String("<br>\n");
            } else {
                rich += QLatin1String("\\n");
            }
            col = 0;
            quote = true;
        } else {
            if (ch == QLatin1Char('\t')){
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
            } else if (ch.isSpace()) {
                rich += QChar(0x00a0U);
                quote = true;
            } else if (ch == QLatin1Char('<')) {
                rich += QLatin1String("&lt;");
            } else if (ch == QLatin1Char('>')) {
                rich += QLatin1String("&gt;");
            } else if (ch == QLatin1Char('&')) {
                rich += QLatin1String("&amp;");
            } else {
                rich += ch;
            }
            ++col;
        }
    }

    if (quote && !forceNoQuote) {
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
    m_variant = QVariant(QString::fromLatin1(node->value));
}

void VariantVisitor::visit(trace::WString *node)
{
    m_variant = QVariant(QString::fromWCharArray(node->value));
}

void VariantVisitor::visit(trace::Enum *e)
{
    m_variant = QVariant::fromValue(ApiEnum(e->sig, e->value));
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

void VariantVisitor::visit(trace::Repr *repr)
{
    /* TODO: Preserve both the human and machine value */
    repr->humanValue->visit(*this);
}

ApiEnum::ApiEnum(const trace::EnumSig *sig, signed long long value)
    : m_sig(sig), m_value(value)
{
    Q_ASSERT(m_sig);
}

QString ApiEnum::toString() const
{
    Q_ASSERT(m_sig);

    for (const trace::EnumValue *it = m_sig->values;
         it != m_sig->values + m_sig->num_values; ++it) {
        if (m_value == it->value) {
            return QString::fromLatin1(it->name);
        }
    }

    return QString::fromLatin1("%1").arg(m_value);
}

QVariant ApiEnum::value() const
{
    if (m_sig) {
        return QVariant::fromValue(m_value);
    }
    Q_ASSERT(!"should never happen");
    return QVariant();
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
        QPair<QString, unsigned long long> pair;

        pair.first = QString::fromLatin1(it->name);
        pair.second = it->value;

        m_sig.append(pair);
    }
}

QString ApiBitmask::toString() const
{
    QString str;
    unsigned long long value = m_value;
    bool first = true;
    for (Signature::const_iterator it = m_sig.begin(); it != m_sig.end(); ++it) {
        Q_ASSERT(it->second || first);
        if ((it->second && (value & it->second) == it->second) ||
            (!it->second && value == 0)) {
            if (!first) {
                str += QLatin1String(" | ");
            }
            str += it->first;
            value &= ~it->second;
            first = false;
        }
        if (value == 0) {
            break;
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

    m_sig.name = QString::fromLatin1(s->sig->name);
    for (unsigned i = 0; i < s->sig->num_members; ++i) {
        VariantVisitor vis;
        m_sig.memberNames.append(
            QString::fromLatin1(s->sig->member_names[i]));
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
    for (auto & value : arr->values) {
        VariantVisitor vis;
        value->visit(vis);

        m_array.append(vis.variant());
    }
    m_array.squeeze();
}

ApiTraceState::ApiTraceState()
{
}

static ApiTexture getTextureFrom(QVariantMap const &image, QString label)
{
    QSize size(image[QLatin1String("__width__")].toInt(),
               image[QLatin1String("__height__")].toInt());
    QString cls = image[QLatin1String("__class__")].toString();
    int depth =
        image[QLatin1String("__depth__")].toInt();
    QString formatName =
        image[QLatin1String("__format__")].toString();

    QByteArray dataArray =
        image[QLatin1String("__data__")].toByteArray();

    QString userLabel =
        image[QLatin1String("__label__")].toString();
    if (!userLabel.isEmpty()) {
        label += QString(", \"%1\"").arg(userLabel);
    }

    ApiTexture tex;
    tex.setSize(size);
    tex.setDepth(depth);
    tex.setFormatName(formatName);
    tex.setLabel(label);
    tex.setData(dataArray);
    return tex;
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

    m_buffers = parsedJson[QLatin1String("buffers")].toMap();

    m_shaderStorageBufferBlocks =
        parsedJson[QLatin1String("shaderstoragebufferblocks")].toMap();

    {
        QVariantMap textures =
            parsedJson[QLatin1String("textures")].toMap();
        for (itr = textures.constBegin(); itr != textures.constEnd(); ++itr) {
            m_textures.append(getTextureFrom(itr.value().toMap(), itr.key()));
        }
    }

    QVariantMap fbos =
        parsedJson[QLatin1String("framebuffer")].toMap();
    for (itr = fbos.constBegin(); itr != fbos.constEnd(); ++itr) {
        QVariantMap buffer = itr.value().toMap();
        QSize size(buffer[QLatin1String("__width__")].toInt(),
                   buffer[QLatin1String("__height__")].toInt());
        QString cls = buffer[QLatin1String("__class__")].toString();
        int depth = buffer[QLatin1String("__depth__")].toInt();
        QString formatName = buffer[QLatin1String("__format__")].toString();

        QByteArray dataArray =
            buffer[QLatin1String("__data__")].toByteArray();

        QString label = itr.key();
        QString userLabel =
            buffer[QLatin1String("__label__")].toString();
        if (!userLabel.isEmpty()) {
            label += QString(", \"%1\"").arg(userLabel);
        }

        ApiFramebuffer fbo;
        fbo.setSize(size);
        fbo.setDepth(depth);
        fbo.setFormatName(formatName);
        fbo.setType(label);
        fbo.setData(dataArray);
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

const QVariantMap & ApiTraceState::buffers() const
{
    return m_buffers;
}

const QVariantMap &ApiTraceState::shaderStorageBufferBlocks() const
{
    return m_shaderStorageBufferBlocks;
}

bool ApiTraceState::isEmpty() const
{
    return m_parameters.isEmpty() &&
           m_shaderSources.isEmpty() &&
           m_textures.isEmpty() &&
           m_framebuffers.isEmpty();
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
      m_binaryDataIndex(-1),
      m_state(0),
      m_staticText(0)
{
}

ApiTraceEvent::ApiTraceEvent(Type t)
    : m_type(t),
      m_binaryDataIndex(-1),
      m_state(0),
      m_staticText(0)
{
    Q_ASSERT(m_type == t);
}

ApiTraceEvent::~ApiTraceEvent()
{
    delete m_state;
    delete m_staticText;
}

ApiTraceState *ApiTraceEvent::state() const
{
    return m_state;
}

void ApiTraceEvent::setState(ApiTraceState *state)
{
    m_state = state;
}

void ApiTraceEvent::setThumbnail(const QImage & thumbnail)
{
    m_thumbnail = thumbnail;
}

const QImage & ApiTraceEvent::thumbnail() const
{
    return m_thumbnail;
}

ApiTraceCall::ApiTraceCall(ApiTraceFrame *parentFrame,
                           TraceLoader *loader,
                           const trace::Call *call)
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_parentFrame(parentFrame),
      m_parentCall(0)
{
    loadData(loader, call);
}

ApiTraceCall::ApiTraceCall(ApiTraceCall *parentCall,
                           TraceLoader *loader,
                           const trace::Call *call)
    : ApiTraceEvent(ApiTraceEvent::Call),
      m_parentFrame(parentCall->parentFrame()),
      m_parentCall(parentCall)
{
    loadData(loader, call);
}


ApiTraceCall::~ApiTraceCall()
{
}


void
ApiTraceCall::loadData(TraceLoader *loader,
                       const trace::Call *call)
{
    m_index = call->no;
    m_thread = call->thread_id;
    m_signature = loader->signature(call->sig->id);

    if (!m_signature) {
        QString name = QString::fromLatin1(call->sig->name);
        QStringList argNames;
        argNames.reserve(call->sig->num_args);
        for (int i = 0; i < call->sig->num_args; ++i) {
            argNames += QString::fromLatin1(call->sig->arg_names[i]);
        }
        m_signature = new ApiTraceCallSignature(name, argNames);
        loader->addSignature(call->sig->id, m_signature);
    }
    if (call->ret) {
        VariantVisitor retVisitor;
        call->ret->visit(retVisitor);
        m_returnValue = retVisitor.variant();
    }
    m_argValues.reserve(call->args.size());
    for (int i = 0; i < call->args.size(); ++i) {
        if (call->args[i].value) {
            VariantVisitor argVisitor;
            call->args[i].value->visit(argVisitor);
            m_argValues.append(argVisitor.variant());
            if (m_argValues[i].type() == QVariant::ByteArray) {
                m_binaryDataIndex = i;
            }
        } else {
            m_argValues.append(QVariant());
        }
    }
    m_argValues.squeeze();
    m_flags = call->flags;
    if (call->backtrace != NULL) {
        QString qbacktrace;
        for (auto frame : *call->backtrace) {
            if (frame->module != NULL) {
                qbacktrace += QString("%1 ").arg(frame->module);
            }
            if (frame->function != NULL) {
                qbacktrace += QString("at %1() ").arg(frame->function);
            }
            if (frame->filename != NULL) {
                qbacktrace += QString("at %1").arg(frame->filename);
                if (frame->linenumber >= 0) {
                    qbacktrace += QString(":%1 ").arg(frame->linenumber);
                }
            }
            else {
                if (frame->offset >= 0) {
                    qbacktrace += QString("[0x%1]").arg(frame->offset, 0, 16);
                }
            }
            qbacktrace += "\n";
        }
        this->setBacktrace(qbacktrace);
    }
}

ApiTraceCall *
ApiTraceCall::parentCall() const
{
    return m_parentCall;
}


ApiTraceEvent *
ApiTraceCall::parentEvent() const
{
    if (m_parentCall)
        return m_parentCall;
    else
        return m_parentFrame;
}

QVector<ApiTraceCall*>
ApiTraceCall::children() const
{
    return m_children;
}

void
ApiTraceCall::addChild(ApiTraceCall *call)
{
    m_children.append(call);
}


int
ApiTraceCall::callIndex(ApiTraceCall *call) const
{
    return m_children.indexOf(call);
}

void
ApiTraceCall::finishedAddingChildren()
{
    m_children.squeeze();
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

ApiTraceEvent *
ApiTraceCall::eventAtRow(int row) const
{
    if (row < m_children.count())
        return m_children.value(row);
    else
        return NULL;
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
    return m_binaryDataIndex >= 0;
}

int ApiTraceCall::binaryDataIndex() const
{
    Q_ASSERT(hasBinaryData());
    return m_binaryDataIndex;
}

QString ApiTraceCall::backtrace() const
{
    return m_backtrace;
}

void ApiTraceCall::setBacktrace(QString backtrace)
{
    m_backtrace = backtrace;
}

QStaticText ApiTraceCall::staticText() const
{
    if (m_staticText && !m_staticText->text().isEmpty())
        return *m_staticText;

    QStringList argNames = m_signature->argNames();
    QVector<QVariant> argValues = arguments();

    QString richText;

    // TODO: Toggle this via a menu option.
    if (0) {
        richText += QString::fromLatin1("<span style=\"color: #aaaaaa; min-width: 3em;\">@%1</span> ")
            .arg(m_thread);
    }

    if (m_flags & trace::CALL_FLAG_MARKER &&
        argNames.count() &&
        argValues.last().userType() == QVariant::String)
    {
        // special handling for string markers
        QString msgText = plainTextToHTML(argValues.last().toString(), false, true);
        richText += QString::fromLatin1(
            "<span style=\"font-weight:bold;color:green;\">%1</span>")
            .arg(msgText);
    } else {
        richText += QString::fromLatin1(
            "<span style=\"font-weight:bold\">%1</span>(").arg(
                m_signature->name());
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
        QString::fromLatin1("%1 ")
        .arg(m_index);
    QString parentTip;
    if (m_parentFrame) {
        parentTip =
            QString::fromLatin1("Frame %1")
            .arg(m_parentFrame->number);
    }

    m_richText += QString::fromLatin1("<span class=\"thread-id\">@%1</span> ")
        .arg(m_thread);

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
    return m_children.count();
}

bool ApiTraceCall::contains(const QString &str,
                            Qt::CaseSensitivity sensitivity) const
{
    QString txt = searchText();
    return txt.contains(str, sensitivity);
}

void ApiTraceCall::missingThumbnail()
{
    m_parentFrame->parentTrace()->missingThumbnail(this);
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
    return m_children.count();
}

int ApiTraceFrame::numTotalCalls() const
{
    return m_calls.count();
}

ApiTrace * ApiTraceFrame::parentTrace() const
{
    return m_parentTrace;
}

QVector<ApiTraceCall*> ApiTraceFrame::calls() const
{
    return m_calls;
}

ApiTraceEvent * ApiTraceFrame::eventAtRow(int row) const
{
    if (row < m_children.count())
        return m_children.value(row);
    else
        return NULL;
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
    return m_children.indexOf(call);
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

void ApiTraceFrame::setCalls(const QVector<ApiTraceCall*> &children,
                             const QVector<ApiTraceCall*> &calls,
                             quint64 binaryDataSize)
{
    m_children = children;
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

void ApiTraceFrame::missingThumbnail()
{
    m_parentTrace->missingThumbnail(this);
}
