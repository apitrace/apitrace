#pragma once

#include "ui_imageviewer.h"
#include <QDialog>

class PixelWidget;
class QLabel;

namespace image {
    class Image;
}

class ImageViewer : public QDialog, public Ui_ImageViewer
{
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = 0, bool opaque = false, bool alpha = false);
    ~ImageViewer();

    void setData(const QByteArray &data);

    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotUpdate();
    void showPixel(int, int);
    void showGrid(const QRect &rect);
    void zoomChangedDirectly();
    void zoomChangedIndirectly(double zoom);
    void zoomToFitChanged(int state);

private:
    void zoomToFit();

    image::Image *m_image;
    QImage m_convertedImage;
    PixelWidget *m_pixelWidget;
    bool m_zoomtoFit;
};
