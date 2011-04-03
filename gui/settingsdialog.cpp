#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

void SettingsDialog::accept()
{
    m_filterOptions = ApiTraceFilter::NullFilter;
    if (extensionsBox->isChecked())
        m_filterOptions |= ApiTraceFilter::ExtensionsFilter;
    if (functionsBox->isChecked())
        m_filterOptions |= ApiTraceFilter::ResolutionsFilter;
    if (errorsBox->isChecked())
        m_filterOptions |= ApiTraceFilter::ErrorsQueryFilter;
    if (statesBox->isChecked())
        m_filterOptions |= ApiTraceFilter::ExtraStateFilter;
    QDialog::accept();
}

void SettingsDialog::setFilterOptions(ApiTraceFilter::FilterOptions opts)
{
    m_filterOptions = opts;
    extensionsBox->setChecked(m_filterOptions & ApiTraceFilter::ExtensionsFilter);
    functionsBox->setChecked(m_filterOptions & ApiTraceFilter::ResolutionsFilter);
    errorsBox->setChecked(m_filterOptions & ApiTraceFilter::ErrorsQueryFilter);
    statesBox->setChecked(m_filterOptions & ApiTraceFilter::ExtraStateFilter);
}

ApiTraceFilter::FilterOptions SettingsDialog::filterOptions() const
{
    return m_filterOptions;
}

#include "settingsdialog.moc"
