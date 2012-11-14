#include "imageviewer.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>

ImageViewer::ImageViewer(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(lowerSpinBox, SIGNAL(valueChanged(double)),
            SLOT(slotUpdate()));
    connect(upperSpinBox, SIGNAL(valueChanged(double)),
            SLOT(slotUpdate()));
    connect(flipCheckBox, SIGNAL(stateChanged(int)),
            SLOT(slotUpdate()));
    connect(opaqueCheckBox, SIGNAL(stateChanged(int)),
            SLOT(slotUpdate()));
    connect(alphaCheckBox, SIGNAL(stateChanged(int)),
            SLOT(slotUpdate()));

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
    m_temp = m_image;
    QPixmap px = QPixmap::fromImage(m_temp);
    imageLabel->setPixmap(px);
    updateGeometry();
}

static inline int clamp(int x)
{
    if (x <= 0) {
        return 0;
    }
    if (x > 255) {
        return 255;
    }
    return x;
}

void ImageViewer::slotUpdate()
{
    m_temp = m_image.mirrored(false, flipCheckBox->isChecked());

    double lowerValue = lowerSpinBox->value();
    double upperValue = upperSpinBox->value();

    bool opaque = opaqueCheckBox->isChecked();
    bool alpha  = alphaCheckBox->isChecked();

    if (lowerValue != 0.0 || upperValue != 1.0 || opaque || alpha) {
        /*
         * Rescale the image.
         *
         * XXX: This would be much more useful if done with the full precision
         * of the original image
         */

        int offset = - lowerValue * 255;
        int scale = 256 / (upperValue - lowerValue);

        m_temp = m_temp.convertToFormat(QImage::Format_ARGB32);

        if (0) {
            qDebug()
                << "offset = " << offset << "\n"
                << "scale = " << scale << "\n";
        }

        int width = m_temp.width();
        int height = m_temp.height();

        int aMask = opaque ? 0xff : 0;

        for (int y = 0; y < height; ++y) {
            QRgb *scanline = (QRgb *)m_temp.scanLine(y);
            for (int x = 0; x < width; ++x) {
                QRgb pixel = scanline[x];
                int r = qRed(pixel);
                int g = qGreen(pixel);
                int b = qBlue(pixel);
                int a = qAlpha(pixel);
                if (alpha) {
                    a = clamp(((a + offset) * scale) >> 8);
                    scanline[x] = qRgba(a, a, a, 0xff);
                } else {
                    r = clamp(((r + offset) * scale) >> 8);
                    g = clamp(((g + offset) * scale) >> 8);
                    b = clamp(((b + offset) * scale) >> 8);
                    a |= aMask;
                    scanline[x] = qRgba(r, g, b, a);
                }
            }
        }
    }

    QPixmap px = QPixmap::fromImage(m_temp);
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
    const float maxPercentOfDesktopSpace = 0.8f;
    QSize maxAvailableSize(maxPercentOfDesktopSpace * screenRect.width(),
                           maxPercentOfDesktopSpace * screenRect.height());

    return QSize(qMin(optimalWindowSize.width(), maxAvailableSize.width()),
                 qMin(optimalWindowSize.height(), maxAvailableSize.height()));
}

#include "imageviewer.moc"
