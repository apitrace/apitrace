#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

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
    ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    void setBase64Data(const QByteArray &base64);

    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void slotUpdate();
    void showPixel(int, int);
    void showGrid(const QRect &rect);

private:
    image::Image *m_image;
    QImage m_convertedImage;
    PixelWidget *m_pixelWidget;
    QLabel *m_pixelLabel;
};


#endif
