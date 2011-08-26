#include "trace_loader.hpp"

#include "trace_snappyfile.hpp"

using namespace Trace;

Loader::Loader()
    : m_frameMarker(FrameMarker_SwapBuffers),
      file(0)
{
}

Loader::~Loader()
{
    close();
}

Loader::FrameMarker Loader::frameMarker() const
{
    return m_frameMarker;
}

void Loader::setFrameMarker(Loader::FrameMarker marker)
{
    m_frameMarker = marker;
}

int Loader::numberOfFrames() const
{
    return m_frameOffsets.size();
}

int Loader::numberOfCallsInFrame(int frameIdx)
{
    if (frameIdx > m_frameOffsets.size()) {
        return 0;
    }

    return m_frameOffsets[frameIdx].numberOfCalls;
}

bool Loader::open(const char *filename)
{
    if (!m_parser.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return false;
    }

    Trace::Call *call;
    File::Offset startOffset;
    int numOfFrames = 0;
    int numOfCalls = 0;

    startOffset = m_parser.currentOffset();

    while ((call = m_parser.parse_call())) {

        ++numOfCalls;

        if (isCallAFrameMarker(call)) {
            File::Offset endOffset = m_parser.currentOffset();
            FrameOffset frameOffset(startOffset, endOffset);

            frameOffset.numberOfCalls = numOfCalls;
            m_frameOffsets[numOfFrames] = frameOffset;
            ++numOfFrames;

            startOffset = endOffset;
            numOfCalls = 0;
        }
        //call->dump(std::cout, color);
        delete call;
    }
    return true;
}

void Loader::close()
{
    m_parser.close();
}

bool Loader::isCallAFrameMarker(const Trace::Call *call) const
{
    std::string name = call->name();

    switch (m_frameMarker) {
    case FrameMarker_SwapBuffers:
        return  name.find("SwapBuffers") != std::string::npos ||
                name == "CGLFlushDrawable" ||
                name == "glFrameTerminatorGREMEDY";
        break;
    case FrameMarker_Flush:
        return name == "glFlush";
        break;
    case FrameMarker_Finish:
        return name == "glFinish";
        break;
    case FrameMarker_Clear:
        return name == "glClear";
        break;
    }
    return false;
}

std::vector<Trace::Call *> Trace::Loader::frame(int idx)
{
    int numOfCalls = numberOfCallsInFrame(idx);
    if (numOfCalls) {
        std::vector<Trace::Call*> calls(numOfCalls);
        m_parser.setCurrentOffset(m_frameOffsets[idx].start);

        Trace::Call *call;
        int parsedCalls = 0;
        while ((call = m_parser.parse_call())) {

            if (isCallAFrameMarker(call)) {
                break;
            }

            calls[parsedCalls] = call;
            ++parsedCalls;
        }
        return calls;
    }
    return std::vector<Trace::Call*>();
}
