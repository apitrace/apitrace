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
    int numberOfCallsInFrame(int frameIdx) const;

    bool open(const char *filename);
    void close();

    std::vector<Trace::Call*> frame(int idx);

private:
    struct FrameBookmark {
        FrameBookmark()
            : numberOfCalls(0)
        {}
        FrameBookmark(const ParseBookmark &s)
            : start(s),
              numberOfCalls(0)
        {}

        ParseBookmark start;
        int numberOfCalls;
    };
    bool isCallAFrameMarker(const Trace::Call *call) const;

private:
    Trace::Parser m_parser;
    FrameMarker m_frameMarker;

    typedef std::map<int, FrameBookmark> FrameBookmarks;
    FrameBookmarks m_frameBookmarks;
};

}

#endif // TRACE_LOADER_HPP
