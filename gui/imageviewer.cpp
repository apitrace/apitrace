#include "imageviewer.h"
#include "pixelwidget.h"
#include "apisurface.h"

#include "image/image.hpp"

#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>

ImageViewer::ImageViewer(QWidget *parent)
    : QDialog(parent),
      m_image(0)
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

    m_pixelWidget = new PixelWidget(scrollAreaWidgetContents);
    verticalLayout_2->addWidget(m_pixelWidget);

    rectLabel->hide();
    pixelLabel->hide();

    connect(m_pixelWidget, SIGNAL(zoomChanged(int)),
            zoomSpinBox, SLOT(setValue(int)));
    connect(zoomSpinBox, SIGNAL(valueChanged(int)),
            m_pixelWidget, SLOT(setZoom(int)));
    connect(m_pixelWidget, SIGNAL(mousePosition(int, int)),
            this, SLOT(showPixel(int, int)));
    connect(m_pixelWidget, SIGNAL(gridGeometry(const QRect &)),
            this, SLOT(showGrid(const QRect &)));
}

ImageViewer::~ImageViewer()
{
    delete m_image;
}

void ImageViewer::setBase64Data(const QByteArray &base64)
{
    delete m_image;
    m_image = ApiSurface::imageFromBase64(base64);
    m_convertedImage = ApiSurface::qimageFromRawImage(m_image);
    m_pixelWidget->setSurface(m_convertedImage);
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
    m_convertedImage =
        m_convertedImage.mirrored(false, flipCheckBox->isChecked());

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

        m_convertedImage =
            m_convertedImage.convertToFormat(QImage::Format_ARGB32);

        if (0) {
            qDebug()
                << "offset = " << offset << "\n"
                << "scale = " << scale << "\n";
        }

        int width = m_convertedImage.width();
        int height = m_convertedImage.height();

        int aMask = opaque ? 0xff : 0;

        for (int y = 0; y < height; ++y) {
            QRgb *scanline = (QRgb *)m_convertedImage.scanLine(y);
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

    m_pixelWidget->setSurface(m_convertedImage);

    updateGeometry();
}

QSize ImageViewer::sizeHint() const
{
    QSize size;

    int vScrollWidth = scrollArea->verticalScrollBar() ?
                scrollArea->verticalScrollBar()->width() : 0;
    int hScrollHeight = scrollArea->horizontalScrollBar() ?
                scrollArea->horizontalScrollBar()->height() : 0;
    QSize optimalWindowSize(m_convertedImage.width() + vScrollWidth,
                            m_convertedImage.height() + hScrollHeight);

    QRect screenRect = QApplication::desktop()->availableGeometry();
    const float maxPercentOfDesktopSpace = 0.8f;
    QSize maxAvailableSize(maxPercentOfDesktopSpace * screenRect.width(),
                           maxPercentOfDesktopSpace * screenRect.height());

    return QSize(qMin(optimalWindowSize.width(), maxAvailableSize.width()),
                 qMin(optimalWindowSize.height(), maxAvailableSize.height()));
}

void ImageViewer::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

template <class T>
QString createPixelLabel(image::Image *img, int x, int y)
{
    QString pixelLabel;
    unsigned char *pixelLocation = 0;
    T *pixel;

    pixelLocation = img->pixels + img->stride() * y;
    pixelLocation += x * img->bytesPerPixel;
    pixel = ((T*)pixelLocation);

    pixelLabel += QLatin1String("[");
    pixelLabel += QString::fromLatin1("%1").arg(pixel[0]);

    for (int channel = 1; channel < img->channels; ++channel) {
        pixelLabel += QString::fromLatin1(", %1").arg(pixel[channel]);
    }
    pixelLabel += QLatin1String("]");

    return pixelLabel;
}

void ImageViewer::showPixel(int x, int y)
{
    xSpinBox->setValue(x);
    ySpinBox->setValue(y);

    if (!m_image)
        return;

    QString label = tr("Pixel: ");

    if (m_image->channelType == image::TYPE_UNORM8) {
        label += createPixelLabel<unsigned char>(m_image, x, y);
    } else {
        label += createPixelLabel<float>(m_image, x, y);
    }

    pixelLabel->setText(label);
    pixelLabel->show();
}

void ImageViewer::showGrid(const QRect &rect)
{
    if (rect.isEmpty()) {
        rectLabel->hide();
        return;
    }
    rectLabel->setText(tr("Grid: [%1, %2, %3, %4]")
                       .arg(rect.x())
                       .arg(rect.y())
                       .arg(rect.width())
                       .arg(rect.height()));
    rectLabel->show();
}

#include "imageviewer.moc"
