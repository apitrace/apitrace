#include "apisurface.h"

#include <QDebug>
#include <QSysInfo>

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

static inline int
rgba8_to_argb(quint8 r, quint8 g, quint8 b, quint8 a)
{
    return (a << 24 | r << 16 | g << 8 | b);
}

static inline int
rgbaf2argb(float r, float g, float b, float a)
{
    quint8 rb = r * 255;
    quint8 gb = g * 255;
    quint8 bb = b * 255;
    quint8 ab = a * 255;

    return (ab << 24 | rb << 16 | gb << 8 | bb);
}

void ApiSurface::contentsFromBase64(const QByteArray &base64)
{
    QByteArray dataArray = QByteArray::fromBase64(base64);
    const quint8 *data = (const quint8*)dataArray.data();
    int width = m_size.width();
    int height = m_size.height();

    if (width <= 0 || height <= 0)
        return;

    int *pixelData = (int*)malloc(sizeof(int) * width * height);

    //XXX not sure if this will work when
    //    QSysInfo::ByteOrder == QSysInfo::BigEndian

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pixel = rgba8_to_argb(data[(y * width + x) * 4 + 0],
                                      data[(y * width + x) * 4 + 1],
                                      data[(y * width + x) * 4 + 2],
                                      data[(y * width + x) * 4 + 3]);
            pixelData[y * width + x] = pixel;
        }
    }

    m_image = QImage((uchar*)pixelData,
                     width, height,
                     QImage::Format_ARGB32).mirrored();
    m_thumb = m_image.scaled(64, 64, Qt::KeepAspectRatio);
    //m_image.save("testoutput.png");

    free(pixelData);
}

QImage ApiSurface::image() const
{
    return m_image;
}

QImage ApiSurface::thumb() const
{
    return m_thumb;
}

ApiTexture::ApiTexture()
    : ApiSurface(),
      m_unit(0),
      m_level(0)
{
}

int ApiTexture::unit() const
{
    return m_unit;
}

void ApiTexture::setUnit(int un)
{
    m_unit = un;
}

QString ApiTexture::target() const
{
    return m_target;
}

void ApiTexture::setTarget(const QString &str)
{
    m_target = str;
}

int ApiTexture::level() const
{
    return m_level;
}

void ApiTexture::setLevel(int l)
{
    m_level = l;
}
