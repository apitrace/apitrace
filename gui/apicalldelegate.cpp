#include "apicalldelegate.h"

#include "apitracecall.h"

#include <QDebug>
#include <QPainter>
#include <QStaticText>

ApiCallDelegate::ApiCallDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void ApiCallDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    ApiTraceCall *call = static_cast<ApiTraceCall*>(index.internalPointer());
    if (call) {
        QStaticText text = call->staticText();
        //text.setTextWidth(option.rect.width());
        painter->drawStaticText(option.rect.topLeft(), text);
    } else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ApiCallDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    ApiTraceCall *call = static_cast<ApiTraceCall*>(index.internalPointer());
    if (call) {
        QStaticText text = call->staticText();
        //text.setTextWidth(option.rect.width());
        return text.size().toSize();
    }
    return QStyledItemDelegate::sizeHint(option, index);
}


#include "apicalldelegate.moc"
