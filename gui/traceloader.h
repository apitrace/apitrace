#ifndef TRACELOADER_H
#define TRACELOADER_H


#include "apitrace.h"
#include "trace_file.hpp"
#include "trace_parser.hpp"

#include <QObject>
#include <QList>
#include <QMap>

class TraceLoader : public QObject
{
    Q_OBJECT
public:
    TraceLoader(QObject *parent=0);
    ~TraceLoader();


    ApiTraceCallSignature *signature(unsigned id);
    void addSignature(unsigned id, ApiTraceCallSignature *signature);

    ApiTraceEnumSignature *enumSignature(unsigned id);
    void addEnumSignature(unsigned id, ApiTraceEnumSignature *signature);

public slots:
    void loadTrace(const QString &filename);
    void loadFrame(ApiTraceFrame *frame);
    void setFrameMarker(ApiTrace::FrameMarker marker);

signals:
    void startedParsing();
    void parsed(int percent);
    void finishedParsing();

    void framesLoaded(const QList<ApiTraceFrame*> &frames);
    void frameLoaded(ApiTraceFrame *frame);

private:
    struct FrameOffset {
        FrameOffset()
            : numberOfCalls(0)
        {}
        FrameOffset(const Trace::File::Offset &s)
            : start(s),
              numberOfCalls(0)
        {}

        Trace::File::Offset start;
        int numberOfCalls;
        unsigned callNumber;
    };
    bool isCallAFrameMarker(const Trace::Call *call) const;
    int numberOfFrames() const;
    int numberOfCallsInFrame(int frameIdx) const;

    void loadHelpFile();
    void scanTrace();
    void parseTrace();

private:
    Trace::Parser m_parser;
    QString m_fileName;
    ApiTrace::FrameMarker m_frameMarker;

    typedef QMap<int, FrameOffset> FrameOffsets;
    FrameOffsets m_frameOffsets;

    QHash<QString, QUrl> m_helpHash;

    QVector<ApiTraceCallSignature*> m_signatures;
    QVector<ApiTraceEnumSignature*> m_enumSignatures;
};

#endif
