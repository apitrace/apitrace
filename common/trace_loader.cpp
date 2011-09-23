#include "trace_loader.hpp"


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
    return m_frameBookmarks.size();
}

int Loader::numberOfCallsInFrame(int frameIdx) const
{
    if (frameIdx > m_frameBookmarks.size()) {
        return 0;
    }
    FrameBookmarks::const_iterator itr =
        m_frameBookmarks.find(frameIdx);
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
    ParseBookmark startBookmark;
    int numOfFrames = 0;
    int numOfCalls = 0;
    int lastPercentReport = 0;

    m_parser.getBookmark(startBookmark);

    while ((call = m_parser.scan_call())) {
        ++numOfCalls;

        if (isCallAFrameMarker(call)) {
            FrameBookmark frameBookmark(startBookmark);
            frameBookmark.numberOfCalls = numOfCalls;

            m_frameBookmarks[numOfFrames] = frameBookmark;
            ++numOfFrames;

            if (m_parser.percentRead() - lastPercentReport >= 5) {
                std::cerr << "\tPercent scanned = "
                          << m_parser.percentRead()
                          << "..."<<std::endl;
                lastPercentReport = m_parser.percentRead();
            }
            
            m_parser.getBookmark(startBookmark);
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
        const FrameBookmark &frameBookmark = m_frameBookmarks[idx];
        std::vector<Trace::Call*> calls(numOfCalls);
        m_parser.setBookmark(frameBookmark.start);

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
