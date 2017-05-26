/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <string.h>
#include <d3d9.h>
#include <sstream>

#include "os_string.hpp"
#include "os_time.hpp"
#include "os_memory.hpp"

#include "d3dstate.hpp"
#include "retrace.hpp"
#include "d3dretrace.hpp"
#include "highlight.hpp"

namespace d3dretrace {

enum {
    GPU_START = 0,
    GPU_END,
    OCCLUSION,
    FREQ,
    NUM_QUERIES,
};

enum debuglevels {
    INFO = 0,
    WARN,
    ERR,
    CRITICAL,
    NUM_DEBUGLEVELS,
};

struct CallQuery
{
    IDirect3DQuery9 *ids[NUM_QUERIES];
    unsigned call;
    bool isDraw;
    const trace::FunctionSig *sig;
    int64_t cpuStart;
    int64_t cpuEnd;
    int64_t vsizeStart;
    int64_t vsizeEnd;
    int64_t rssStart;
    int64_t rssEnd;
};

static bool supportsTimestamp;
static bool supportsOcclusion;
static std::list<CallQuery> callQueries;
static IDirect3DDevice9 *dev;

static void
debugLog(enum debuglevels severity, const char* message) {
    /* Ignore application messages while dumping state. */
    if (retrace::dumpingState) {
        return;
    }

    const highlight::Highlighter & highlighter = highlight::defaultHighlighter(std::cerr);

    const char *severityStr = "";
    const highlight::Attribute * color = &highlighter.normal();

    switch (severity) {
    case ERR:
        color = &highlighter.color(highlight::RED);
        severityStr = " ERR";
        break;
    case CRITICAL:
        color = &highlighter.color(highlight::RED);
        severityStr = " CRITICAL";
        break;
    case WARN:
        color = &highlighter.color(highlight::GREEN);
        severityStr = " WARN";
        break;
    case INFO:
        color = &highlighter.color(highlight::GRAY);
        severityStr = " INFO";
        break;
    default:
        assert(0);
    }

    std::cerr << *color << retrace::callNo << ": message:" << severityStr << ": ";

    std::cerr << message;

    std::cerr << highlighter.normal();

    // Write new line if not included in the message already.
    size_t messageLen = strlen(message);
    if (!messageLen ||
        (message[messageLen - 1] != '\n' &&
         message[messageLen - 1] != '\r')) {
       std::cerr << std::endl;
    }
}

static void
completeCallQuery(CallQuery& query) {
    /* Get call start and duration */
    int64_t gpuStart = 0, gpuEnd = 0, gpuDuration = 0, gpuFreq = 0, cpuDuration = 0, pixels = 0, vsizeDuration = 0, rssDuration = 0;

    if (query.isDraw) {
        if (retrace::profilingGpuTimes && supportsTimestamp) {
            while(query.ids[GPU_START]->GetData(&gpuStart, sizeof(UINT64), 0) != S_OK) {}
            while(query.ids[GPU_END]->GetData(&gpuEnd, sizeof(UINT64), 0) != S_OK) {}
            while(query.ids[FREQ]->GetData(&gpuFreq, sizeof(UINT64), 0) != S_OK) {}
            query.ids[GPU_START]->Release();
            query.ids[GPU_END]->Release();
            query.ids[FREQ]->Release();

            if (!gpuFreq) {
                debugLog(WARN, "GPU bug: TIMESTAMPFREQ returned 0.");
                gpuFreq = 1000000000ULL;
            }
            gpuStart = gpuStart * 1000000000ULL / gpuFreq;
            gpuDuration = (gpuEnd - gpuStart) * 1000000000ULL / gpuFreq;
        }

        if (retrace::profilingPixelsDrawn && supportsOcclusion) {
            while(query.ids[OCCLUSION]->GetData(&pixels, sizeof(UINT64), 0) != S_OK) {}
            query.ids[OCCLUSION]->Release();
        }
    } else {
        pixels = -1;
    }

    if (retrace::profilingCpuTimes) {
        double cpuTimeScale = 1.0E9 / os::timeFrequency;
        cpuDuration = (query.cpuEnd - query.cpuStart) * cpuTimeScale;
        query.cpuStart *= cpuTimeScale;
    }

    if (retrace::profilingMemoryUsage) {
        vsizeDuration = query.vsizeEnd - query.vsizeStart;
        rssDuration = query.rssEnd - query.rssStart;
    }

    /* Add call to profile */
    retrace::profiler.addCall(query.call, query.sig->name, 0, pixels, gpuStart, gpuDuration, query.cpuStart, cpuDuration, query.vsizeStart, vsizeDuration, query.rssStart, rssDuration);
}

void
flushQueries() {
    for (auto & callQuerie : callQueries) {
        completeCallQuery(callQuerie);
    }

    callQueries.clear();
}

void
setProfileDevice(IDirect3DDevice9 *new_dev) {
    HRESULT hr;

    if (dev && new_dev != dev && retrace::profiling) {
        /* Complete any remaining queries */
        flushQueries();
    }

    dev = new_dev;
    if (!dev)
        return;

    hr = dev->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, NULL);
    if (hr == D3D_OK) {
        supportsTimestamp = 1;
    } else {
        debugLog(ERR, "GPU doesn't support timestamp queries.");
        supportsTimestamp = 0;
    }

