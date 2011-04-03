#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "apitracefilter.h"
#include "ui_settings.h"
#include <QDialog>


class SettingsDialog : public QDialog, public Ui_Settings
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    void accept();

    void setFilterOptions(ApiTraceFilter::FilterOptions opts);
    ApiTraceFilter::FilterOptions filterOptions() const;

private:
    ApiTraceFilter::FilterOptions m_filterOptions;
};

#endif
