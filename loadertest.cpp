#include "trace_loader.hpp"

#include <iostream>

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; ++i) {
        Trace::Loader loader;

        if (!loader.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

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


        std::vector<Trace::Call*> frame = loader.frame(
                 0);
        std::vector<Trace::Call*>::const_iterator itr;
        for (itr = frame.begin(); itr != frame.end(); ++itr) {
           (*itr)->dump(std::cout, true);
        }
    }


    return 0;
}

