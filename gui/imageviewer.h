#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "ui_imageviewer.h"
#include <QDialog>

class ImageViewer : public QDialog, public Ui_ImageViewer
{
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = 0);

    void setImage(const QImage &image);

    QSize sizeHint() const;

private slots:
    void slotUpdate();

private:
    QImage m_image;
    QImage m_temp;
};


#endif
