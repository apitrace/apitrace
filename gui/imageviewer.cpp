#include "imageviewer.h"

#include <QPainter>
#include <QPixmap>

ImageViewer::ImageViewer(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    QPixmap px(32, 32);
    QPainter p(&px);
    p.fillRect(0, 0, 32, 32, Qt::white);
    p.fillRect(0, 0, 16, 16, QColor(193, 193, 193));
    p.fillRect(16, 16, 16, 16, QColor(193, 193, 193));
    p.end();
    QPalette pal = scrollAreaWidgetContents->palette();
    pal.setBrush(QPalette::Background,
                 QBrush(px));
    pal.setBrush(QPalette::Base,
                 QBrush(px));
    scrollAreaWidgetContents->setPalette(pal);
}

void ImageViewer::setImage(const QImage &image)
{
    QPixmap px = QPixmap::fromImage(image);
    imageLabel->setPixmap(px);
}

#include "imageviewer.moc"
