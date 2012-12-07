#include "tracedialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

TraceDialog::TraceDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    apiComboBox->addItem("GL");
#ifdef Q_OS_WIN
    apiComboBox->addItem("D3D7");
    apiComboBox->addItem("D3D8");
    apiComboBox->addItem("D3D9");
    apiComboBox->addItem("D3D10");
    apiComboBox->addItem("D3D10_1");
    apiComboBox->addItem("D3D11");
#else
    apiComboBox->addItem("EGL");
#endif

    connect(browseButton, SIGNAL(clicked()),
            this, SLOT(browse()));
}

QString TraceDialog::api() const
{
    return apiComboBox->currentText().toLower();
}

QString TraceDialog::applicationPath() const
{
    return applicationEdit->text();
}

QStringList TraceDialog::arguments() const
{
    QStringList args =
        argumentsEdit->text().split(';', QString::SkipEmptyParts);
    return args;
}

void TraceDialog::browse()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            tr("Find the application"),
            QDir::currentPath());

    if (!fileName.isEmpty() && isFileOk(fileName)) {
        applicationEdit->setText(fileName);
    }
}

void TraceDialog::accept()
{
    if (isFileOk(applicationEdit->text())) {
        QDialog::accept();
    }
}

bool TraceDialog::isFileOk(const QString &fileName)
{
    QFileInfo fi(fileName);

    if (!fi.exists()) {
        QMessageBox::warning(this, tr("File Missing"),
                             tr("File '%1' doesn't exist.")
                             .arg(fi.fileName()));
        return false;
    }
    if (!fi.isExecutable()) {
        QMessageBox::warning(this, tr("Application Not Runnable"),
                             tr("File '%1' can't be executed.")
                             .arg(fi.fileName()));
        return false;
    }

    return true;
}

#include "tracedialog.moc"
