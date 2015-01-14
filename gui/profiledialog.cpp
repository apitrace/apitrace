#include "profiledialog.h"
#include "profiletablemodel.h"
#include <QSortFilterProxyModel>

#include "graphing/histogramview.h"
#include "graphing/timeaxiswidget.h"
#include "graphing/frameaxiswidget.h"
#include "graphing/heatmapverticalaxiswidget.h"
#include "profileheatmap.h"

/* Handy function to allow selection of a call in main window */
ProfileDialog* g_profileDialog = 0;

void Profiling::jumpToCall(int index)
{
    if (g_profileDialog) {
        g_profileDialog->showCall(index);
    }
}

/* Provides frame numbers based off call index */
class FrameCallDataProvider : public FrameDataProvider {
public:
    FrameCallDataProvider(const trace::Profile* profile)
    {
        m_profile = profile;
    }

    unsigned size() const {
        return m_profile->frames.size();
    }

    qint64 frameStart(unsigned index) const {
        return m_profile->frames[index].calls.begin;
    }

    qint64 frameEnd(unsigned index) const {
        return m_profile->frames[index].calls.end;
    }

private:
    const trace::Profile* m_profile;
};

/* Provides frame numbers based off time */
class FrameTimeDataProvider : public FrameDataProvider {
public:
    FrameTimeDataProvider(const trace::Profile* profile)
    {
        m_profile = profile;
    }

    unsigned size() const {
        return m_profile->frames.size();
    }

    qint64 frameStart(unsigned index) const {
        return m_profile->frames[index].cpuStart;
    }

    qint64 frameEnd(unsigned index) const {
        return m_profile->frames[index].cpuStart + m_profile->frames[index].cpuDuration;
    }

private:
    const trace::Profile* m_profile;
};

ProfileDialog::ProfileDialog(QWidget *parent)
    : QDialog(parent),
      m_profile(0)
{
    setupUi(this);
    g_profileDialog = this;

    /* Gradients for call duration histograms */
    QLinearGradient cpuGradient;
    cpuGradient.setColorAt(0.9, QColor(0, 0, 210));
    cpuGradient.setColorAt(0.0, QColor(130, 130, 255));

    QLinearGradient gpuGradient;
    gpuGradient.setColorAt(0.9, QColor(210, 0, 0));
    gpuGradient.setColorAt(0.0, QColor(255, 130, 130));


    /* Setup heatmap timeline */
    m_timeline->setLabel(new GraphLabelWidget("Frames ", m_timeline));
    m_timeline->label()->setFlags(Qt::AlignVCenter | Qt::AlignRight);

    m_timeline->setView(new HeatmapView(m_timeline));

    m_timeline->setAxis(GraphWidget::AxisTop, new FrameAxisWidget(m_timeline));
    m_timeline->setAxis(GraphWidget::AxisLeft, new HeatmapVerticalAxisWidget(m_timeline));
    m_timeline->axis(GraphWidget::AxisLeft)->resize(80, 0);

    m_timeline->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_timeline->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    /* Setup Cpu call duration histogram */
    m_cpuGraph->setLabel(new GraphLabelWidget("CPU", m_cpuGraph));

    m_cpuGraph->setAxis(GraphWidget::AxisTop, new FrameAxisWidget(m_cpuGraph));
    m_cpuGraph->setAxis(GraphWidget::AxisLeft, new TimeAxisWidget(m_cpuGraph));
    m_cpuGraph->axis(GraphWidget::AxisLeft)->resize(80, 0);

    HistogramView* cpuView = new HistogramView(m_cpuGraph);
    cpuView->setSelectedGradient(cpuGradient);
    m_cpuGraph->setView(cpuView);


    /* Setup Gpu call duration histogram */
    m_gpuGraph->setLabel(new GraphLabelWidget("GPU", m_gpuGraph));

    m_gpuGraph->setAxis(GraphWidget::AxisTop, new FrameAxisWidget(m_gpuGraph));
    m_gpuGraph->setAxis(GraphWidget::AxisLeft, new TimeAxisWidget(m_gpuGraph));
    m_gpuGraph->axis(GraphWidget::AxisLeft)->resize(80, 0);

    HistogramView* gpuView = new HistogramView(m_gpuGraph);
    gpuView->setSelectedGradient(gpuGradient);
    m_gpuGraph->setView(gpuView);


    /* Synchronise selections */
    connect(m_timeline, SIGNAL(selectionChanged(SelectionState)), m_cpuGraph, SLOT(setSelection(SelectionState)));
    connect(m_timeline, SIGNAL(selectionChanged(SelectionState)), m_gpuGraph, SLOT(setSelection(SelectionState)));

    connect(m_cpuGraph, SIGNAL(selectionChanged(SelectionState)), m_timeline, SLOT(setSelection(SelectionState)));
    connect(m_cpuGraph, SIGNAL(selectionChanged(SelectionState)), m_gpuGraph, SLOT(setSelection(SelectionState)));

    connect(m_gpuGraph, SIGNAL(selectionChanged(SelectionState)), m_timeline, SLOT(setSelection(SelectionState)));
    connect(m_gpuGraph, SIGNAL(selectionChanged(SelectionState)), m_cpuGraph, SLOT(setSelection(SelectionState)));

    connect(m_timeline, SIGNAL(selectionChanged(SelectionState)), this, SLOT(graphSelectionChanged(SelectionState)));
    connect(m_cpuGraph, SIGNAL(selectionChanged(SelectionState)), this, SLOT(graphSelectionChanged(SelectionState)));
    connect(m_gpuGraph, SIGNAL(selectionChanged(SelectionState)), this, SLOT(graphSelectionChanged(SelectionState)));


    /* Synchronise views between cpuGraph and gpuGraph */
    connect(m_cpuGraph, SIGNAL(horizontalViewChanged(qint64,qint64)), m_gpuGraph, SLOT(setHorizontalView(qint64,qint64)));
    connect(m_gpuGraph, SIGNAL(horizontalViewChanged(qint64,qint64)), m_cpuGraph, SLOT(setHorizontalView(qint64,qint64)));
}


