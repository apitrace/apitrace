#ifndef APISURFACE_H
#define APISURFACE_H

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

    void contentsFromBase64(const QByteArray &base64);

    QByteArray base64Data() const;
    QImage thumb() const;

    static image::Image *imageFromBase64(const QByteArray &data);
    static QImage qimageFromRawImage(const image::Image *img,
                                     float lowerValue = 0.0f,
                                     float upperValue = 1.0f,
                                     bool opaque = false,
                                     bool alpha = false);

private:
    QSize  m_size;
    int m_numChannels;
    QByteArray m_base64Data;
    QImage m_thumb;
    int m_depth;
    QString m_formatName;
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

#endif
