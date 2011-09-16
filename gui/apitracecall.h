#ifndef APITRACECALL_H
#define APITRACECALL_H

#include "apisurface.h"

#include <QStaticText>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "trace_model.hpp"


class ApiTrace;
class TraceLoader;

class VariantVisitor : public Trace::Visitor
{
public:
    VariantVisitor(TraceLoader *loader)
        : m_loader(loader)
    {}
    virtual void visit(Trace::Null *);
    virtual void visit(Trace::Bool *node);
    virtual void visit(Trace::SInt *node);
    virtual void visit(Trace::UInt *node);
    virtual void visit(Trace::Float *node);
    virtual void visit(Trace::String *node);
    virtual void visit(Trace::Enum *e);
    virtual void visit(Trace::Bitmask *bitmask);
    virtual void visit(Trace::Struct *str);
    virtual void visit(Trace::Array *array);
    virtual void visit(Trace::Blob *blob);
    virtual void visit(Trace::Pointer *ptr);

    QVariant variant() const
    {
        return m_variant;
    }
private:
    TraceLoader *m_loader;
    QVariant m_variant;
};


struct ApiTraceError
{
    int callIndex;
    QString type;
    QString message;
};

class ApiTraceEnumSignature
{
public:
    ApiTraceEnumSignature(const QString &name = QString(),
                          const QVariant &val=QVariant())\
        : m_name(name),
          m_value(val)
    {}

    QVariant value() const { return m_value; }
    QString name() const { return m_name; }
private:
    QString m_name;
    QVariant m_value;
};

class ApiEnum
{
public:
    ApiEnum(ApiTraceEnumSignature *sig=0);

    QString toString() const;

    QVariant value() const;
    QString name() const;
private:
    ApiTraceEnumSignature *m_sig;
};
Q_DECLARE_METATYPE(ApiEnum);

class ApiPointer
{
public:
    ApiPointer(unsigned long long val=0);

    QString toString() const;

    unsigned long long value() const;

private:
    unsigned long long m_value;
};
Q_DECLARE_METATYPE(ApiPointer);

class ApiBitmask
{
public:
    typedef QList<QPair<QString, unsigned long long> > Signature;

    ApiBitmask(const Trace::Bitmask *bitmask = 0);

    QString toString() const;

    unsigned long long value() const;
    Signature signature() const;

private:
    void init(const Trace::Bitmask *bitmask);
private:
    Signature m_sig;
    unsigned long long m_value;
};
Q_DECLARE_METATYPE(ApiBitmask);

class ApiStruct
{
public:
    struct Signature {
        QString name;
        QStringList memberNames;
    };

    ApiStruct(const Trace::Struct *s = 0);

    QString toString() const;
    Signature signature() const;
    QList<QVariant> values() const;

private:
    void init(const Trace::Struct *bitmask);
private:
    Signature m_sig;
    QList<QVariant> m_members;
};
Q_DECLARE_METATYPE(ApiStruct);

class ApiArray
{
public:
    ApiArray(const Trace::Array *arr = 0);
    ApiArray(const QVector<QVariant> &vals);

    QString toString() const;

    QVector<QVariant> values() const;
private:
    void init(const Trace::Array *arr);
private:
    QVector<QVariant> m_array;
};
Q_DECLARE_METATYPE(ApiArray);


QString apiVariantToString(const QVariant &variant, bool multiLine = false);

class ApiTraceFrame;

class ApiTraceState {
public:
    ApiTraceState();
    explicit ApiTraceState(const QVariantMap &parseJson);

    bool isEmpty() const;
    const QVariantMap & parameters() const;
    const QMap<QString, QString> & shaderSources() const;
    const QVariantMap & uniforms() const;
    const QList<ApiTexture> & textures() const;
    const QList<ApiFramebuffer> & framebuffers() const;

    ApiFramebuffer colorBuffer() const;
private:
    QVariantMap m_parameters;
    QMap<QString, QString> m_shaderSources;
    QVariantMap m_uniforms;
    QList<ApiTexture> m_textures;
    QList<ApiFramebuffer> m_framebuffers;
};
Q_DECLARE_METATYPE(ApiTraceState);

class ApiTraceCallSignature
{
public:
    ApiTraceCallSignature(const QString &name,
                          const QStringList &argNames);
    ~ApiTraceCallSignature();

