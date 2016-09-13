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
            this, SLOT(browseApplication()));
    connect(browseWorkingDirButton, SIGNAL(clicked()),
            this, SLOT(browseWorkingDir()));
}

QString TraceDialog::api() const
{
    return apiComboBox->currentText().toLower();
}

QString TraceDialog::applicationPath() const
{
    return applicationEdit->text();
}

QString TraceDialog::workingDirPath() const
{
    QString workingDir = workingDirEdit->text();

    if(workingDir.isEmpty()) {
        return QDir::currentPath();
    }

    return workingDir;
}

QStringList TraceDialog::arguments() const
{
    QStringList args =
        argumentsEdit->text().split(';', QString::SkipEmptyParts);
    return args;
}

void TraceDialog::browseApplication()
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

void TraceDialog::browseWorkingDir()
{
    QString path =
            QFileDialog::getExistingDirectory(
                this,
                tr("Choose working directory"),
                workingDirPath());

    if(!path.isEmpty() && isDirOk(path)) {
        workingDirEdit->setText(path);
    }
}

void TraceDialog::accept()
{
    if (isFileOk(applicationEdit->text())
            && isDirOk(workingDirPath())) {
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

bool TraceDialog::isDirOk(const QString &path)
{
    QFileInfo fi(path);

    if(!fi.exists()) {
        QMessageBox::warning(this, tr("Directory Does Not Exist"),
                             tr("Directory '%1' doesn't exist.").
                             arg(fi.fileName()));
        return false;
    }

    if(!fi.isDir()) {
        QMessageBox::warning(this, tr("Directory Does Not Exist"),
                             tr("Path '%1' is not a directory").
                             arg(fi.fileName()));
        return false;
    }

    return true;
}

#include "tracedialog.moc"
