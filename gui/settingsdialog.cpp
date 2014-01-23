#include "settingsdialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      m_filter(0)
{
    setupUi(this);

    m_showFilters.insert(
        tr("Draw events"),
        QRegExp("glDraw|glVertex|glBegin|glEnd"));
    m_showFilters.insert(
        tr("Texture events"),
        QRegExp("glTex|glBindTex|glBegin|glEnd"));

    QMap<QString, QRegExp>::const_iterator itr;
    for (itr = m_showFilters.constBegin();
         itr != m_showFilters.constEnd(); ++itr) {
        showFilterCB->addItem(itr.key(), itr.value());
    }
    showFilterCB->addItem(tr("Custom"), QRegExp());

    connect(showFilterCB, SIGNAL(currentIndexChanged(const QString &)),
            SLOT(changeRegexp(const QString&)));
    connect(showFilterEdit, SIGNAL(textEdited(const QString &)),
            SLOT(regexpChanged(const QString&)));

    showFilterCB->setCurrentIndex(0);
    showFilterEdit->setText(m_showFilters.constBegin().value().pattern());

    // Connect Apply button from ButtonBox
    QPushButton* applyButton = buttonBox->button(QDialogButtonBox::Apply);
    Q_ASSERT (applyButton != NULL);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
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

    QRegExp regexp = model->filterRegexp();
    if (regexp.isEmpty()) {
        showFilterBox->setChecked(false);
    } else {
        showFilterBox->setChecked(true);
        QMap<QString, QRegExp>::const_iterator itr;
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
        m_filter->setFilterRegexp(QRegExp(showFilterEdit->text()));
    } else {
        m_filter->setFilterRegexp(QRegExp());
    }
}

void SettingsDialog::accept()
{
    settingsUpdate();
    QDialog::accept();
}

void SettingsDialog::settingsUpdate()
{
    if (showFilterBox->isChecked()) {
        QRegExp regexp(showFilterEdit->text());
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
        QRegExp regexp(customEdit->text());
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
}

void SettingsDialog::apply()
{
    settingsUpdate();
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

#include "settingsdialog.moc"
