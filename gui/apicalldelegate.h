#ifndef APICALLDELEGATE_H
#define APICALLDELEGATE_H

#include <QStyledItemDelegate>

class ApiCallDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ApiCallDelegate(QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

private:
    QIcon m_stateEmblem;
    QIcon m_editEmblem;
    QIcon m_errorEmblem;
};

#endif
