#include "vertexdatainterpreter.h"

#include <QListWidget>
#include <QStringList>

#include <QDebug>

#include <GL/gl.h>

#include <qmath.h>

static int
sizeForType(int glType)
{
    switch(glType) {
    case GL_FLOAT:
        return sizeof(GLfloat);
    case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    case GL_BYTE:
        return sizeof(GLbyte);
    case GL_SHORT:
        return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
    case GL_INT:
        return sizeof(GLint);
    case GL_UNSIGNED_INT:
        return sizeof(GLuint);
    case GL_DOUBLE:
        return sizeof(GLdouble);
    default:
        return sizeof(GLint);
    }
}

template <typename T>
static QStringList
convertData(const QByteArray &dataArray,
            int type,
            int stride,
            int numComponents)
{
    QStringList strings;

    const char *data = dataArray.constData();
    int typeSize = sizeForType(type);
    int elementSize = numComponents * typeSize;

    if (!stride)
        stride = elementSize;

    int numElements = dataArray.size() / stride;

    if ((numElements % numComponents) != 0) {
        int temp = qFloor(dataArray.size() / (float)stride);
        int fullElemSize = temp * stride;
        if (fullElemSize + numComponents * typeSize <= dataArray.size()){
            /* num full elements plus the part of the buffer in which we fit */
            numElements = temp + 1;
        } else {
            numElements = temp;
        }
    }

#if 0
    qDebug() << "numElements = "<<numElements;
    qDebug() << "elementSize = "<<elementSize;
    qDebug() << "stride = "<<stride;
    qDebug() << "numComponents = "<<numComponents;
    qDebug() << "typeSize = "<<typeSize;
#endif


    for (int i = 0; i < numElements; ++i) {
        QString vectorString = QString::fromLatin1("%1) [").arg(i);
        for (int j = 0; j < numComponents; ++j) {
            int offset = i*stride + j*typeSize;
            const T *elementPtr =
                (const T*)(data + offset);
            T elem = *elementPtr;
            vectorString += QString::number(elem);
            if ((j + 1) < numComponents)
                vectorString += QLatin1String(", ");
        }
        vectorString += "]";
        strings += vectorString;
    }

    return strings;
}


VertexDataInterpreter::VertexDataInterpreter(QObject *parent)
    : QObject(parent),
      m_listWidget(0),
      m_type(GL_FLOAT),
      m_stride(16),
      m_components(4)
{
}

void VertexDataInterpreter::setData(const QByteArray &data)
{
    m_data = data;
    if (m_listWidget)
        m_listWidget->clear();
}

QByteArray VertexDataInterpreter::data() const
{
    return m_data;
}

void VertexDataInterpreter::setType(int type)
{
    m_type = type;
}

int VertexDataInterpreter::type() const
{
    return m_type;
}

void VertexDataInterpreter::setStride(int stride)
{
    m_stride = stride;
}

int VertexDataInterpreter::stride() const
{
    return m_stride;
}

void VertexDataInterpreter::setComponents(int num)
{
    m_components = num;
}

int VertexDataInterpreter::components() const
{
    return m_components;
}

void VertexDataInterpreter::setListWidget(QListWidget *listWidget)
{
    m_listWidget = listWidget;
}

void VertexDataInterpreter::interpretData()
{
    if (!m_listWidget)
        return;

    m_listWidget->clear();

    if (m_data.isEmpty() || !m_components)
        return;

    QStringList lst;
    switch(m_type) {
    case GL_FLOAT:
        lst = convertData<float>(m_data, m_type, m_stride, m_components);
        break;
    case GL_UNSIGNED_BYTE:
        lst = convertData<quint8>(m_data, m_type, m_stride, m_components);
        break;
    case GL_BYTE:
        lst = convertData<qint8>(m_data, m_type, m_stride, m_components);
        break;
    case GL_SHORT:
        lst = convertData<qint16>(m_data, m_type, m_stride, m_components);
        break;
    case GL_UNSIGNED_SHORT:
        lst = convertData<quint16>(m_data, m_type, m_stride, m_components);
        break;
    case GL_INT:
        lst = convertData<unsigned int>(m_data, m_type, m_stride, m_components);
        break;
    case GL_UNSIGNED_INT:
        lst = convertData<int>(m_data, m_type, m_stride, m_components);
        break;
    case GL_DOUBLE:
        lst = convertData<double>(m_data, m_type, m_stride, m_components);
        break;
    default:
        qDebug()<<"unkown gltype = "<<m_type;
    }
    //qDebug()<<"list is "<<lst;
    m_listWidget->addItems(lst);
}


void VertexDataInterpreter::setTypeFromString(const QString &str)
{
    if (str == QLatin1String("GL_FLOAT")) {
        setType(GL_FLOAT);
    } else if (str == QLatin1String("GL_INT")) {
        setType(GL_INT);
    } else if (str == QLatin1String("GL_UNSIGNED_INT")) {
        setType(GL_UNSIGNED_INT);
    } else if (str == QLatin1String("GL_SHORT")) {
        setType(GL_SHORT);
    } else if (str == QLatin1String("GL_UNSIGNED_SHORT")) {
        setType(GL_UNSIGNED_SHORT);
    } else if (str == QLatin1String("GL_BYTE")) {
        setType(GL_BYTE);
    } else if (str == QLatin1String("GL_UNSIGNED_BYTE")) {
        setType(GL_UNSIGNED_BYTE);
    } else if (str == QLatin1String("GL_DOUBLE")) {
        setType(GL_DOUBLE);
    } else {
        qDebug()<<"unknown vertex data type";
    }
}

#include "vertexdatainterpreter.moc"
