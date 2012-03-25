#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#define THUMBNAIL_SIZE 64

#include <QImage>

inline QImage
thumbnail(const QImage &image, Qt::TransformationMode transformationMode = Qt::FastTransformation) {
    return image.scaled(THUMBNAIL_SIZE, THUMBNAIL_SIZE, Qt::KeepAspectRatio, transformationMode);
}

#endif
