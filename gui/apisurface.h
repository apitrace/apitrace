#pragma once

#include <QImage>
#include <QSize>
#include <QString>

namespace image {
    class Image;
}

class ApiSurface
{
public:
    ApiSurface();

    QSize size() const;
    void setSize(const QSize &size);

    int depth() const;
    void setDepth(int depth);

    QString formatName() const;
    void setFormatName(const QString &str);

    void setData(const QByteArray &data);
    QImage calculateThumbnail(bool opaque, bool alpha) const;

    QByteArray data() const;

    static image::Image *imageFromData(const QByteArray &data);
    static QImage qimageFromRawImage(const image::Image *img,
                                     float lowerValue = 0.0f,
                                     float upperValue = 1.0f,
                                     bool opaque = false,
                                     bool alpha = false);

private:

    QSize  m_size;
    QByteArray m_data;
    int m_depth;
    QString m_formatName;

    QImage calculateThumbnail(const QByteArray &data, bool opaque,
                              bool alpha) const;
};

class ApiTexture : public ApiSurface
{
public:
    ApiTexture();

    QString label() const;
    void setLabel(const QString &str);

private:
    QString m_label;
};

class ApiFramebuffer : public ApiSurface
{
public:
    ApiFramebuffer();

    QString type() const;
    void setType(const QString &str);

private:
    QString m_type;

};
