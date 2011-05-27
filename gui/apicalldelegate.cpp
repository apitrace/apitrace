#include "apicalldelegate.h"

#include "apitracecall.h"
#include "apitracemodel.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStaticText>
#include <QStyle>

ApiCallDelegate::ApiCallDelegate(QWidget *parent)
    : QStyledItemDelegate(parent),
      m_stateEmblem(":/resources/dialog-information.png"),
      m_editEmblem(":/resources/document-edit.png"),
      m_errorEmblem(":/resources/script-error.png")
{
}

void ApiCallDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    QVariant var = index.data(ApiTraceModel::EventRole);
    ApiTraceEvent *event = var.value<ApiTraceEvent*>();

    Q_ASSERT(index.column() == 0);

    if (event) {
        QPoint offset;
        QStaticText text = event->staticText();
        //text.setTextWidth(option.rect.width());
        //QStyledItemDelegate::paint(painter, option, index);
        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &option, painter, 0);
        if (!event->state().isEmpty()) {
            QPixmap px = m_stateEmblem.pixmap(option.rect.height(),
                                              option.rect.height());
            painter->drawPixmap(option.rect.topLeft(), px);
            offset = QPoint(option.rect.height() + 5, 0);
        }
        if (event->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
            if (call->hasError()) {
                QPixmap px = m_errorEmblem.pixmap(option.rect.height(),
                                                  option.rect.height());
                painter->drawPixmap(option.rect.topLeft() + offset, px);
                offset += QPoint(option.rect.height() + 5, 0);
            }
            if (call->edited()) {
                QPixmap px = m_editEmblem.pixmap(option.rect.height(),
                                                 option.rect.height());
                painter->drawPixmap(option.rect.topLeft() + offset, px);
                offset += QPoint(option.rect.height() + 5, 0);
            }
        }

        painter->drawStaticText(option.rect.topLeft() + offset, text);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ApiCallDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    ApiTraceEvent *event =
        index.data(ApiTraceModel::EventRole).value<ApiTraceEvent*>();

#ifndef __APPLE__
    /* XXX: This fails on MacOSX, but seems safe to ignore */
    Q_ASSERT(index.column() == 0);
#endif

    if (event) {
        QStaticText text = event->staticText();
        //text.setTextWidth(option.rect.width());
        //qDebug()<<"text size = "<<text.size();
        return text.size().toSize();
    }
    return QStyledItemDelegate::sizeHint(option, index);
}


#include "apicalldelegate.moc"
