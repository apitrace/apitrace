/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include "qubjson.h"

#include <QDebug>
#include <QVariant>
#include <QDataStream>

#include "ubjson.hpp"


using namespace ubjson;


static Marker
readMarker(QDataStream &stream)
{
    quint8 byte;
    stream >> byte;
    if (stream.status() != QDataStream::Ok) {
        return MARKER_EOF;
    }
    return static_cast<Marker>(byte);
}


static int8_t
readInt8(QDataStream &stream)
{
    qint8 i;
    stream >> i;
    return i;
}

static uint8_t
readUInt8(QDataStream &stream)
{
    quint8 u;
    stream >> u;
    return u;
}

static int16_t
readInt16(QDataStream &stream)
{
    qint16 i;
    stream >> i;
    return i;
}

static int32_t
readInt32(QDataStream &stream)
{
    qint32 i;
    stream >> i;
    return i;
}

static int64_t
readInt64(QDataStream &stream)
{
    qint64 i;
    stream >> i;
    return i;
}

static float
readFloat32(QDataStream &stream)
{
    float f;
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream >> f;
    return f;
}

static double
readFloat64(QDataStream &stream)
{
    double f;
    stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    stream >> f;
    return f;
}


static int
readSize(QDataStream &stream, Marker type)
{
    switch (type) {
    case MARKER_INT8: {
        int8_t size = readInt8(stream);
        Q_ASSERT(size >= 0);
        return size;
    }
    case MARKER_UINT8:
        return readUInt8(stream);
    case MARKER_INT16: {
        int16_t size = readInt16(stream);
        Q_ASSERT(size >= 0);
        return size;
    }
    case MARKER_INT32: {
        int32_t size = readInt32(stream);
        Q_ASSERT(size >= 0);
        return size;
    }
    case MARKER_INT64: {
        int64_t size = readInt64(stream);
        Q_ASSERT(size >= 0);
        Q_ASSERT_X(size <= INT_MAX, "qubjson::readSize", "size too large (https://git.io/JOMRa)");
        return size;
    }
    default:
        Q_UNIMPLEMENTED();
    case MARKER_EOF:
        return 0;
    }
}


static QString
readChar(QDataStream &stream)
{
    qint8 c;
    stream >> c;
    Q_ASSERT(c >= 0);
    return QChar(static_cast<char>(c));
}


static int
readSize(QDataStream &stream)
{
    Marker type = readMarker(stream);
    return readSize(stream, type);
}


static QString
readString(QDataStream &stream, int size)
{
    char *buf = new char [size];
    stream.readRawData(buf, size);
    QString str = QString::fromUtf8(buf, size);
    delete [] buf;
    return str;
}


static QString
readString(QDataStream &stream)
{
    int size = readSize(stream);
    return readString(stream, size);
}


static QVariant
readVariant(QDataStream &stream, Marker type);


static QVariant
readArray(QDataStream &stream)
{
    Marker marker = readMarker(stream);
    if (marker == MARKER_TYPE) {
        Marker type = readMarker(stream);
        Q_ASSERT(type == MARKER_UINT8);
        Q_UNUSED(type);
        marker = readMarker(stream);
        Q_ASSERT(marker == MARKER_COUNT);
        int count = readSize(stream);
        QByteArray array(count, Qt::Uninitialized);
        int read = stream.readRawData(array.data(), count);
        Q_ASSERT(read == count);
        Q_UNUSED(read);
        return array;
    } else if (marker == MARKER_COUNT) {
        int count = readSize(stream);
        QVariantList array;
        for (int i = 0; i < count; ++i) {
            marker = readMarker(stream);
            QVariant value = readVariant(stream, marker);
            array.append(value);
        }
        return array;
    } else {
        QVariantList array;
        while (marker != MARKER_ARRAY_END &&
               marker != MARKER_EOF) {
            QVariant value = readVariant(stream, marker);
            array.append(value);
            marker = readMarker(stream);
        }
        return array;
    }
}


static QVariantMap
readObject(QDataStream &stream)
{
    QVariantMap object;
    Marker marker = readMarker(stream);
    while (marker != MARKER_OBJECT_END &&
           marker != MARKER_EOF) {
        int nameSize = readSize(stream, marker);
        QString name = readString(stream, nameSize);
        marker = readMarker(stream);
        QVariant value = readVariant(stream, marker);
        object[name] = value;
        marker = readMarker(stream);
    }
    return object;
}


static QVariant
readVariant(QDataStream &stream, Marker type)
{
    switch (type) {
    case MARKER_NULL:
        return QVariant();
    case MARKER_NOOP:
        return QVariant();
    case MARKER_TRUE:
        return true;
    case MARKER_FALSE:
         return false;
    case MARKER_INT8:
        return readInt8(stream);
    case MARKER_UINT8:
        return readUInt8(stream);
    case MARKER_INT16:
        return readInt16(stream);
    case MARKER_INT32:
        return readInt32(stream);
    case MARKER_INT64:
        return (qlonglong)readInt64(stream);
    case MARKER_FLOAT32:
        return readFloat32(stream);
    case MARKER_FLOAT64:
        return readFloat64(stream);
    case MARKER_HIGH_PRECISION:
        Q_UNIMPLEMENTED();
        return QVariant();
    case MARKER_CHAR:
        return readChar(stream);
    case MARKER_STRING:
        return readString(stream);
    case MARKER_ARRAY_BEGIN:
        return readArray(stream);
    case MARKER_OBJECT_BEGIN:
        return readObject(stream);
    case MARKER_ARRAY_END:
    case MARKER_OBJECT_END:
    case MARKER_TYPE:
    case MARKER_COUNT:
    default:
        Q_ASSERT(0);
    case MARKER_EOF:
        return QVariant();
    }
}


QVariant decodeUBJSONObject(QIODevice *io)
{
    QDataStream stream(io);
    stream.setByteOrder(QDataStream::BigEndian);
    Marker marker = readMarker(stream);
    return readVariant(stream, marker);
}

