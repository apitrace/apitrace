#include "settingsdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

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

    connect(adbPathEdit, SIGNAL(textEdited(const QString &)),
            SLOT(adbPathChanged(const QString&)));


    {
        QSettings s;
        s.beginGroup(QLatin1String("android"));
        adbPathEdit->setText(s.value(QLatin1String("adbPath"), adbPathEdit->text()).toString());
        stdoutPort->setValue(s.value(QLatin1String("stdoutPort"), stdoutPort->value()).toInt());
        stderrPort->setValue(s.value(QLatin1String("stderrPort"), stderrPort->value()).toInt());
        s.endGroup();
    }

    connect(chooseAdbPath, SIGNAL(clicked()),
            SLOT(chooseAdbPathPushed()));
    connect(stdoutPort, SIGNAL(valueChanged(int)),
            SLOT(stdoutPortChanged(int)));
    connect(stderrPort, SIGNAL(valueChanged(int)),
            SLOT(stderrPortChanged(int)));

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

void SettingsDialog::adbPathChanged(const QString &adbPath)
{
    QSettings s;
    s.beginGroup(QLatin1String("android"));
    s.setValue(QLatin1String("adbPath"), adbPath);
    s.endGroup();
}

void SettingsDialog::chooseAdbPathPushed()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose adb path"), adbPathEdit->text(), QLatin1String("ADB (adb"
#ifdef Q_OS_WIN
                                                    ".exe"
#endif
                                                    ")"));
    if (!fileName.isEmpty()) {
        adbPathEdit->setText(fileName);
        adbPathChanged(fileName);
    }
}

void SettingsDialog::stdoutPortChanged(int port)
{
    QSettings s;
    s.beginGroup(QLatin1String("android"));
    s.setValue(QLatin1String("stdoutPort"), port);
    s.endGroup();
}

void SettingsDialog::stderrPortChanged(int port)
{
    QSettings s;
    s.beginGroup(QLatin1String("android"));
    s.setValue(QLatin1String("stderrPort"), port);
    s.endGroup();
}

void SettingsDialog::setFilterModel(ApiTraceFilter *filter)
{
    m_filter = filter;
    filtersFromModel(m_filter);
}

#include "settingsdialog.moc"
