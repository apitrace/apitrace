#ifndef APISURFACE_H
#define APISURFACE_H

#include <QImage>
#include <QSize>
#include <QString>

class ApiSurface
{
public:
    ApiSurface();

    QSize size() const;
    void setSize(const QSize &size);

    int numChannels() const;
    void setNumChannels(int numChannels);

    void contentsFromBase64(const QByteArray &base64);

    QImage image() const;
    QImage thumb() const;

private:
    QSize  m_size;
    int m_numChannels;
    QImage m_image;
    QImage m_thumb;
};


class ApiTexture : public ApiSurface
{
public:
    ApiTexture();

    int unit() const;
    void setUnit(int un);

    int level() const;
    void setLevel(int l);

    QString target() const;
    void setTarget(const QString &str);

private:
    int m_unit;
    int m_level;
    QString m_target;
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
