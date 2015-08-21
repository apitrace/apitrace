#include <iostream>

#include "metric_writer.hpp"

void ProfilerQuery::writeMetricHeaderCallback(Metric* metric, int event, void* data, int error,
                                      void* userData) {
    std::cout << "\t" << metric->name();
}

void ProfilerQuery::writeMetricEntryCallback(Metric* metric, int event, void* data, int error,
                                     void* userData) {
    if (error) {
        std::cout << "\t" << "#ERR" << error;
        return;
    }
    if (!data) {
        std::cout << "\t" << "-";
        return;
    }
    switch(metric->numType()) {
        case CNT_NUM_UINT: std::cout << "\t" << *(reinterpret_cast<unsigned*>(data)); break;
        case CNT_NUM_FLOAT: std::cout << "\t" << *(reinterpret_cast<float*>(data)); break;
        case CNT_NUM_DOUBLE: std::cout << "\t" << *(reinterpret_cast<double*>(data)); break;
        case CNT_NUM_BOOL: std::cout << "\t" << *(reinterpret_cast<bool*>(data)); break;
        case CNT_NUM_UINT64: std::cout << "\t" << *(reinterpret_cast<uint64_t*>(data)); break;
        case CNT_NUM_INT64: std::cout << "\t" << *(reinterpret_cast<int64_t*>(data)); break;
    }
}

void ProfilerQuery::writeMetricHeader(QueryBoundary qb) const {
    for (auto &a : *metricBackends) {
        a->enumDataQueryId(eventId, &writeMetricHeaderCallback, qb);
    }
    std::cout << std::endl;
}

void ProfilerQuery::writeMetricEntry(QueryBoundary qb) const {
    for (auto &a : *metricBackends) {
        a->enumDataQueryId(eventId, &writeMetricEntryCallback, qb);
    }
    std::cout << std::endl;
}

template<typename T>
T ProfilerCall::StringTable<T>::getId(const std::string &str) {
    auto res = stringLookupTable.find(str);
    T index;
    if (res == stringLookupTable.end()) {
        index = static_cast<T>(strings.size());
        strings.push_back(str);
        stringLookupTable[str] = index;
    } else {
        index = res->second;
    }
    return index;
}

template<typename T>
std::string ProfilerCall::StringTable<T>::getString(T id) {
    return strings[static_cast<typename decltype(stringLookupTable)::size_type>(id)];
}


ProfilerCall::ProfilerCall(unsigned eventId, const data* queryData)
    : ProfilerQuery(QUERY_BOUNDARY_CALL, eventId)
{
    if (queryData) {
        isFrameEnd = queryData->isFrameEnd;
        no = queryData->no;
        program = queryData->program;
        nameTableEntry = nameTable.getId(queryData->name);
    }
}


void ProfilerCall::writeHeader() const {
    std::cout << "#\tcall no\tprogram\tname";
    ProfilerQuery::writeMetricHeader(QUERY_BOUNDARY_CALL);
}

void ProfilerCall::writeEntry() const {
    if (isFrameEnd) {
        std::cout << "frame_end" << std::endl;
    } else {
        std::cout << "call"
            << "\t" << no
            << "\t" << program
            << "\t" << nameTable.getString(nameTableEntry);
        ProfilerQuery::writeMetricEntry(QUERY_BOUNDARY_CALL);
    }
}


void ProfilerDrawcall::writeHeader() const {
    std::cout << "#\tcall no\tprogram\tname";
    ProfilerQuery::writeMetricHeader(QUERY_BOUNDARY_DRAWCALL);
}

void ProfilerDrawcall::writeEntry() const {
    if (isFrameEnd) {
        std::cout << "frame_end" << std::endl;
    } else {
        std::cout << "call"
            << "\t" << no
            << "\t" << program
            << "\t" << nameTable.getString(nameTableEntry);
        ProfilerQuery::writeMetricEntry(QUERY_BOUNDARY_DRAWCALL);
    }
}


void ProfilerFrame::writeHeader() const {
    std::cout << "#";
    ProfilerQuery::writeMetricHeader(QUERY_BOUNDARY_FRAME);
}

void ProfilerFrame::writeEntry() const {
    std::cout << "frame";
    ProfilerQuery::writeMetricEntry(QUERY_BOUNDARY_FRAME);
}


MetricWriter::MetricWriter(std::vector<MetricBackend*> &metricBackends,
                           const MmapAllocator<char> &alloc)
    : frameQueue(MmapAllocator<ProfilerCall>(alloc)),
      callQueue(MmapAllocator<ProfilerCall>(alloc)),
      drawcallQueue(MmapAllocator<ProfilerCall>(alloc))
{
    ProfilerQuery::metricBackends = &metricBackends;
}

void MetricWriter::addQuery(QueryBoundary boundary, unsigned eventId,
                            const void* queryData)
{
    switch (boundary) {
        case QUERY_BOUNDARY_FRAME:
            frameQueue.emplace_back(eventId);
            break;
        case QUERY_BOUNDARY_CALL:
            callQueue.emplace_back(eventId,
                    reinterpret_cast<const ProfilerCall::data*>(queryData));
            break;
        case QUERY_BOUNDARY_DRAWCALL:
            drawcallQueue.emplace_back(eventId,
                    reinterpret_cast<const ProfilerCall::data*>(queryData));
            break;
        default:
            break;
    }
}

void MetricWriter::writeQuery(QueryBoundary boundary) {
    switch (boundary) {
        case QUERY_BOUNDARY_FRAME:
            frameQueue.front().writeEntry();
            frameQueue.pop_front();
            break;
        case QUERY_BOUNDARY_CALL:
            callQueue.front().writeEntry();
            callQueue.pop_front();
            break;
        case QUERY_BOUNDARY_DRAWCALL:
            drawcallQueue.front().writeEntry();
            drawcallQueue.pop_front();
            break;
        default:
            break;
    }
}

void MetricWriter::writeAll(QueryBoundary boundary) {
    switch (boundary) {
        case QUERY_BOUNDARY_FRAME:
            frameQueue.front().writeHeader();
            while (!frameQueue.empty()) {
                writeQuery(boundary);
            }
            break;
        case QUERY_BOUNDARY_CALL:
            callQueue.front().writeHeader();
            while (!callQueue.empty()) {
                writeQuery(boundary);
            }
            break;
        case QUERY_BOUNDARY_DRAWCALL:
            drawcallQueue.front().writeHeader();
            while (!drawcallQueue.empty()) {
                writeQuery(boundary);
            }
            break;
        default:
            break;
    }
    std::cout << std::endl;
}

std::vector<MetricBackend*>* ProfilerQuery::metricBackends = nullptr;

ProfilerCall::StringTable<int16_t> ProfilerCall::nameTable;
