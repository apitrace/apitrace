#ifndef TRACE_LOADER_HPP
#define TRACE_LOADER_HPP

#include "trace_file.hpp"
#include "trace_parser.hpp"

#include <string>
#include <map>
#include <queue>
#include <vector>

namespace Trace  {

class Frame;

class Loader
{
public:
    enum FrameMarker {
        FrameMarker_SwapBuffers,
        FrameMarker_Flush,
        FrameMarker_Finish,
        FrameMarker_Clear
    };
public:
    Loader();
    ~Loader();

    Loader::FrameMarker frameMarker() const;
    void setFrameMarker(Loader::FrameMarker marker);

    int numberOfFrames() const;
    int numberOfCallsInFrame(int frameIdx);

    bool open(const char *filename);
    void close();

    std::vector<Trace::Call*> frame(int idx);

private:
    struct FrameOffset {
        FrameOffset()
            : numberOfCalls(0)
        {}
        FrameOffset(const File::Offset &s,
                    const File::Offset &e)
            : start(s),
              end(e),
              numberOfCalls(0)
        {}

        File::Offset start;
        File::Offset end;
        int numberOfCalls;
    };
    bool isCallAFrameMarker(const Trace::Call *call) const;

private:
    Trace::Parser m_parser;
    FrameMarker m_frameMarker;

    std::map<int, Trace::Frame*> m_frameCache;
    std::queue<Trace::Frame*> m_loadedFrames;

    std::map<int, FrameOffset> m_frameOffsets;

    Trace::File *file;
};

}

#endif // TRACE_LOADER_HPP
