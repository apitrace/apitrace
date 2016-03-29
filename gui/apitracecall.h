#pragma once

#include "apisurface.h"

#include <QStaticText>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "trace_model.hpp"


class ApiTrace;
class TraceLoader;

class VariantVisitor : public trace::Visitor
{
public:
    virtual void visit(trace::Null *) override;
    virtual void visit(trace::Bool *node) override;
    virtual void visit(trace::SInt *node) override;
    virtual void visit(trace::UInt *node) override;
    virtual void visit(trace::Float *node) override;
    virtual void visit(trace::Double *node) override;
    virtual void visit(trace::String *node) override;
    virtual void visit(trace::WString *node) override;
    virtual void visit(trace::Enum *e) override;
    virtual void visit(trace::Bitmask *bitmask) override;
    virtual void visit(trace::Struct *str) override;
    virtual void visit(trace::Array *array) override;
    virtual void visit(trace::Blob *blob) override;
    virtual void visit(trace::Pointer *ptr) override;
    virtual void visit(trace::Repr *ptr) override;

    QVariant variant() const
    {
        return m_variant;
    }
private:
    QVariant m_variant;
};


struct ApiTraceError
{
    int callIndex;
    QString type;
    QString message;
};

class ApiEnum
{
public:
    ApiEnum(const trace::EnumSig *sig=0, signed long long value = 0);

    QString toString() const;

    QVariant value() const;
private:
    const trace::EnumSig *m_sig;
    signed long long m_value;
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

    ApiBitmask(const trace::Bitmask *bitmask = 0);

    QString toString() const;

    unsigned long long value() const;
    Signature signature() const;

private:
    void init(const trace::Bitmask *bitmask);
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

    ApiStruct(const trace::Struct *s = 0);

    QString toString(bool multiLine = false) const;
    Signature signature() const;
    QList<QVariant> values() const;

private:
    void init(const trace::Struct *bitmask);
private:
    Signature m_sig;
    QList<QVariant> m_members;
};
Q_DECLARE_METATYPE(ApiStruct);

class ApiArray
{
public:
    ApiArray(const trace::Array *arr = 0);
    ApiArray(const QVector<QVariant> &vals);

    QString toString(bool multiLine = false) const;

    QVector<QVariant> values() const;
private:
    void init(const trace::Array *arr);
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
    const QVariantMap & buffers() const;
    const QVariantMap & shaderStorageBufferBlocks() const;
    const QList<ApiTexture> & textures() const;
    const QList<ApiFramebuffer> & framebuffers() const;

    ApiFramebuffer colorBuffer() const;
private:
    QVariantMap m_parameters;
    QMap<QString, QString> m_shaderSources;
    QVariantMap m_uniforms;
    QVariantMap m_buffers;
    QVariantMap m_shaderStorageBufferBlocks;
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

class ApiTraceCall;

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
    virtual int callIndex(ApiTraceCall *call) const = 0;
    virtual ApiTraceEvent *eventAtRow(int row) const = 0;

    ApiTraceState *state() const;
    void setState(ApiTraceState *state);
    bool hasState() const
    {
        return m_state && !m_state->isEmpty();
    }

    void setThumbnail(const QImage & thumbnail);
    const QImage & thumbnail() const;

    virtual void missingThumbnail() = 0;

protected:
    int m_type : 4;
    mutable int m_binaryDataIndex:8;
    ApiTraceState *m_state;

    mutable QStaticText *m_staticText;

    QImage m_thumbnail;
};
Q_DECLARE_METATYPE(ApiTraceEvent*);

class ApiTraceCall : public ApiTraceEvent
{
public:
    ApiTraceCall(ApiTraceCall *parentCall, TraceLoader *loader,
                 const trace::Call *tcall);
    ApiTraceCall(ApiTraceFrame *parentFrame, TraceLoader *loader,
                 const trace::Call *tcall);
    ~ApiTraceCall();

    int index() const;
    QString name() const;
    QStringList argNames() const;
    QVector<QVariant> arguments() const;
    QVariant returnValue() const;
    trace::CallFlags flags() const;
    QUrl helpUrl() const;
    void setHelpUrl(const QUrl &url);
    ApiTraceFrame *parentFrame()const;
    void setParentFrame(ApiTraceFrame *frame);

    int callIndex(ApiTraceCall *call) const override;

    ApiTraceEvent *parentEvent() const;
    ApiTraceCall *parentCall() const;
    QVector<ApiTraceCall*> children() const;
    ApiTraceEvent *eventAtRow(int row) const override;
    void addChild(ApiTraceCall *call);
    void finishedAddingChildren();

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
    QStaticText staticText() const override;
    int numChildren() const override;
    bool hasBinaryData() const;
    int binaryDataIndex() const;

    QString backtrace() const;
    void setBacktrace(QString backtrace);

    void missingThumbnail() override;

private:
    void loadData(TraceLoader *loader,
                  const trace::Call *tcall);
private:
    int m_index;
    unsigned m_thread;
    ApiTraceCallSignature *m_signature;
    QVector<QVariant> m_argValues;
    QVariant m_returnValue;
    trace::CallFlags m_flags;
    ApiTraceFrame *m_parentFrame;
    ApiTraceCall *m_parentCall;
    QVector<ApiTraceCall*> m_children;

    QVector<QVariant> m_editedValues;

    QString m_error;

    QString m_backtrace;

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
    int numChildren() const override;
    int numChildrenToLoad() const;
    int numTotalCalls() const;
    QStaticText staticText() const override;

    ApiTraceEvent *eventAtRow(int row) const override;
    int callIndex(ApiTraceCall *call) const override;
    ApiTraceCall *callWithIndex(int index) const;
    QVector<ApiTraceCall*> calls() const;
    void setCalls(const QVector<ApiTraceCall*> &topLevelCalls,
                  const QVector<ApiTraceCall*> &allCalls,
                  quint64 binaryDataSize);

    ApiTraceCall *findNextCall(ApiTraceCall *from,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity) const;

    ApiTraceCall *findPrevCall(ApiTraceCall *from,
                               const QString &str,
                               Qt::CaseSensitivity sensitivity) const;

    int binaryDataSize() const;

    bool isLoaded() const;

    void setLastCallIndex(unsigned index);
    unsigned lastCallIndex() const;

    void missingThumbnail() override;

private:
    ApiTrace *m_parentTrace;
    quint64 m_binaryDataSize;
    QVector<ApiTraceCall*> m_children;
    QVector<ApiTraceCall*> m_calls;
    bool m_loaded;
    unsigned m_callsToLoad;
    unsigned m_lastCallIndex;
};
Q_DECLARE_METATYPE(ApiTraceFrame*);
