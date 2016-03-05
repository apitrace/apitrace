#pragma once

#include "graphing/heatmapview.h"
#include "profiling.h"

/**
 * Data providers for a heatmap based off the trace::Profile call data
 */

class ProfileHeatmapRowIterator : public HeatmapRowIterator {
public:
    ProfileHeatmapRowIterator(const trace::Profile* profile, qint64 start, qint64 end, int steps, bool gpu, int program = -1) :
        m_profile(profile),
        m_step(-1),
        m_stepWidth(1),
        m_stepCount(steps),
        m_index(0),
        m_timeStart(start),
        m_timeEnd(end),
        m_useGpu(gpu),
        m_program(program),
        m_selected(false),
        m_timeSelection(false),
        m_programSelection(false)
    {
        m_timeWidth = m_timeEnd - m_timeStart;
    }

    virtual bool next() override
    {
        unsigned maxIndex = m_program == -1 ? m_profile->calls.size() : m_profile->programs[m_program].calls.size();

        if (m_index >= maxIndex) {
            return false;
        }

        double dtds = m_timeWidth / (double)m_stepCount;

        qint64 heatDuration = 0;
        qint64 programHeatDuration = 0;
        m_heat = 0.0f;
        m_step += m_stepWidth;
        m_stepWidth = 1;

        m_selected = false;

        /* Iterator through calls until step != lastStep */
        for (; m_index < maxIndex; ++m_index)
        {
            const trace::Profile::Call* call;

            if (m_program == -1) {
                call = &m_profile->calls[m_index];
            } else {
                call = &m_profile->calls[ m_profile->programs[m_program].calls[m_index] ];
            }

            qint64 start, duration, end;

            if (m_useGpu) {
                start = call->gpuStart;
                duration = call->gpuDuration;

                if (call->pixels < 0) {
                    continue;
                }
            } else {
                start = call->cpuStart;
                duration = call->cpuDuration;
            }

            end = start + duration;

            if (end < m_timeStart) {
                continue;
            }

            if (start > m_timeEnd) {
                m_index = m_profile->calls.size();
                break;
            }

            double left  = timeToStep(start);
            double right = timeToStep(end);

            int leftStep = left;
            int rightStep = right;

            if (leftStep > m_step) {
                break;
            }

            if (m_programSelection && call->program == m_programSel) {
                m_selected = true;
            }

            if (rightStep - leftStep > 1) {
                m_label = QString::fromStdString(call->name);
                m_step = left;
                m_stepWidth = rightStep - leftStep;
                heatDuration = dtds;
                ++m_index;
                break;
            }

            if (leftStep < m_step) {
                qint64 rightTime = stepToTime(rightStep);
                heatDuration += end - rightTime;

                if (m_programSelection && call->program == m_programSel) {
                    programHeatDuration += end - rightTime;
                }
            } else if (leftStep == rightStep) {
                heatDuration += duration;

                if (m_programSelection && call->program == m_programSel) {
                    programHeatDuration += duration;
                }
            } else if (rightStep - leftStep == 1) {
                qint64 rightTime = stepToTime(rightStep);
                heatDuration += rightTime - start;

                if (m_programSelection && call->program == m_programSel) {
                    programHeatDuration += rightTime - start;
                }

                break;
            }
        }

        m_heat = heatDuration / dtds;
        m_programHeat = programHeatDuration / dtds;

        if (m_timeSelection) {
            qint64 time = stepToTime(m_step);

            if (time >= m_timeSelStart && time <= m_timeSelEnd) {
                m_programHeat = 1.0;
            }
        }

        if (m_programSelection && (m_program == m_programSel || (m_selected && m_stepWidth > 1))) {
            m_programHeat = 1.0;
        }

        if (m_programHeat > 0) {
            m_selected = true;
        }

        return true;
    }

    virtual bool isGpu() const override
    {
        return m_useGpu;
    }

    virtual float heat() const override
    {
        return m_heat;
    }

