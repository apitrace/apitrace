#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QStringListModel>
#include <QModelIndex>

class AndroidFileDialogModel;

namespace Ui { class AndroidFileDialog; }

class AndroidFileDialog : public QDialog
{
    Q_OBJECT
    enum Mode {
        OpenFile = QDialogButtonBox::Open,
        SaveFile = QDialogButtonBox::Save
    };

public:
    explicit AndroidFileDialog(Mode mode, QWidget *parent = 0);
    ~AndroidFileDialog();

    void setInitialPath(const QString &path);
    void setSuffix(const QString &suffix);
    QString filePath() const;
    void setMode(Mode mode);

    static QString getOpenFileName(QWidget *parent = 0, const QString &caption = QString(), const QString &path = QString(), const QString &suffix = QString());
    static QString getSaveFileName(QWidget *parent = 0, const QString &caption = QString(), const QString &path = QString(), const QString &filter = QString());

private slots:
    virtual void accept() override;
    void upFolder();
    void itemActivated(QModelIndex index);
    void setDir(const QString &dir);

private:
    QString m_dir;
    QStringListModel m_pathModel;
    Ui::AndroidFileDialog *m_ui;
    AndroidFileDialogModel *m_model;
    Mode m_mode;
};