    QString name() const
    {
        return m_name;
    }
    QStringList argNames() const
    {
        return m_argNames;
    }

    QUrl helpUrl() const;
    void setHelpUrl(const QUrl &url);

private:
    QString m_name;
    QStringList m_argNames;
    QUrl m_helpUrl;
};

class ApiTraceEvent
{
public:
    enum Type {
        None  = 0,
        Call  = 1 << 0,
        Frame = 1 << 1
    };
public:
    ApiTraceEvent();
    ApiTraceEvent(Type t);
    virtual ~ApiTraceEvent();

    Type type() const { return (Type)m_type; }

    virtual QStaticText staticText() const = 0;
    virtual int numChildren() const = 0;

    QVariantMap stateParameters() const;
    ApiTraceState *state() const;
    void setState(ApiTraceState *state);
    bool hasState() const
    {
        return m_state && !m_state->isEmpty();
    }

protected:
    int m_type : 4;
    mutable bool m_hasBinaryData;
    mutable int m_binaryDataIndex:8;
    ApiTraceState *m_state;

    mutable QStaticText *m_staticText;
};
Q_DECLARE_METATYPE(ApiTraceEvent*);

class ApiTraceCall : public ApiTraceEvent
{
public:
    ApiTraceCall(ApiTraceFrame *parentFrame, TraceLoader *loader,
                 const Trace::Call *tcall);
    ~ApiTraceCall();

    int index() const;
    QString name() const;
    QStringList argNames() const;
    QVector<QVariant> arguments() const;
    QVariant returnValue() const;
    QUrl helpUrl() const;
    void setHelpUrl(const QUrl &url);
    ApiTraceFrame *parentFrame()const;
    void setParentFrame(ApiTraceFrame *frame);

    bool hasError() const;
    QString error() const;
    void setError(const QString &msg);

    QVector<QVariant> originalValues() const;

    bool edited() const;
    void setEditedValues(const QVector<QVariant> &lst);
    QVector<QVariant> editedValues() const;
    void revert();

    bool contains(const QString &str,
                  Qt::CaseSensitivity sensitivity) const;

    ApiTrace *parentTrace() const;

    QString toHtml() const;
    QString searchText() const;
    QStaticText staticText() const;
    int numChildren() const;
    bool hasBinaryData() const;
    int binaryDataIndex() const;
private:
    int m_index;
    ApiTraceCallSignature *m_signature;
    QVector<QVariant> m_argValues;
    QVariant m_returnValue;
    ApiTraceFrame *m_parentFrame;

    QVector<QVariant> m_editedValues;

    QString m_error;

    mutable QString m_richText;
    mutable QString m_searchText;
};
Q_DECLARE_METATYPE(ApiTraceCall*);

class ApiTraceFrame : public ApiTraceEvent
{
public:
    ApiTraceFrame(ApiTrace *parent=0);
    ~ApiTraceFrame();
    int number;

    bool isEmpty() const;

    void setParentTrace(ApiTrace *parent);
    ApiTrace *parentTrace() const;

    void setNumChildren(int num);
    int numChildren() const;
    int numChildrenToLoad() const;
    QStaticText staticText() const;

    int callIndex(ApiTraceCall *call) const;
    ApiTraceCall *call(int idx) const;
    ApiTraceCall *callWithIndex(int index) const;
    void addCall(ApiTraceCall *call);
    QVector<ApiTraceCall*> calls() const;
    void setCalls(const QVector<ApiTraceCall*> &calls,
                  quint64 binaryDataSize);

    ApiTraceCall *findNextCall(ApiTraceCall *from,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity) const;

    ApiTraceCall *findPrevCall(ApiTraceCall *from,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity) const;

    int binaryDataSize() const;

    bool loaded() const;
    void setLoaded(bool l);

    void setLastCallIndex(unsigned index);
    unsigned lastCallIndex() const;
private:
    ApiTrace *m_parentTrace;
    quint64 m_binaryDataSize;
    QVector<ApiTraceCall*> m_calls;
    bool m_loaded;
    unsigned m_callsToLoad;
    unsigned m_lastCallIndex;
};
Q_DECLARE_METATYPE(ApiTraceFrame*);


#endif
