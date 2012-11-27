#include "apisurface.h"
#include "thumbnail.h"

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

void ApiSurface::contentsFromBase64(const QByteArray &base64)
{
    QByteArray dataArray = QByteArray::fromBase64(base64);
    m_image.loadFromData(dataArray, "png");
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