ProfileDialog::~ProfileDialog()
{
    delete m_profile;
}


void ProfileDialog::showCall(int call)
{
    emit jumpToCall(call);
}


void ProfileDialog::tableDoubleClicked(const QModelIndex& index)
{
    ProfileTableModel* model = (ProfileTableModel*)m_table->model();

    if (!model) {
        return;
    }

    const trace::Profile::Call* call = model->getJumpCall(index);

    if (call) {
        emit jumpToCall(call->no);
    } else {
        unsigned program = model->getProgram(index);

        SelectionState state;
        state.type = SelectionState::Vertical;
        state.end = state.start = program;

        m_timeline->setSelection(state);
        m_cpuGraph->setSelection(state);
        m_gpuGraph->setSelection(state);
    }
}


void ProfileDialog::setProfile(trace::Profile* profile)
{

    if (profile && profile->frames.size()) {
        HeatmapVerticalAxisWidget* programAxis;
        FrameAxisWidget* frameAxis;
        HistogramView* histogram;
        HeatmapView* heatmap;


        /* Setup data providers for Cpu graph */
        m_cpuGraph->setProfile(profile);
        histogram = (HistogramView*)m_cpuGraph->view();
        frameAxis = (FrameAxisWidget*)m_cpuGraph->axis(GraphWidget::AxisTop);

        histogram->setDataProvider(new CallDurationDataProvider(profile, false));
        frameAxis->setDataProvider(new FrameCallDataProvider(profile));

        /* Setup data provider for Gpu graph */
        m_gpuGraph->setProfile(profile);
        histogram = (HistogramView*)m_gpuGraph->view();
        frameAxis = (FrameAxisWidget*)m_gpuGraph->axis(GraphWidget::AxisTop);

        histogram->setDataProvider(new CallDurationDataProvider(profile, true));
        frameAxis->setDataProvider(new FrameCallDataProvider(profile));

        /* Setup data provider for heatmap timeline */
        heatmap = (HeatmapView*)m_timeline->view();
        frameAxis = (FrameAxisWidget*)m_timeline->axis(GraphWidget::AxisTop);
        programAxis = (HeatmapVerticalAxisWidget*)m_timeline->axis(GraphWidget::AxisLeft);

        heatmap->setDataProvider(new ProfileHeatmapDataProvider(profile));
        frameAxis->setDataProvider(new FrameTimeDataProvider(profile));
        programAxis->setDataProvider(new ProfileHeatmapDataProvider(profile));

        /* Setup data model for table view */
        ProfileTableModel* model = new ProfileTableModel(m_table);
        model->setProfile(profile);

        delete m_table->model();
        m_table->setModel(model);
        m_table->update(QModelIndex());
        m_table->sortByColumn(2, Qt::DescendingOrder);
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        m_table->resizeColumnsToContents();

        /* Reset selection */
        SelectionState emptySelection;
        emptySelection.type = SelectionState::None;
        m_cpuGraph->setSelection(emptySelection);
        m_gpuGraph->setSelection(emptySelection);
        m_timeline->setSelection(emptySelection);
    }

    delete m_profile;
    m_profile = profile;
}


void ProfileDialog::graphSelectionChanged(SelectionState state)
{
    ProfileTableModel* model = (ProfileTableModel*)m_table->model();

    if (!model) {
        return;
    }

    if (state.type == SelectionState::None) {
        model->selectNone();
    } else if (state.type == SelectionState::Horizontal) {
        model->selectTime(state.start, state.end);
    } else if (state.type == SelectionState::Vertical) {
        model->selectProgram(state.start);
    }

    m_table->reset();

    if (state.type == SelectionState::Vertical) {
        m_table->selectRow(model->getRowIndex(state.start));
    }
}

#include "profiledialog.moc"
