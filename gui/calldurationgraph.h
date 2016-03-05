#pragma once

#include "graphing/graphwidget.h"
#include "trace_profiler.hpp"
#include "profiling.h"

/**
 * Wrapper for call duration graphs.
 *
 * This implements the transformSelectionIn and transformSelectionOut to
 * allow sharing the selection between the graphs and the heatmap as they
 * are using different scales. The duration graphs have call.no on the X-axis
 * whereas the heatmap has time on the X axis.
 */
class CallDurationGraph : public GraphWidget {
public:
    CallDurationGraph(QWidget* parent = 0) :
        GraphWidget(parent),
        m_profile(NULL)
    {
    }

    void setProfile(const trace::Profile* profile)
    {
        m_profile = profile;
    }

protected:
    /* Transform from time-based horizontal selection to call no based. */
    virtual SelectionState transformSelectionIn(SelectionState state) override
    {
        if (!m_profile || state.type != SelectionState::Horizontal) {
            return state;
        }

        qint64 timeStart = state.start;
        qint64 timeEnd = state.end;

        std::vector<trace::Profile::Call>::const_iterator itr;

        itr =
            Profiling::binarySearchTimespan<
                trace::Profile::Call,
                &trace::Profile::Call::cpuStart,
                &trace::Profile::Call::cpuDuration>
            (m_profile->calls.begin(), m_profile->calls.end(), timeStart, true);

        state.start = itr - m_profile->calls.begin();

        itr =
            Profiling::binarySearchTimespan<
                trace::Profile::Call,
                &trace::Profile::Call::cpuStart,
                &trace::Profile::Call::cpuDuration>
            (m_profile->calls.begin(), m_profile->calls.end(), timeEnd, true);

        state.end = itr - m_profile->calls.begin();

        return state;
    }

    virtual SelectionState transformSelectionOut(SelectionState state) override
    {
        if (!m_profile || state.type != SelectionState::Horizontal) {
            return state;
        }

        qint64 start = qMax<qint64>(0, state.start);
        qint64 end = qMin<qint64>(state.end, m_profile->calls.size());

        /* Call based -> time based */
        state.start = m_profile->calls[start].cpuStart;
        state.end = m_profile->calls[end].cpuStart + m_profile->calls[end].cpuDuration;

        return state;
    }

private:
    const trace::Profile* m_profile;
};

/* Data provider for call duration graphs */
class CallDurationDataProvider : public GraphDataProvider {
public:
    CallDurationDataProvider(const trace::Profile* profile, bool gpu) :
        m_gpu(gpu),
        m_profile(profile),
        m_selectionState(NULL)
    {
    }

    virtual qint64 size() const override
    {
        return m_profile ? m_profile->calls.size() : 0;
    }

    virtual bool selected(qint64 index) const override
    {
        if (m_selectionState) {
            if (m_selectionState->type == SelectionState::Horizontal) {
                if (m_selectionState->start <= index && index < m_selectionState->end) {
                    return true;
                }
            } else if (m_selectionState->type == SelectionState::Vertical) {
                return m_profile->calls[index].program == m_selectionState->start;
            }
        }

        return false;
    }

    virtual void setSelectionState(SelectionState* state) override
    {
        m_selectionState = state;
    }

    virtual qint64 value(qint64 index) const override
    {
        if (m_gpu) {
            return m_profile->calls[index].gpuDuration;
        } else {
            return m_profile->calls[index].cpuDuration;
        }
    }

    virtual void itemDoubleClicked(qint64 index) const override
    {
        if (!m_profile) {
            return;
        }

        if (index < 0 || index >= m_profile->calls.size()) {
            return;
        }

        const trace::Profile::Call& call = m_profile->calls[index];
        Profiling::jumpToCall(call.no);
    }

    virtual QString itemTooltip(qint64 index) const override
    {
        if (!m_profile) {
            return QString();
        }

        if (index < 0 || index >= m_profile->calls.size()) {
            return QString();
        }

        const trace::Profile::Call& call = m_profile->calls[index];

        QString text;
        text  = QString::fromStdString(call.name);
        text += QString("\nCall: %1").arg(call.no);
        text += QString("\nCPU Duration: %1").arg(Profiling::getTimeString(call.cpuDuration));

        if (call.pixels >= 0) {
            text += QString("\nGPU Duration: %1").arg(Profiling::getTimeString(call.gpuDuration));
            text += QString("\nPixels Drawn: %1").arg(QLocale::system().toString((qlonglong)call.pixels));
            text += QString("\nProgram: %1").arg(call.program);
        }

        return text;
    }

private:
    bool m_gpu;
    const trace::Profile* m_profile;
    SelectionState* m_selectionState;
};

