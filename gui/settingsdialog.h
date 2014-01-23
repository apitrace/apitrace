#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "trace_api.hpp"
#include "apitracefilter.h"
#include "ui_settings.h"
#include <QDialog>
#include <QRegExp>


class SettingsDialog : public QDialog, public Ui_Settings
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    void accept();
    void settingsUpdate();

    void setFilterModel(ApiTraceFilter *filter);
private slots:
    void changeRegexp(const QString &name);
    void regexpChanged(const QString &pattern);
    void apply();

private:
    void filtersFromModel(const ApiTraceFilter *model);
    void filtersToModel(ApiTraceFilter *model);
private:
    QMap<QString, QRegExp> m_showFilters;
    ApiTraceFilter *m_filter;
};

#endif