    virtual float selectedHeat() const override
    {
        return m_programHeat;
    }

    virtual int step() const override
    {
        return m_step;
    }

    virtual int width() const override
    {
        return m_stepWidth;
    }

    virtual QString label() const override
    {
        return m_label;
    }

    void setProgramSelection(int program)
    {
        m_programSelection = true;
        m_programSel = program;
    }

    void setTimeSelection(qint64 start, qint64 end)
    {
        m_timeSelection = true;
        m_timeSelStart = start;
        m_timeSelEnd = end;
    }

private:
    double timeToStep(qint64 time) const
    {
        double pos = time;
        pos -= m_timeStart;
        pos /= m_timeWidth;
        pos *= m_stepCount;
        return pos;
    }

    qint64 stepToTime(int pos) const
    {
        double time = pos;
        time /= m_stepCount;
        time *= m_timeWidth;
        time += m_timeStart;
        return (qint64)time;
    }

private:
    const trace::Profile* m_profile;

    int m_step;
    int m_stepWidth;
    int m_stepCount;

    unsigned m_index;

    float m_heat;

    qint64 m_timeStart;
    qint64 m_timeEnd;
    qint64 m_timeWidth;

    bool m_useGpu;
    int m_program;

    QString m_label;

    bool m_selected;

    bool m_timeSelection;
    qint64 m_timeSelStart;
    qint64 m_timeSelEnd;

    bool m_programSelection;
    int m_programSel;

    float m_programHeat;
};

class ProfileHeatmapDataProvider : public HeatmapDataProvider {
protected:
    enum SelectionType {
        None,
        Time,
        Program
    };

public:
    ProfileHeatmapDataProvider(trace::Profile* profile) :
        m_profile(profile),
        m_selectionState(NULL)
    {
        sortRows();
    }

    virtual qint64 start() const override
    {
        return m_profile->frames.front().cpuStart;
    }

    virtual qint64 end() const override
    {
        return m_profile->frames.back().cpuStart + m_profile->frames.back().cpuDuration;
    }

    virtual unsigned dataRows() const override
    {
        return m_rowPrograms.size();
    }

    virtual QString dataLabel(unsigned row) const override
    {
        if (row >= m_rowPrograms.size()) {
            return QString();
        } else {
            return QString("%1").arg(m_rowPrograms[row]);
        }
    }

    virtual qint64 dataRowAt(unsigned row) const override
    {
        if (row >= m_rowPrograms.size()) {
            return 0;
        } else {
            return m_rowPrograms[row];
        }
    }

    virtual HeatmapRowIterator* dataRowIterator(int row, qint64 start, qint64 end, int steps) const override
    {
        ProfileHeatmapRowIterator* itr = new ProfileHeatmapRowIterator(m_profile, start, end, steps, true, m_rowPrograms[row]);

        if (m_selectionState) {
            if (m_selectionState->type == SelectionState::Horizontal) {
                itr->setTimeSelection(m_selectionState->start, m_selectionState->end);
            } else if (m_selectionState->type == SelectionState::Vertical) {
                itr->setProgramSelection(m_selectionState->start);
            }
        }

        return itr;
    }

    virtual unsigned headerRows() const override
    {
        return 2;
    }

    virtual qint64 headerRowAt(unsigned row) const override
    {
        return row;
    }

    virtual QString headerLabel(unsigned row) const override
    {
        if (row == 0) {
            return "CPU";
        } else if (row == 1) {
            return "GPU";
        } else {
            return QString();
        }
    }

    virtual HeatmapRowIterator* headerRowIterator(int row, qint64 start, qint64 end, int steps) const override
    {
        ProfileHeatmapRowIterator* itr = new ProfileHeatmapRowIterator(m_profile, start, end, steps, row != 0);

        if (m_selectionState) {
            if (m_selectionState->type == SelectionState::Horizontal) {
                itr->setTimeSelection(m_selectionState->start, m_selectionState->end);
            } else if (m_selectionState->type == SelectionState::Vertical) {
                itr->setProgramSelection(m_selectionState->start);
            }
        }

        return itr;
    }

