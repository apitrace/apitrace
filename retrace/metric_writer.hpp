#pragma once

#include <queue>
#include <string>
#include <unordered_map>

#include "metric_backend.hpp"
#include "mmap_allocator.hpp"

class ProfilerQuery
{
private:
    unsigned eventId;
    static void writeMetricHeaderCallback(Metric* metric, int event, void* data, int error,
                                  void* userData);
    static void writeMetricEntryCallback(Metric* metric, int event, void* data, int error,
                                 void* userData);

public:
    static std::vector<MetricBackend*>* metricBackends;

    ProfilerQuery(QueryBoundary qb, unsigned eventId)
        : eventId(eventId) {};
    void writeMetricHeader(QueryBoundary qb) const;
    void writeMetricEntry(QueryBoundary qb) const;
};

class ProfilerCall : public ProfilerQuery
{
public:
    struct data {
        bool isFrameEnd;
        unsigned no;
        unsigned program;
        const char* name;
    };

protected:
    template<typename T>
    class StringTable
    {
    private:
        std::deque<std::string> strings;
        std::unordered_map<std::string, T> stringLookupTable;

    public:
        T getId(const std::string &str);
        std::string getString(T id);
    };

    static StringTable<int16_t> nameTable;

    int16_t nameTableEntry;
    bool isFrameEnd;
    unsigned no;
    unsigned program;

public:
    ProfilerCall(unsigned eventId, const data* queryData = nullptr);
    void writeHeader() const;
    void writeEntry() const;
};

class ProfilerDrawcall : public ProfilerCall
{
public:
    ProfilerDrawcall(unsigned eventId, const data* queryData)
        : ProfilerCall( eventId, queryData) {};
    void writeHeader() const;
    void writeEntry() const;
};

class ProfilerFrame : public ProfilerQuery
{
public:
    ProfilerFrame(unsigned eventId)
        : ProfilerQuery(QUERY_BOUNDARY_FRAME, eventId) {};
    void writeHeader() const;
    void writeEntry() const;
};

class MetricWriter
{
private:
    std::deque<ProfilerFrame, MmapAllocator<ProfilerFrame>> frameQueue;
    std::deque<ProfilerCall, MmapAllocator<ProfilerCall>> callQueue;
    std::deque<ProfilerDrawcall, MmapAllocator<ProfilerDrawcall>> drawcallQueue;

public:
    MetricWriter(std::vector<MetricBackend*> &metricBackends,
                 const MmapAllocator<char> &alloc);

    void addQuery(QueryBoundary boundary, unsigned eventId,
                  const void* queryData = nullptr);

    void writeQuery(QueryBoundary boundary);

    void writeAll(QueryBoundary boundary);
};
