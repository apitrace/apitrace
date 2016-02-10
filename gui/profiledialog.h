#pragma once

#include "ui_profiledialog.h"
#include <QDialog>

namespace trace { struct Profile; }

class ProfileDialog : public QDialog, public Ui_ProfileDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget *parent = 0);
    ~ProfileDialog();
    
    void setProfile(trace::Profile* profile);
    void showCall(int call);

public slots:
    void tableDoubleClicked(const QModelIndex& index);
    void graphSelectionChanged(SelectionState state);

signals:
    void jumpToCall(int call);

private:
    trace::Profile *m_profile;
};
