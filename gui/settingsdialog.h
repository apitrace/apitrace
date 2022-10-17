#pragma once

#include "trace_api.hpp"
#include "apitracefilter.h"
#include "ui_settings.h"
#include <QDialog>
#include <QRegularExpression>


class SettingsDialog : public QDialog, public Ui_Settings
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    void accept() override;

    void setFilterModel(ApiTraceFilter *filter);
private slots:
    void changeRegexp(const QString &name);
    void regexpChanged(const QString &pattern);

private:
    void filtersFromModel(const ApiTraceFilter *model);
    void filtersToModel(ApiTraceFilter *model);
private:
    QMap<QString, QRegularExpression> m_showFilters;
    ApiTraceFilter *m_filter;
};
