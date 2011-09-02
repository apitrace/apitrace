#include "trace_loader.hpp"

#include "trace_snappyfile.hpp"

using namespace Trace;

Loader::Loader()
    : m_frameMarker(FrameMarker_SwapBuffers)
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

int Loader::numberOfCallsInFrame(int frameIdx) const
{
    if (frameIdx > m_frameOffsets.size()) {
        return 0;
    }
    FrameOffsets::const_iterator itr =
        m_frameOffsets.find(frameIdx);
    return itr->second.numberOfCalls;
}

bool Loader::open(const char *filename)
{
    if (!m_parser.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return false;
    }
    if (!m_parser.supportsOffsets()) {
        std::cerr << "error: " <<filename<< " doesn't support seeking "
                  << "\n";
        return false;
    }

    Trace::Call *call;
    File::Offset startOffset;
    int numOfFrames = 0;
    int numOfCalls = 0;
    unsigned callNum = 0;

    startOffset = m_parser.currentOffset();
    callNum = m_parser.currentCallNumber();

    while ((call = m_parser.scan_call())) {
        ++numOfCalls;

        if (isCallAFrameMarker(call)) {
            File::Offset endOffset = m_parser.currentOffset();
            FrameOffset frameOffset(startOffset);
            frameOffset.numberOfCalls = numOfCalls;
            frameOffset.callNumber = callNum;

            m_frameOffsets[numOfFrames] = frameOffset;
            ++numOfFrames;

            startOffset = endOffset;
            callNum = m_parser.currentCallNumber();
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

std::vector<Trace::Call *> Loader::frame(int idx)
{
    int numOfCalls = numberOfCallsInFrame(idx);
    if (numOfCalls) {
        const FrameOffset &frameOffset = m_frameOffsets[idx];
        std::vector<Trace::Call*> calls(numOfCalls);
        m_parser.setCurrentOffset(frameOffset.start);
        m_parser.setCurrentCallNumber(frameOffset.callNumber);

        Trace::Call *call;
        int parsedCalls = 0;
        while ((call = m_parser.parse_call())) {

            calls[parsedCalls] = call;
            ++parsedCalls;

            if (isCallAFrameMarker(call)) {
                break;
            }

        }
        assert(parsedCalls == numOfCalls);
        return calls;
    }
    return std::vector<Trace::Call*>();
}
