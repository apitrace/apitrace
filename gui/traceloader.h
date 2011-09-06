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
    TraceLoader(ApiTrace *parent);
    ~TraceLoader();

public slots:
    void loadTrace(const QString &filename);
    void loadFrame(int frameIdx);
    void setFrameMarker(ApiTrace::FrameMarker marker);

signals:
    void startedParsing();
    void parsed(float percent);
    void finishedParsing();

    void framesLoaded(const QList<ApiTraceFrame*> &frames);
    void frameLoaded(int frameIdx,
                     const QVector<ApiTraceCall*> &calls,
                     quint64 binaryDataSize);

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
    ApiTrace *m_trace;
    Trace::Parser m_parser;
    QString m_fileName;
    ApiTrace::FrameMarker m_frameMarker;

    typedef QMap<int, FrameOffset> FrameOffsets;
    FrameOffsets m_frameOffsets;

    QHash<QString, QUrl> m_helpHash;
};

#endif
