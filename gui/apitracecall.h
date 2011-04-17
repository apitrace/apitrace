#ifndef APITRACECALL_H
#define APITRACECALL_H

#include "apisurface.h"

#include <QStaticText>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "trace_model.hpp"


class VariantVisitor : public Trace::Visitor
{
public:
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
    QVariant m_variant;
};


class ApiPointer
{
public:
    ApiPointer(int val=0);

    QString toString() const;
private:
    int m_value;
};
Q_DECLARE_METATYPE(ApiPointer);

class ApiBitmask
{
public:
    typedef QList<QPair<QString, unsigned long long> > Signature;

    ApiBitmask(const Trace::Bitmask *bitmask = 0);

    QString toString() const;

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
    ApiArray(const QList<QVariant> &vals);

    QString toString() const;

    QList<QVariant> values() const;
private:
    void init(const Trace::Array *arr);
private:
    QList<QVariant> m_array;
};
Q_DECLARE_METATYPE(ApiArray);


QString apiVariantToString(const QVariant &variant);

class ApiTraceFrame;

class ApiTraceState {
public:
    ApiTraceState();
    explicit ApiTraceState(const QVariantMap &parseJson);

    bool isEmpty() const;
    QVariantMap parameters() const;
    QMap<QString, QString> shaderSources() const;
    QList<ApiTexture> textures() const;
    QList<ApiFramebuffer> framebuffers() const;

private:
    QVariantMap m_parameters;
    QMap<QString, QString> m_shaderSources;
    QList<ApiTexture> m_textures;
    QList<ApiFramebuffer> m_framebuffers;
};
Q_DECLARE_METATYPE(ApiTraceState);

class ApiTraceEvent
{
public:
    enum Type {
        None,
        Call,
        Frame
    };
public:
    ApiTraceEvent();
    ApiTraceEvent(Type t);
    virtual ~ApiTraceEvent();

    Type type() const { return m_type; }

    virtual QStaticText staticText() const = 0;
    virtual int numChildren() const = 0;

    QVariantMap stateParameters() const;
    ApiTraceState state() const;
    void setState(const ApiTraceState &state);

protected:
    Type m_type;
    ApiTraceState m_state;

    mutable QStaticText *m_staticText;
};
Q_DECLARE_METATYPE(ApiTraceEvent*);

class ApiTraceCall : public ApiTraceEvent
{
public:
    ApiTraceCall();
    ApiTraceCall(const Trace::Call *tcall);
    ~ApiTraceCall();

    int index() const;
    QString name() const;
    QStringList argNames() const;
    QVariantList arguments() const;
    QVariant returnValue() const;
    QUrl helpUrl() const;
    void setHelpUrl(const QUrl &url);
    ApiTraceFrame *parentFrame()const;
    void setParentFrame(ApiTraceFrame *frame);

    void revert();

    QString toHtml() const;
    QString filterText() const;
    QStaticText staticText() const;
    int numChildren() const;
    bool hasBinaryData() const;
    int binaryDataIndex() const;
private:
    int m_index;
    QString m_name;
    QStringList m_argNames;
    QVariantList m_argValues;
    QVariant m_returnValue;
    ApiTraceFrame *m_parentFrame;
    QUrl m_helpUrl;

    mutable QString m_richText;
    mutable QString m_filterText;
    mutable bool m_hasBinaryData;
    mutable int m_binaryDataIndex;
};
Q_DECLARE_METATYPE(ApiTraceCall*);

class ApiTraceFrame : public ApiTraceEvent
{
public:
    ApiTraceFrame();
    int number;
    QList<ApiTraceCall*> calls;

    int numChildren() const;
    QStaticText staticText() const;
};
Q_DECLARE_METATYPE(ApiTraceFrame*);


#endif
