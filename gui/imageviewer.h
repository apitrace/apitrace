#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "ui_imageviewer.h"
#include <QDialog>

class PixelWidget;
class QLabel;

class ImageViewer : public QDialog, public Ui_ImageViewer
{
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = 0);

    void setImage(const QImage &image);

    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void slotUpdate();
    void showPixel(int, int);
    void showGrid(const QRect &rect);

private:
    QImage m_image;
    QImage m_temp;
    PixelWidget *m_pixelWidget;
    QLabel *m_pixelLabel;
};


#endif
