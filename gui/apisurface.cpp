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
