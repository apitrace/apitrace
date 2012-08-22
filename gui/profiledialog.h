#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

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

public slots:
    void setVerticalScrollMax(int max);
    void setHorizontalScrollMax(int max);

    void tableDoubleClicked(const QModelIndex& index);

    void selectionChanged(int64_t start, int64_t end);

signals:
    void jumpToCall(int call);

private:
    trace::Profile *m_profile;
};

QString getTimeString(int64_t time, int64_t unitTime = 0);

#endif
