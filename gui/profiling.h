#pragma once

#include <QString>
#include <QLocale>
#include "trace_profiler.hpp"

class Profiling {
public:
    /**
     * Select and show the call in main window.
     */
    static void jumpToCall(int index);

    /**
     * Convert a CPU / GPU time to a textual representation.
     * This includes automatic unit selection.
     */
    static QString getTimeString(int64_t time, int64_t unitTime = 0)
    {
        QString text;
        QString unit = " ns";
        double unitScale = 1;

        if (unitTime == 0) {
            unitTime = time;
        }

        if (unitTime >= 60e9) {
            int64_t mins = time / 60e9;
            text += QString("%1 m ").arg(mins);

            time -= mins * 60e9;
            unit = " s";
            unitScale = 1e9;
        } else if (unitTime >= 1e9) {
            unit = " s";
            unitScale = 1e9;
        } else if (unitTime >= 1e6) {
            unit = " ms";
            unitScale = 1e6;
        } else if (unitTime >= 1e3) {
            unit = QString::fromUtf8(" µs");
            unitScale = 1e3;
        }

        /* 3 decimal places */
        text += QString("%1").arg(time / unitScale, 0, 'f', 3);

        /* Remove trailing 0 */
        while(text.endsWith('0'))
            text.truncate(text.length() - 1);

        /* Remove trailing decimal point */
        if (text.endsWith(QLocale::system().decimalPoint()))
            text.truncate(text.length() - 1);

        return text + unit;
    }

    template<typename val_ty, int64_t val_ty::* mem_ptr_start, int64_t val_ty::* mem_ptr_dura>
    static typename std::vector<val_ty>::const_iterator binarySearchTimespan(
            typename std::vector<val_ty>::const_iterator begin,
            typename std::vector<val_ty>::const_iterator end,
            int64_t time,
            bool nearest = false)
    {
        int lower = 0;
        int upper = end - begin;
        int pos = (lower + upper) / 2;
        typename std::vector<val_ty>::const_iterator itr = begin + pos;

        while (!((*itr).*mem_ptr_start <= time && (*itr).*mem_ptr_start + (*itr).*mem_ptr_dura > time) && (lower <= upper)) {
            if ((*itr).*mem_ptr_start > time) {
                upper = pos - 1;
            } else {
                lower = pos + 1;
            }

            pos = (lower + upper) / 2;
            itr = begin + pos;
        }

        if (nearest || lower <= upper) {
            return itr;
        } else {
            return end;
        }
    }

    static std::vector<unsigned>::const_iterator binarySearchTimespanIndexed(
            const std::vector<trace::Profile::Call>& calls,
            std::vector<unsigned>::const_iterator begin,
            std::vector<unsigned>::const_iterator end,
            int64_t time,
            bool nearest = false)
    {
        int lower = 0;
        int upper = end - begin - 1;
        int pos = (lower + upper) / 2;
        std::vector<unsigned>::const_iterator itr = begin + pos;

        while (lower <= upper) {
            const trace::Profile::Call& call = calls[*itr];

            if (call.gpuStart <= time && call.gpuStart + call.gpuDuration > time) {
                break;
            }

            if (call.gpuStart > time) {
                upper = pos - 1;
            } else {
                lower = pos + 1;
            }

            pos = (lower + upper) / 2;
            itr = begin + pos;
        }

        if (nearest || lower <= upper) {
            return itr;
        } else {
            return end;
        }
    }
};
