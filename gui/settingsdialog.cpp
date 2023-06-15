#include "settingsdialog.h"

#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      m_filter(0)
{
    setupUi(this);

    m_showFilters.insert(
        tr("Draw events"),
        QRegularExpression("glDraw|glVertex|glBegin|glEnd"));
    m_showFilters.insert(
        tr("Texture events"),
        QRegularExpression("glTex|glBindTex|glBegin|glEnd"));

    QMap<QString, QRegularExpression>::const_iterator itr;
    for (itr = m_showFilters.constBegin();
         itr != m_showFilters.constEnd(); ++itr) {
        showFilterCB->addItem(itr.key(), itr.value());
    }
    showFilterCB->addItem(tr("Custom"), QRegularExpression());

    connect(showFilterCB, SIGNAL(currentTextChanged(const QString &)),
            SLOT(changeRegexp(const QString&)));
    connect(showFilterEdit, SIGNAL(textEdited(const QString &)),
            SLOT(regexpChanged(const QString&)));

    showFilterCB->setCurrentIndex(0);
    showFilterEdit->setText(m_showFilters.constBegin().value().pattern());
}

void SettingsDialog::filtersFromModel(const ApiTraceFilter *model)
{
    ApiTraceFilter::FilterOptions opts = model->filterOptions();
    extensionsBox->setChecked(opts & ApiTraceFilter::ExtensionsFilter);
    functionsBox->setChecked(opts & ApiTraceFilter::ResolutionsFilter);
    errorsBox->setChecked(opts & ApiTraceFilter::ErrorsQueryFilter);
    statesBox->setChecked(opts & ApiTraceFilter::ExtraStateFilter);
    customBox->setChecked(opts & ApiTraceFilter::CustomFilter);
    QString customFilter = model->customFilterRegexp();
    if (!customFilter.isEmpty()) {
        customEdit->setText(customFilter);
    } else {
        //Add a common default
        QLatin1String str("glXMakeContextCurrent");
        customEdit->setText(str);
    }

    QRegularExpression regexp = model->filterRegexp();
    if (regexp.pattern().isEmpty()) {
        showFilterBox->setChecked(false);
    } else {
        showFilterBox->setChecked(true);
        QMap<QString, QRegularExpression>::const_iterator itr;
        int i = 0;
        for (itr = m_showFilters.constBegin();
             itr != m_showFilters.constEnd(); ++itr, ++i) {
            if (itr.value() == regexp) {
                showFilterCB->setCurrentIndex(i);
                showFilterEdit->setText(itr.value().pattern());
                return;
            }
        }
        /* custom filter */
        showFilterCB->setCurrentIndex(m_showFilters.count());
        showFilterEdit->setText(regexp.pattern());
    }
}

void SettingsDialog::filtersToModel(ApiTraceFilter *model)
{
    ApiTraceFilter::FilterOptions opts = ApiTraceFilter::NullFilter;
    if (extensionsBox->isChecked()) {
        opts |= ApiTraceFilter::ExtensionsFilter;
    }
    if (functionsBox->isChecked()) {
        opts |= ApiTraceFilter::ResolutionsFilter;
    }
    if (errorsBox->isChecked()) {
        opts |= ApiTraceFilter::ErrorsQueryFilter;
    }
    if (statesBox->isChecked()) {
        opts |= ApiTraceFilter::ExtraStateFilter;
    }
    if (customBox->isChecked()) {
        opts |= ApiTraceFilter::CustomFilter;
        m_filter->setCustomFilterRegexp(customEdit->text());
    }
    m_filter->setFilterOptions(opts);
    if (showFilterBox->isChecked()) {
        m_filter->setFilterRegexp(QRegularExpression(showFilterEdit->text()));
    } else {
        m_filter->setFilterRegexp(QRegularExpression());
    }
}

void SettingsDialog::accept()
{
    if (showFilterBox->isChecked()) {
        QRegularExpression regexp(showFilterEdit->text());
        if (!regexp.isValid()) {
            QMessageBox::warning(
                this,
                tr("Invalid Regexp"),
                tr("The currently set regular expression "
                   "for filtering events is invalid."));
            return;
        }
    }
    if (customBox->isChecked()) {
        QRegularExpression regexp(customEdit->text());
        if (!regexp.isValid()) {
            QMessageBox::warning(
                this,
                tr("Invalid Regexp"),
                tr("The currently set regular expression "
                   "for custom events is invalid."));
            return;
        }
    }
    filtersToModel(m_filter);
    QDialog::accept();
}

void SettingsDialog::changeRegexp(const QString &name)
{
    showFilterEdit->setText(m_showFilters[name].pattern());
}

void SettingsDialog::regexpChanged(const QString &pattern)
{
    int customIndex = m_showFilters.count();
    if (showFilterCB->currentIndex() != customIndex) {
        showFilterCB->blockSignals(true);
        showFilterCB->setCurrentIndex(customIndex);
        showFilterCB->blockSignals(false);
    }
}

void SettingsDialog::setFilterModel(ApiTraceFilter *filter)
{
    m_filter = filter;
    filtersFromModel(m_filter);
}
