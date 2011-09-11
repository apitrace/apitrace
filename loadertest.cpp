#include "trace_loader.hpp"
#include "os.hpp"

#include <iostream>


static const double msecsInSec = 1000000;

static void timeFrameFetch(Trace::Loader &loader, unsigned frameIdx)
{
    long long t1, t2;
    std::vector<Trace::Call*> frame;

    t1 = OS::GetTime();
    frame = loader.frame(frameIdx);
    t2 = OS::GetTime();
    std::cout << "Time to fetch the frame["
              << frameIdx
              << "] size "
              << frame.size()
              << " is = "
              << (t2 - t1)/msecsInSec
              << " secs "<<std::endl;

    for (unsigned i = 0; i < frame.size(); ++i) {
        delete frame[i];
    }
}


int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; ++i) {
        Trace::Loader loader;

        long long t1 = OS::GetTime();
        if (!loader.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }
        long long t2 = OS::GetTime();
        std::cout << "Time to scan file = "<< (t2 - t1)/msecsInSec
                  << " secs "<<std::endl;

        std::cout << "Number of frames = "
                  << loader.numberOfFrames()
                  << std::endl;
        std::cout << "Number of calls in frame 0 = "
                  << loader.numberOfCallsInFrame(0)
                  << std::endl;
        int lastFrame = loader.numberOfFrames() - 1;
        std::cout << "Number of calls in frame "
                  << lastFrame << " = "
                  << loader.numberOfCallsInFrame(lastFrame)
                  << std::endl;

        unsigned biggestFrameIdx = 0;
        unsigned maxFrameSize = 0;
        for (unsigned i = 0; i < loader.numberOfFrames(); ++i) {
            if (loader.numberOfCallsInFrame(i) > maxFrameSize) {
                maxFrameSize = loader.numberOfCallsInFrame(i);
                biggestFrameIdx = i;
            }
        }

        timeFrameFetch(loader, loader.numberOfFrames()/2);
        timeFrameFetch(loader, 0);
        timeFrameFetch(loader, loader.numberOfFrames() - 1);
        timeFrameFetch(loader, biggestFrameIdx);
    }

    return 0;
}

