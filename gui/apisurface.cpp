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

int ApiSurface::numChannels() const
{
    return m_numChannels;
}

void ApiSurface::setNumChannels(int numChannels)
{
    m_numChannels = numChannels;
}

void ApiSurface::contentsFromBase64(const QByteArray &base64)
{
    QByteArray dataArray = QByteArray::fromBase64(base64);
    m_image.loadFromData(dataArray, "png");
    m_image = m_image.mirrored();
    m_thumb = m_image.scaled(64, 64, Qt::KeepAspectRatio);
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
