#pragma once

#include "ui_searchwidget.h"
#include <QPalette>
#include <QWidget>

class QShowEvent;

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent=0);

    void setFound(bool f);
    void show();
signals:
    void searchNext(const QString &str, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    void searchPrev(const QString &str, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

private slots:
    void slotSearchNext();
    void slotSearchPrev();
    void slotCancel();

protected:
    virtual bool eventFilter(QObject *object, QEvent* event) override;
    virtual void showEvent(QShowEvent *event) override;

private:
    Qt::CaseSensitivity caseSensitivity() const;
private:
    Ui_SearchWidget m_ui;
    QPalette m_origPalette;
};