    hr = dev->CreateQuery(D3DQUERYTYPE_OCCLUSION, NULL);
    if (hr == D3D_OK) {
        supportsOcclusion = 1;
    } else {
        debugLog(ERR, "GPU doesn't support occlusion queries.");
        supportsOcclusion = 0;
    }
}

void
beginProfile(trace::Call &call, bool isDraw) {
    /* Create call query */
    CallQuery query;

    if (!dev) {
        debugLog(CRITICAL, "Internal Error: No device bound.");
        return;
    }

    query.isDraw = isDraw;
    query.call = call.no;
    query.sig = call.sig;

    /* GPU profiling only for draw calls */
    if (isDraw) {
        if (retrace::profilingGpuTimes && supportsTimestamp) {
                dev->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &query.ids[GPU_START]);
                dev->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &query.ids[GPU_END]);
                dev->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &query.ids[FREQ]);
                query.ids[GPU_START]->Issue(D3DISSUE_END);
                query.ids[FREQ]->Issue(D3DISSUE_END);
        }

        if (retrace::profilingPixelsDrawn && supportsOcclusion) {
            dev->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query.ids[OCCLUSION]);
            query.ids[OCCLUSION]->Issue(D3DISSUE_BEGIN);
        }
    }

    callQueries.push_back(query);

    /* CPU profiling for all calls */
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.cpuStart = os::getTime();
    }

    if (retrace::profilingMemoryUsage) {
        CallQuery& query = callQueries.back();
        query.vsizeStart = os::getVsize();
        query.rssStart = os::getRss();
    }
}

void
endProfile(trace::Call &call, bool isDraw) {
    if (!dev) {
        debugLog(CRITICAL, "Internal Error: No device bound.");
        return;
    }
    /* CPU profiling for all calls */
    if (retrace::profilingCpuTimes) {
        CallQuery& query = callQueries.back();
        query.cpuEnd = os::getTime();
    }

    /* GPU profiling only for draw calls */
    if (isDraw) {
        CallQuery& query = callQueries.back();

        if (retrace::profilingGpuTimes && supportsTimestamp) {
            query.ids[GPU_END]->Issue(D3DISSUE_END);
        }

        if (retrace::profilingPixelsDrawn && supportsOcclusion) {
            query.ids[OCCLUSION]->Issue(D3DISSUE_END);
        }
    }

    if (retrace::profilingMemoryUsage) {
        CallQuery& query = callQueries.back();
        query.vsizeEnd = os::getVsize();
        query.rssEnd = os::getRss();
    }
}

void
frame_complete(trace::Call &call) {
    if (retrace::profiling) {
        /* Complete any remaining queries */
        flushQueries();

        /* Indicate end of current frame */
        retrace::profiler.addFrameEnd();
    }

    retrace::frameComplete(call);
}

}

void
retrace::setFeatureLevel(const char *featureLevel) {
    /* TODO: Allow to override D3D feature level. */
}


void
retrace::setUp(void) {
}


void
retrace::addCallbacks(retrace::Retracer &retracer)
{
    retracer.addCallbacks(d3dretrace::ddraw_callbacks);
    retracer.addCallbacks(d3dretrace::d3d8_callbacks);
    retracer.addCallbacks(d3dretrace::d3d9_callbacks);
#ifdef HAVE_DXGI
    retracer.addCallbacks(d3dretrace::dxgi_callbacks);
#endif
}

void
retrace::flushRendering(void) {
}

void
retrace::finishRendering(void) {
    if (retrace::profiling) {
        /* Complete any remaining queries */
        d3dretrace::flushQueries();
    }
}

void
retrace::waitForInput(void) {
    /* TODO */
}

void
retrace::cleanUp(void) {
}
