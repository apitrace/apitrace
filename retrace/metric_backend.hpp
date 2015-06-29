/**************************************************************************
 *
 * Copyright 2015 Alexander Trukhin
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

#pragma once

#include <memory>
#include <string>

/**
 * Profiling boundary.
 */
enum QueryBoundary {
    QUERY_BOUNDARY_DRAWCALL = 0, /**< draw call boundary */
    QUERY_BOUNDARY_FRAME,        /**< frame boundary */
    QUERY_BOUNDARY_CALL,         /**< any call boundary */
    QUERY_BOUNDARY_LIST_END
};

/**
 * Numeric type of the metric.
 */
enum MetricNumType {
    CNT_NUM_UINT = 0,
    CNT_NUM_FLOAT,
    CNT_NUM_UINT64,
    CNT_NUM_DOUBLE,
    CNT_NUM_BOOL,
    CNT_NUM_INT64
};

/**
 * Type of data metric represents.
 */
enum MetricType {
    CNT_TYPE_GENERIC = 0, /**< generally a number, comparable type */
    CNT_TYPE_NUM,         /**< number, not necessarily comparable (e.g. event number) */
    CNT_TYPE_DURATION,    /**< duration */
    CNT_TYPE_PERCENT,     /**< percentage */
    CNT_TYPE_TIMESTAMP,   /**< timestamp (e.g. GL_TIMESTAMP in OpenGL) */
    CNT_TYPE_OTHER        /**< not listed above */
};


/**
 * Metric interface.
 *
 * Each metric can be uniquely identified by a group id and a metric id.
 * Each backend is assumed to implement its own version of Metric. It is also
 * supposed to provide metric groups and corresponding metrics.
 * This interface is used to communicate with backend and can be used
 * to store internal data in backend.
 *
 * It is generally a good idea to cache some parameters (e.g. numeric type).
 */
class Metric
{
public:
    virtual ~Metric() {}

    /**
     * Returns metric id
     */
    virtual unsigned id() = 0;

    /**
     * Returns metric group id
     */
    virtual unsigned groupId() = 0;

    /**
     * Returns metric name string
     */
    virtual std::string name() = 0;

    /**
     * Returns metric description string (or an empty string if not available)
     */
    virtual std::string description() = 0;

    /**
     * Returns metric numeric type
     */
    virtual MetricNumType numType() = 0;

    /**
     * Returns data type metric represents
     */
    virtual MetricType type() = 0;
};

/**
 * Callbacks for use in backend interface.
 * int error      : error code (0 - no error)
 * void* userData : arbitrary pointer
 */
typedef void (*enumGroupsCallback)(unsigned group, int error, void* userData);
typedef void (*enumMetricsCallback)(Metric* metric, int error, void* userData);
typedef void (*enumDataCallback)(Metric* metric, int event, void* data,
                                 int error, void* userData);

/**
 * Backend interface.
 *
 * Abstraction for metric-collection system.
 * Such system is supposed to have its own version of MetricBackend.
 * Backend can be used to query available metrics, to profile calls/frames and
 * to collect metrics.
 * Backend is responsible for storing metric data.
 *
 * Unfortunately, not all collection systems allow to collect all metrics
 * at the same time. Therefore multiple passes are needed, this interface provides
 * the mean to implement such behaviour.
 *
 *
 * Typical workflow example:
 * MetricBackend* backend;
 * backend->enableMetric(...);
 * ...
 * backend->enableMetric(...);
 * for (i=0; i < backend->generatePasses(); i++) {
 *     backend->beginPass();
 *
 *     backend->beginQuery(QUERY_BOUNDARY_FRAME);
 *
 *     backend->beginQuery(QUERY_BOUNDARY_CALL or QUERY_BOUNDARY_DRAWCALL);
 *     ... profiled call ...
 *     backend->endQuery(QUERY_BOUNDARY_CALL or QUERY_BOUNDARY_DRAWCALL);
 *
 *     ...
 *
 *     backend->beginQuery(QUERY_BOUNDARY_CALL or QUERY_BOUNDARY_DRAWCALL);
 *     ... profiled call ...
 *     backend->endQuery(QUERY_BOUNDARY_CALL or QUERY_BOUNDARY_DRAWCALL);
 *
 *     backend->endQuery(QUERY_BOUNDARY_FRAME);
 *
 *     ... following frames ...
 *
 *     backend->endPass();
 * }
 * // collect data
 *
 *
 * It is generally a good idea to implement MetricBackend as a singleton.
 */
class MetricBackend
{
public:
    virtual ~MetricBackend() {}

    /**
     * Returns true if MetricBackend is supported on current HW.
     */
    virtual bool isSupported() = 0;

    /**
     * Enumerates metric groups, calls callback for each group.
     */
    virtual void enumGroups(enumGroupsCallback callback,
                            void* userData = nullptr) = 0;

    /**
     * Enumerates metrics in specified group, calls callback for each metric.
     * Callback receives pointer to the metric object among other params.
     * Metric object is an object of class derived from Metric.
     */
    virtual void enumMetrics(unsigned group, enumMetricsCallback callback,
                             void* userData = nullptr) = 0;

    /**
     * Returns group name string (or an empty string if not available).
     */
    virtual std::string getGroupName(unsigned group) = 0;

    /**
     * Returns pointer to the metric object with given group id, metric id.
     * Metric object is an object of class derived from Metric.
     */
    virtual std::unique_ptr<Metric> getMetricById(unsigned groupId, unsigned metricId) = 0;

    /**
     * Returns pointer to the metric object with given metric name string.
     * Metric object is an object of class derived from Metric.
     */
    virtual std::unique_ptr<Metric> getMetricByName(std::string metricName) = 0;

    /**
     * Adds given metric object to the internal list of metrics
     * to be profiled.
     * pollingRule sets the boundary for collecting metric
     * Returns error code (0 - no error).
     */
    virtual int enableMetric(Metric* metric, QueryBoundary pollingRule = QUERY_BOUNDARY_DRAWCALL) = 0;

    /**
     * Generates passes based on enabled metrics.
     * Returns number of generated passes.
     */
    virtual unsigned generatePasses() = 0;

    /**
     * Begins pass. Subsequent calls begin next passes.
     * A pass needs to be ended before starting a new one.
     */
    virtual void beginPass() = 0;

    /**
     * Ends pass.
     */
    virtual void endPass() = 0;

    /**
     * Begins query (profiles unit, i.e. frames or calls). Subsequent calls
     * begin next queries.
     * Parameter boundary should be set to the type of boundary beginQuery/endQuery
     * constructions enclose.
     * A query needs to be ended before starting a new one.
     */
    virtual void beginQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL) = 0;

    /**
     * Ends query.
     * Parameter boundary should be set to the type of boundary beginQuery/endQuery
     * constructions enclose.
     */
    virtual void endQuery(QueryBoundary boundary = QUERY_BOUNDARY_DRAWCALL) = 0;

    /**
     * Enumerates collected metrics data for a given query id and given
     * type of boundary.
     * Query ids begin with 0 for first query.
     * Metric data is passed to callback.
     *
     * The order in which metrics are returned can differ from the one in which
     * metrics were enabled (via enableMetric(...)) . However, it should be
     * guaranteed that order is the same for every query.
     */
    virtual void enumDataQueryId(unsigned id, enumDataCallback callback,
                                 QueryBoundary boundary,
                                 void* userData = nullptr) = 0;

    /**
     * Returns number of passes generated by generatePasses(...).
     * If generatePasses(...) was not called returns 1.
     */
    virtual unsigned getNumPasses() = 0;

};