    virtual qint64 dataItemAt(unsigned row, qint64 time) const override
    {
        if (row >= m_rowPrograms.size()) {
            return -1;
        }

        unsigned program = m_rowPrograms[row];

        std::vector<unsigned>::const_iterator item =
                Profiling::binarySearchTimespanIndexed
                    (m_profile->calls, m_profile->programs[program].calls.begin(), m_profile->programs[program].calls.end(), time);

        if (item == m_profile->programs[program].calls.end()) {
            return -1;
        }

        return *item;
    }

    virtual qint64 headerItemAt(unsigned row, qint64 time) const override
    {
        if (row >= m_rowPrograms.size()) {
            return -1;
        }

        if (row == 0) {
            /* CPU */
            std::vector<trace::Profile::Call>::const_iterator item =
                    Profiling::binarySearchTimespan<trace::Profile::Call,
                                        &trace::Profile::Call::cpuStart,
                                        &trace::Profile::Call::cpuDuration>
                    (m_profile->calls.begin(), m_profile->calls.end(), time);

            if (item != m_profile->calls.end()) {
                return item - m_profile->calls.begin();
            }
        } else if (row == 1) {
            /* GPU */
            for (unsigned i = 0; i < m_rowPrograms.size(); ++i) {
                qint64 index = dataItemAt(i, time);

                if (index != -1) {
                    return index;
                }
            }
        }

        return -1;
    }

    virtual void itemDoubleClicked(qint64 index) const override
    {
        if (index < 0 || index >= m_profile->calls.size()) {
            return;
        }

        const trace::Profile::Call& call = m_profile->calls[index];
        Profiling::jumpToCall(call.no);
    }

    virtual QString itemTooltip(qint64 index) const override
    {
        if (index >= m_profile->calls.size()) {
            return QString();
        }

        const trace::Profile::Call& call = m_profile->calls[index];

        QString text;
        text  = QString::fromStdString(call.name);

        text += QString("\nCall: %1").arg(call.no);
        text += QString("\nCPU Start: %1").arg(Profiling::getTimeString(call.cpuStart, 1e3));
        text += QString("\nCPU Duration: %1").arg(Profiling::getTimeString(call.cpuDuration, 1e3));

        if (call.pixels >= 0) {
            text += QString("\nGPU Start: %1").arg(Profiling::getTimeString(call.gpuStart, 1e3));
            text += QString("\nGPU Duration: %1").arg(Profiling::getTimeString(call.gpuDuration, 1e3));
            text += QString("\nPixels Drawn: %1").arg(QLocale::system().toString((qlonglong)call.pixels));
        }

        return text;
    }

    virtual void setSelectionState(SelectionState* state) override
    {
        m_selectionState = state;
    }

private:
    void sortRows()
    {
        typedef QPair<quint64, unsigned> Pair;
        std::vector<Pair> gpu;

        /* Map shader to visible row */
        for (std::vector<trace::Profile::Program>::const_iterator itr = m_profile->programs.begin(); itr != m_profile->programs.end(); ++itr) {
            const trace::Profile::Program& program = *itr;
            unsigned no = itr -  m_profile->programs.begin();

            if (program.gpuTotal > 0) {
                gpu.push_back(Pair(program.gpuTotal, no));
            }
        }

        /* Sort the shaders by most used gpu */
        qSort(gpu);

        /* Create row order */
        m_rowPrograms.clear();

        for (std::vector<Pair>::const_reverse_iterator itr = gpu.rbegin(); itr != gpu.rend(); ++itr) {
            m_rowPrograms.push_back(itr->second);
        }
    }

protected:
    trace::Profile* m_profile;
    std::vector<int> m_rowPrograms;
    SelectionState* m_selectionState;
};
