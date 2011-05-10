#include "vertexdatainterpreter.h"

#include <QListWidget>
#include <QStringList>

#include <QDebug>

#include <qmath.h>

static int
sizeForType(int type)
{
    switch(type) {
    case DT_INT8:
    case DT_UINT8:
        return 1;
    case DT_INT16:
    case DT_UINT16:
        return 2;
    case DT_INT32:
    case DT_UINT32:
        return 4;
    case DT_FLOAT:
        return sizeof(float);
    case DT_DOUBLE:
        return sizeof(double);
    default:
        return sizeof(int);
    }
}

template <typename T>
static QStringList
convertData(const QByteArray &dataArray,
            int type,
            int stride,
            int numComponents,
            int startingOffset)
{
    QStringList strings;

    int dataSize = dataArray.size() - startingOffset;
    const char *data = dataArray.constData() + startingOffset;
    int typeSize = sizeForType(type);
    int elementSize = numComponents * typeSize;

    if (!stride)
        stride = elementSize;

    int numElements = dataSize / stride;

    if ((numElements % numComponents) != 0) {
        int temp = qFloor(dataSize / (float)stride);
        int fullElemSize = temp * stride;
        if (fullElemSize + numComponents * typeSize <= dataSize) {
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
      m_type(DT_FLOAT),
      m_stride(16),
      m_components(4),
      m_startingOffset(0)
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
    case DT_INT8:
        lst = convertData<qint8>(m_data, m_type, m_stride, m_components,
                                 m_startingOffset);
        break;
    case DT_UINT8:
        lst = convertData<quint8>(m_data, m_type, m_stride, m_components,
                                  m_startingOffset);
        break;
    case DT_INT16:
        lst = convertData<qint16>(m_data, m_type, m_stride, m_components,
                                  m_startingOffset);
        break;
    case DT_UINT16:
        lst = convertData<quint16>(m_data, m_type, m_stride, m_components,
                                   m_startingOffset);
        break;
    case DT_INT32:
        lst = convertData<qint32>(m_data, m_type, m_stride, m_components,
                                  m_startingOffset);
        break;
    case DT_UINT32:
        lst = convertData<quint32>(m_data, m_type, m_stride, m_components,
                                   m_startingOffset);
        break;
    case DT_FLOAT:
        lst = convertData<float>(m_data, m_type, m_stride, m_components,
                                 m_startingOffset);
        break;
    case DT_DOUBLE:
        lst = convertData<double>(m_data, m_type, m_stride, m_components,
                                  m_startingOffset);
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
        setType(DT_FLOAT);
    } else if (str == QLatin1String("GL_INT")) {
        setType(DT_INT32);
    } else if (str == QLatin1String("GL_UNSIGNED_INT")) {
        setType(DT_UINT32);
    } else if (str == QLatin1String("GL_SHORT")) {
        setType(DT_INT16);
    } else if (str == QLatin1String("GL_UNSIGNED_SHORT")) {
        setType(DT_UINT16);
    } else if (str == QLatin1String("GL_BYTE")) {
        setType(DT_INT8);
    } else if (str == QLatin1String("GL_UNSIGNED_BYTE")) {
        setType(DT_UINT8);
    } else if (str == QLatin1String("GL_DOUBLE")) {
        setType(DT_DOUBLE);
    } else {
        qDebug()<<"unknown vertex data type";
    }
}

int VertexDataInterpreter::startingOffset() const
{
    return m_startingOffset;
}

void VertexDataInterpreter::setStartingOffset(int offset)
{
    m_startingOffset = offset;
}

#include "vertexdatainterpreter.moc"
