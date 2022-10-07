#include "imageviewer.h"
#include "pixelwidget.h"
#include "apisurface.h"

#include "image.hpp"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QSettings>
#include <QScreen>

ImageViewer::ImageViewer(QWidget *parent, bool opaque, bool alpha)
    : QDialog(parent),
      m_image(0),
      m_zoomtoFit()
{
    setupUi(this);
    opaqueCheckBox->setChecked(opaque);
    alphaCheckBox->setChecked(alpha);

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
    pal.setBrush(QPalette::Window,
                 QBrush(px));
    pal.setBrush(QPalette::Base,
                 QBrush(px));
    scrollAreaWidgetContents->setPalette(pal);

    m_pixelWidget = new PixelWidget(scrollAreaWidgetContents);
    verticalLayout_2->setAlignment(Qt::AlignCenter);
    verticalLayout_2->addWidget(m_pixelWidget);

    rectLabel->hide();
    pixelLabel->hide();

    connect(m_pixelWidget, SIGNAL(zoomChanged(double)),
            this, SLOT(zoomChangedIndirectly(double)));
    connect(zoomSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(zoomChangedDirectly()));
    connect(zoomSpinBox, SIGNAL(valueChanged(double)),
            m_pixelWidget, SLOT(setZoom(double)));
    connect(m_pixelWidget, SIGNAL(mousePosition(int, int)),
            this, SLOT(showPixel(int, int)));
    connect(m_pixelWidget, SIGNAL(gridGeometry(const QRect &)),
            this, SLOT(showGrid(const QRect &)));
    connect(m_pixelWidget, SIGNAL(zoomStepUp()),
            this, SLOT(zoomChangedDirectly()));
    connect(m_pixelWidget, SIGNAL(zoomStepUp()),
            zoomSpinBox, SLOT(stepUp()));
    connect(m_pixelWidget, SIGNAL(zoomStepDown()),
            this, SLOT(zoomChangedDirectly()));
    connect(m_pixelWidget, SIGNAL(zoomStepDown()),
            zoomSpinBox, SLOT(stepDown()));
    connect(zoomToFitCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(zoomToFitChanged(int)));

    const auto zoomToFit = QSettings().value("imageViewerZoomToFit");
    if (!zoomToFit.isNull()) {
        zoomToFitCheckBox->setChecked(zoomToFit.toBool());
    }

    m_zoomtoFit = zoomToFitCheckBox->checkState() == Qt::Checked;

    m_pixelWidget->setFocus();
}

ImageViewer::~ImageViewer()
{
    delete m_image;
}

void ImageViewer::setData(const QByteArray &data)
{
    delete m_image;
    m_image = ApiSurface::imageFromData(data);
    slotUpdate();
}

void ImageViewer::slotUpdate()
{
    m_convertedImage =
        m_convertedImage.mirrored(false, flipCheckBox->isChecked());

    double lowerValue = lowerSpinBox->value();
    double upperValue = upperSpinBox->value();

    bool opaque = opaqueCheckBox->isChecked();
    bool alpha  = alphaCheckBox->isChecked();

    m_convertedImage = ApiSurface::qimageFromRawImage(m_image,
                                                      lowerValue, upperValue,
                                                      opaque, alpha);

    if (flipCheckBox->isChecked()) {
        m_convertedImage = m_convertedImage.mirrored(false, true);
    }

    m_pixelWidget->setSurface(m_convertedImage);

    updateGeometry();
}

QSize ImageViewer::sizeHint() const
{
    int vScrollWidth = scrollArea->verticalScrollBar() ?
                scrollArea->verticalScrollBar()->width() : 0;
    int hScrollHeight = scrollArea->horizontalScrollBar() ?
                scrollArea->horizontalScrollBar()->height() : 0;
    QSize optimalWindowSize(m_convertedImage.width() + vScrollWidth,
                            m_convertedImage.height() + hScrollHeight);

    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    const float maxPercentOfDesktopSpace = 0.8f;
    QSize maxAvailableSize(maxPercentOfDesktopSpace * screenRect.width(),
                           maxPercentOfDesktopSpace * screenRect.height());

    return QSize(qMin(optimalWindowSize.width(), maxAvailableSize.width()),
                 qMin(optimalWindowSize.height(), maxAvailableSize.height()));
}

void ImageViewer::resizeEvent(QResizeEvent *e)
{
    if (m_zoomtoFit) {
        zoomToFit();
    }

    QWidget::resizeEvent(e);
}

template <class T>
QString createPixelLabel(image::Image *img, int x, int y)
{
    QString pixelLabel;
    unsigned char *pixelLocation = 0;
    T *pixel;

    if (x < 0 || y < 0 || x >= img->width || y >= img->height) {
        return QString::fromLatin1("(Out of bounds)");
    }

    pixelLocation = img->pixels + img->stride() * y;
    pixelLocation += x * img->bytesPerPixel;
    pixel = ((T*)pixelLocation);

    pixelLabel += QLatin1String("[");
    pixelLabel += QString::fromLatin1("%1").arg((double)pixel[0], 0, 'g', 9);

    for (int channel = 1; channel < img->channels; ++channel) {
        pixelLabel += QString::fromLatin1(", %1").arg((double)pixel[channel], 0, 'g', 9);
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

    /* If the image is flipped, substitute y to match */
    if (flipCheckBox->isChecked()) {
        y = m_convertedImage.height() - y - 1;
    }

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

void ImageViewer::zoomChangedDirectly()
{
    m_zoomtoFit = false;
}

void ImageViewer::zoomChangedIndirectly(double zoom)
{
    zoomSpinBox->blockSignals(true);
    zoomSpinBox->setValue(zoom);
    zoomSpinBox->blockSignals(false);
}

void ImageViewer::zoomToFitChanged(int state)
{
    const auto newZoomtoFit = state == Qt::Checked;

    if (m_zoomtoFit != newZoomtoFit) {
        m_zoomtoFit = newZoomtoFit;
        if (m_zoomtoFit) {
            zoomToFit();
        }
    }

    QSettings().setValue("imageViewerZoomToFit", newZoomtoFit);
}

void ImageViewer::zoomToFit()
{
    int left, top, right, bottom;
    verticalLayout_2->getContentsMargins(&left, &top, &right, &bottom);

    const int w = scrollArea->width() - left - right - 2 * scrollArea->frameWidth();
    const int h = scrollArea->height() - top - bottom - 2 * scrollArea->frameWidth();
    const int iw = m_convertedImage.width();
    const int ih = m_convertedImage.height();

    m_pixelWidget->setZoom(std::min(w / (double)iw, h / (double)ih), false);
}
