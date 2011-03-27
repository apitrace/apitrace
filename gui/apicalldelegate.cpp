#include "apicalldelegate.h"

#include "apitracecall.h"

#include <QDebug>
#include <QPainter>
#include <QStaticText>
#include <QStyle>

ApiCallDelegate::ApiCallDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void ApiCallDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    ApiTraceCall *call = index.data().value<ApiTraceCall*>();
    if (call) {
        QStaticText text = call->staticText();
        //text.setTextWidth(option.rect.width());
        QStyledItemDelegate::paint(painter, option, index);
        painter->drawStaticText(option.rect.topLeft(), text);
    } else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ApiCallDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    ApiTraceCall *call = index.data().value<ApiTraceCall*>();
    if (call) {
        QStaticText text = call->staticText();
        //text.setTextWidth(option.rect.width());
        return text.size().toSize();
    }
    return QStyledItemDelegate::sizeHint(option, index);
}


#include "apicalldelegate.moc"
