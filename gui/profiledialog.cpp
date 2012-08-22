#include "profiledialog.h"
#include "profiletablemodel.h"
#include <QSortFilterProxyModel>

ProfileDialog::ProfileDialog(QWidget *parent)
    : QDialog(parent),
      m_profile(0)
{
    setupUi(this);

    connect(m_timeline, SIGNAL(jumpToCall(int)), SIGNAL(jumpToCall(int)));
    connect(m_timeline, SIGNAL(selectionChanged(int64_t,int64_t)), SLOT(selectionChanged(int64_t,int64_t)));
}


ProfileDialog::~ProfileDialog()
{
    delete m_profile;
}


void ProfileDialog::tableDoubleClicked(const QModelIndex& index)
{
    ProfileTableModel* model = (ProfileTableModel*)m_table->model();
    const trace::Profile::Call* call = model->getJumpCall(index);

    if (call) {
        emit jumpToCall(call->no);
    }
}


void ProfileDialog::setProfile(trace::Profile* profile)
{
    if (m_profile) {
        delete m_profile;
    }

    m_profile = profile;
    m_timeline->setProfile(m_profile);

    ProfileTableModel* model = new ProfileTableModel(m_table);
    model->setProfile(m_profile);

    delete m_table->model();
    m_table->setModel(model);
    m_table->resizeColumnsToContents();
    m_table->sortByColumn(2, Qt::DescendingOrder);
}


void ProfileDialog::selectionChanged(int64_t start, int64_t end)
{
    ProfileTableModel* model = (ProfileTableModel*)m_table->model();
    model->setTimeSelection(start, end);
    m_table->reset();
}


void ProfileDialog::setVerticalScrollMax(int max)
{
    if (max <= 0) {
        m_verticalScrollBar->hide();
    } else {
        m_verticalScrollBar->show();
        m_verticalScrollBar->setMinimum(0);
        m_verticalScrollBar->setMaximum(max);
    }
}


void ProfileDialog::setHorizontalScrollMax(int max)
{
    if (max <= 0) {
        m_horizontalScrollBar->hide();
    } else {
        m_horizontalScrollBar->show();
        m_horizontalScrollBar->setMinimum(0);
        m_horizontalScrollBar->setMaximum(max);
    }
}

#include "profiledialog.moc"
