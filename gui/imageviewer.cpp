#include "imageviewer.h"

#include <QDesktopWidget>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>

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
    m_image = image;
    QPixmap px = QPixmap::fromImage(image);
    imageLabel->setPixmap(px);
    updateGeometry();
}

QSize ImageViewer::sizeHint() const
{
    QSize size;

    int vScrollWidth = scrollArea->verticalScrollBar() ?
                scrollArea->verticalScrollBar()->width() : 0;
    int hScrollHeight = scrollArea->horizontalScrollBar() ?
                scrollArea->horizontalScrollBar()->height() : 0;
    QSize optimalWindowSize(m_image.width() + vScrollWidth,
                            m_image.height() + hScrollHeight);

    QRect screenRect = QApplication::desktop()->availableGeometry();
    const float maxPercentOfDesktopSpace = 0.8;
    QSize maxAvailableSize(maxPercentOfDesktopSpace * screenRect.width(),
                           maxPercentOfDesktopSpace * screenRect.height());

    return QSize(qMin(optimalWindowSize.width(), maxAvailableSize.width()),
                 qMin(optimalWindowSize.height(), maxAvailableSize.height()));
}

#include "imageviewer.moc"
