#include "androidfiledialog.h"
#include "androidutils.h"

#include "ui_androidfiledialog.h"

#include <QAbstractTableModel>
#include <QApplication>
#include <QFileInfo>

typedef QLatin1String _;

static bool androidFileLessThan(const AndroidUtils::FileInfo &file1, const AndroidUtils::FileInfo &file2)
{
    if (file1.type != file2.type)
        return file1.type == AndroidUtils::FileInfo::Directory;
    return file1.name < file2.name;
}

const int FileInfoRole = Qt::UserRole + 1;

static QString humanSize(double size)
{
    QStringList list;
    list << " KB" << " MB" << " GB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(size >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        size /= 1024.0;
    }

    return QString::number(size, 'f', 2) + unit;
}

class AndroidFileDialogModel : public QAbstractTableModel
{
public:
    AndroidFileDialogModel(QObject * parent = 0)
        : QAbstractTableModel(parent)
    {}

    void setDir(const QString &dir)
    {
        m_fileList = m_androidUtils.ls(dir + _("/"));
        setFilterSuffix(m_suffix);
    }

    void setFilterSuffix(const QString &suffix)
    {
        m_suffix = suffix;
        beginResetModel();
        if (!m_suffix.isEmpty()) {
            AndroidUtils::FileInfoList::iterator it = m_fileList.begin();
            while(it != m_fileList.end()) {
                if (it->type == AndroidUtils::FileInfo::File &&
                        !it->name.endsWith(m_suffix)) {
                    it = m_fileList.erase(it);
                } else {
                    ++it;
                }
            }
        }
        std::sort(m_fileList.begin(), m_fileList.end(), androidFileLessThan);
        endResetModel();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_fileList.size())
            return QVariant();

        QVariant ret;
        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0:
                ret = m_fileList.at(index.row()).name;
                break;
            case 1:
            {
                const AndroidUtils::FileInfo &file = m_fileList.at(index.row());
                if (file.type == AndroidUtils::FileInfo::File)
                    ret = humanSize(file.size);
            }
                break;
            default:
                break;
            }
            break;

        case Qt::DecorationRole:
            if (index.column() == 0) {
                if (m_fileList.at(index.row()).type == AndroidUtils::FileInfo::File)
                    ret = qApp->style()->standardIcon(QStyle::SP_FileIcon);
                else
                    ret = qApp->style()->standardIcon(QStyle::SP_DirIcon);
            }
            break;

        case Qt::TextAlignmentRole:
            ret = int(index.column() == 1 ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
            break;

        case FileInfoRole:
            ret = QVariant::fromValue(m_fileList.at(index.row()));
            break;
        default:
            break;
        }
        return ret;
    }
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return QVariant();

        QVariant ret;
        switch (section) {
        case 0:
            ret = tr("Name");
            break;
        case 1:
            ret = tr("Size");
            break;
        }
        return ret;
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 2;
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;
        return m_fileList.size();
    }

private:
    AndroidUtils m_androidUtils;
    QString m_suffix;
    AndroidUtils::FileInfoList m_fileList;
};

AndroidFileDialog::AndroidFileDialog(Mode mode, QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::AndroidFileDialog),
      m_model(new AndroidFileDialogModel(this)),
      m_mode(mode)
{
    m_ui->setupUi(this);
    m_ui->pathComboBox->setModel(&m_pathModel);
    m_ui->filesView->setModel(m_model);
    m_ui->upButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowUp));
    m_ui->buttonBox->setStandardButtons(QDialogButtonBox::StandardButton(m_mode) | QDialogButtonBox::Cancel);
    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(m_ui->upButton, SIGNAL(clicked()), SLOT(upFolder()));
    connect(m_ui->pathComboBox, SIGNAL(activated(QString)), SLOT(setDir(QString)));
    QHeaderView *hv = m_ui->filesView->horizontalHeader();
    hv->setSectionResizeMode(0, QHeaderView::Stretch);
    hv->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    connect(m_ui->filesView, SIGNAL(activated(QModelIndex)), SLOT(itemActivated(QModelIndex)));
}

AndroidFileDialog::~AndroidFileDialog()
{
    delete m_model;
}

void AndroidFileDialog::setInitialPath(const QString &path)
{
    AndroidUtils::FileInfoList files = AndroidUtils().ls(path);
    if (files.size() == 1 && files.at(0).type == AndroidUtils::FileInfo::File) {
        QFileInfo fi(path);
        m_ui->fileLineEdit->setText(fi.fileName());
        setDir(fi.absolutePath());
    } else {
        if (files.isEmpty() && m_mode == SaveFile) {
            QFileInfo fi(path);
            m_ui->fileLineEdit->setText(fi.fileName());
            setDir(fi.absolutePath());
        } else {
            m_ui->fileLineEdit->setText(QString());
            setDir(path);
        }
    }
}

void AndroidFileDialog::setSuffix(const QString &suffix)
{
    m_model->setFilterSuffix(suffix);
}

QString AndroidFileDialog::filePath() const
{
    return m_dir + _("/") + m_ui->fileLineEdit->text();
}

QString AndroidFileDialog::getOpenFileName(QWidget *parent, const QString &caption, const QString &path, const QString &filter)
{
    AndroidFileDialog d(OpenFile, parent);
    d.setWindowTitle(caption);
    d.setSuffix(filter);
    d.setInitialPath(path);
    if (d.exec() == QDialog::Accepted)
        return d.filePath();
    return QString();
}

QString AndroidFileDialog::getSaveFileName(QWidget *parent, const QString &caption, const QString &path, const QString &filter)
{
    AndroidFileDialog d(SaveFile, parent);
    d.setWindowTitle(caption);
    d.setSuffix(filter);
    d.setInitialPath(path);
    if (d.exec() == QDialog::Accepted)
        return d.filePath();
    return QString();
}

void AndroidFileDialog::accept()
{
    if (m_mode == SaveFile) {
        QDialog::accept();
        return;
    }
    AndroidUtils::FileInfoList files = AndroidUtils().ls(filePath());
    if (files.size() == 1 && files.at(0).type == AndroidUtils::FileInfo::File)
        QDialog::accept();
}

void AndroidFileDialog::upFolder()
{
    setDir(QFileInfo(m_dir).absolutePath());
}

void AndroidFileDialog::itemActivated(QModelIndex index)
{
    QVariant file = index.data(FileInfoRole);
    if (file.isNull())
        return;
    AndroidUtils::FileInfo f = file.value<AndroidUtils::FileInfo>();
    if (f.type == AndroidUtils::FileInfo::Directory) {
        setDir(m_dir + _("/") + f.name);
    } else {
        m_ui->fileLineEdit->setText(f.name);
        accept();
    }
}

void AndroidFileDialog::setDir(const QString &dir)
{
    m_dir = dir;
    while (m_dir.endsWith(QLatin1Char('/')))
        m_dir.chop(1);

    if (m_dir.isEmpty())
        m_dir = _("/");

    QStringList paths;
    QString dirname = m_dir;
    paths << dirname;
    while (dirname != _("/")) {
        dirname = QFileInfo(dirname).absolutePath();
        paths << dirname;
    }

    m_pathModel.setStringList(paths);
    m_ui->pathComboBox->setCurrentIndex(0);
    m_ui->upButton->setEnabled(paths.size() > 1);
    m_model->setDir(dir);
}

Q_DECLARE_METATYPE(AndroidUtils::FileInfo);
