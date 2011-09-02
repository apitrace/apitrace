#include "trace_loader.hpp"
#include "os.hpp"

#include <iostream>

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; ++i) {
        Trace::Loader loader;
        const double msecsInSec = 1000000;

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

        t1 = OS::GetTime();
        std::vector<Trace::Call*> frame = loader.frame(
            loader.numberOfFrames()/2);
        t2 = OS::GetTime();
        std::cout << "Time to fetch a frame size "
                  << frame.size()
                  << " is = "
                  << (t2 - t1)/msecsInSec
                  << " secs "<<std::endl;

        t1 = OS::GetTime();
        frame = loader.frame(
            0);
        t2 = OS::GetTime();
        std::cout << "Time to fetch a frame size "
                  << frame.size()
                  << " is = "
                  << (t2 - t1)/msecsInSec
                  << " secs "<<std::endl;

        t1 = OS::GetTime();
        frame = loader.frame(loader.numberOfFrames() - 1);
        t2 = OS::GetTime();
        std::cout << "Time to fetch a frame size "
                  << frame.size()
                  << " is = "
                  << (t2 - t1)/msecsInSec
                  << " secs "<<std::endl;

    }

    return 0;
}

