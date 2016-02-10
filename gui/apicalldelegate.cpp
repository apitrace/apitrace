#include "apicalldelegate.h"

#include "apitracecall.h"
#include "apitracemodel.h"
#include "thumbnail.h"

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
        QPoint offset = option.rect.topLeft();
        QStaticText text = event->staticText();
        QSize textSize = text.size().toSize();
        //text.setTextWidth(option.rect.width());
        //QStyledItemDelegate::paint(painter, option, index);
        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &option, painter, 0);

        // draw thumbnail of frame
        if (event->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);
            const QImage & thumbnail = frame->thumbnail();
            if (!thumbnail.isNull()) {
                painter->drawImage(offset, thumbnail);
                offset += QPoint(textSize.height() + thumbnail.width(), option.rect.height()/2 - textSize.height()/2);
            } else {
                frame->missingThumbnail();
            }
        }

        if (event->hasState()) {
            QPixmap px = m_stateEmblem.pixmap(textSize.height(),
                                              textSize.height());
            painter->drawPixmap(option.rect.topLeft(), px);
            offset += QPoint(textSize.height() + 5, 0);
        }
        if (event->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
            const QImage & thumbnail = call->thumbnail();
            if (!thumbnail.isNull()) {
                painter->drawImage(offset, thumbnail);
                offset += QPoint(textSize.height() + thumbnail.width(), option.rect.height()/2 - textSize.height()/2);
            } else if (call->flags() & trace::CALL_FLAG_RENDER) {
                call->missingThumbnail();
            }
            if (call->hasError()) {
                QPixmap px = m_errorEmblem.pixmap(textSize.height(),
                                                  textSize.height());
                painter->drawPixmap(offset, px);
                offset += QPoint(textSize.height() + 5, 0);
            }
            if (call->edited()) {
                QPixmap px = m_editEmblem.pixmap(textSize.height(),
                                                 textSize.height());
                painter->drawPixmap(offset, px);
                offset += QPoint(textSize.height() + 5, 0);
            }
        }

        painter->drawStaticText(offset, text);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ApiCallDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QVariant var = index.data(ApiTraceModel::EventRole);
    ApiTraceEvent *event = var.value<ApiTraceEvent*>();

#ifndef __APPLE__
    /* XXX: This fails on MacOSX, but seems safe to ignore */
    Q_ASSERT(index.column() == 0);
#endif

    if (event) {
        QStaticText text = event->staticText();
        //text.setTextWidth(option.rect.width());
        //qDebug()<<"text size = "<<text.size();
        QSize size = text.size().toSize();

        // Adjust for thumbnail
        if (event->type() == ApiTraceEvent::Frame) {
            ApiTraceFrame *frame = static_cast<ApiTraceFrame*>(event);
            const QImage & thumbnail = frame->thumbnail();
            if (!thumbnail.isNull()) {
                size.rwidth() += thumbnail.width();
                if (size.height() < thumbnail.height()) {
                    size.setHeight(thumbnail.height());
                }
            }
        } else if (event->type() == ApiTraceEvent::Call) {
            ApiTraceCall *call = static_cast<ApiTraceCall*>(event);
            const QImage & thumbnail = call->thumbnail();
            if (!thumbnail.isNull()) {
                size.rwidth() += thumbnail.width();
                if (size.height() < thumbnail.height()) {
                    size.setHeight(thumbnail.height());
                }
            }
        }

        return size;
    }
    return QStyledItemDelegate::sizeHint(option, index);
}


#include "apicalldelegate.moc"
