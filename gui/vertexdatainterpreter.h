#pragma once

#include <QObject>

class QListWidget;

enum DataType {
    DT_INT8,
    DT_UINT8,
    DT_INT16,
    DT_UINT16,
    DT_INT32,
    DT_UINT32,
    DT_FLOAT,
    DT_DOUBLE,
};

class VertexDataInterpreter : public QObject
{
    Q_OBJECT
public:
    VertexDataInterpreter(QObject *parent=0);

    QByteArray data() const;

    int type() const;
    int stride() const;
    int components() const;
    int startingOffset() const;

    void setListWidget(QListWidget *listWidget);

public slots:
    void interpretData();

    void setData(const QByteArray &data);
    void setTypeFromString(const QString &str);
    void setStride(int stride);
    void setComponents(int num);
    void setType(int type);
    void setStartingOffset(int offset);

private:
    QListWidget *m_listWidget;
    QByteArray m_data;
    int m_type;
    int m_stride;
    int m_components;
    int m_startingOffset;
};
