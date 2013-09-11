#include "apisurface.h"
#include "thumbnail.h"

#include <sstream>

#include <QDebug>
#include <QSysInfo>

#include "image/image.hpp"


ApiSurface::ApiSurface()
{
}

QSize ApiSurface::size() const
{
    return m_size;
}

void ApiSurface::setSize(const QSize &size)
{
    m_size = size;
}

struct ByteArrayBuf : public std::streambuf
{
    ByteArrayBuf(QByteArray & a)
    {
        setg(a.data(), a.data(), a.data() + a.size());
    }
};

void ApiSurface::contentsFromBase64(const QByteArray &base64)
{
    QByteArray dataArray = QByteArray::fromBase64(base64);

    /*
     * FIXME: Detect the float PFM images here.
     */
    ByteArrayBuf buf(dataArray);
    std::istream istr(&buf);
    image::Image *image = image::readPNG(istr);

    /*
     * FIXME: Instead of converting to QImage here, we should be deferring the conversion
     * to imageviewer.cpp.
     *
     * XXX: We still need the thumbnail though.
     */

    Q_ASSERT(image);

    int width = image->width;
    int height = image->height;

    m_image = QImage(width, height, QImage::Format_ARGB32);

    const unsigned char *srcRow = image->start();
    for (int y = 0; y < height; ++y) {
        QRgb *dst = (QRgb *)m_image.scanLine(y);

        if (image->channelType == image::TYPE_UNORM8) {
            const unsigned char *src = srcRow;
            for (int x = 0; x < width; ++x) {
                unsigned char rgba[4];
                for (int c = 0; c < image->channels; ++c) {
                    rgba[c] = *src++;
                }
                if (image->channels == 1) {
                    // Use gray-scale instead of red
                    rgba[1] = rgba[0];
                    rgba[2] = rgba[0];
                }
                dst[x] = qRgba(rgba[0], rgba[1], rgba[2], rgba[3]);
            }
        } else {
            const float *src = (const float *)srcRow;
            for (int x = 0; x < width; ++x) {
                unsigned char rgba[4] = {0, 0, 0, 0xff};
                for (int c = 0; c < image->channels; ++c) {
                    float f = *src++;
                    unsigned char u;
                    if (f >= 255.0f) {
                        u = 255;
                    } else if (f <= 0.0f) {
                        u = 0;
                    } else {
                        u = f * 255 + 0.5;
                    }
                    rgba[c] = u;
                }
                if (image->channels == 1) {
                    // Use gray-scale instead of red
                    rgba[1] = rgba[0];
                    rgba[2] = rgba[0];
                }
                dst[x] = qRgba(rgba[0], rgba[1], rgba[2], rgba[3]);
            }
        }

        srcRow += image->stride();
    }

    delete image;

    m_thumb = thumbnail(m_image);
}

QImage ApiSurface::image() const
{
    return m_image;
}

QImage ApiSurface::thumb() const
{
    return m_thumb;
}

int ApiSurface::depth() const
{
    return m_depth;
}

void ApiSurface::setDepth(int depth)
{
    m_depth = depth;
}

QString ApiSurface::formatName() const
{
    return m_formatName;
}

void ApiSurface::setFormatName(const QString &str)
{
    m_formatName = str;
}


ApiTexture::ApiTexture()
    : ApiSurface()
{
}

QString ApiTexture::label() const
{
    return m_label;
}

void ApiTexture::setLabel(const QString &str)
{
    m_label = str;
}

ApiFramebuffer::ApiFramebuffer()
    : ApiSurface()
{
}

QString ApiFramebuffer::type() const
{
    return m_type;
}

void ApiFramebuffer::setType(const QString &str)
{
    m_type = str;
}

