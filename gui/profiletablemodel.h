#pragma once

#include <QAbstractTableModel>
#include "trace_profiler.hpp"

struct ProfileTableRow
{
    ProfileTableRow(unsigned no)
        : program(no),
          uses(0),
          gpuTime(0),
          cpuTime(0),
          pixels(0),
          longestGpu(0),
          longestCpu(0),
          longestPixel(0)
    {
    }

    unsigned program;
    qulonglong uses;
    qulonglong gpuTime;
    qulonglong cpuTime;
    qulonglong pixels;

    const trace::Profile::Call* longestGpu;
    const trace::Profile::Call* longestCpu;
    const trace::Profile::Call* longestPixel;
};

class ProfileTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ProfileTableModel(QObject *parent = NULL);

    void setProfile(trace::Profile* profile);

    void selectNone();
    void selectProgram(unsigned program);
    void selectTime(int64_t start, int64_t end);

    int getRowIndex(unsigned program) const;
    unsigned getProgram(const QModelIndex & index) const;
    const trace::Profile::Call* getJumpCall(const QModelIndex & index) const;

    virtual int rowCount(const QModelIndex & parent) const override;
    virtual int columnCount(const QModelIndex & parent) const override;

    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    void updateModel();
    ProfileTableRow* getRow(unsigned program);

private:
    QList<ProfileTableRow> m_rowData;
    trace::Profile *m_profile;
    int64_t m_timeMin;
    int64_t m_timeMax;
    int m_sortColumn;
    Qt::SortOrder m_sortOrder;
};

