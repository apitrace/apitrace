#ifndef VERTEXDATAINTERPRETER_H
#define VERTEXDATAINTERPRETER_H

#include <QObject>

class QListWidget;

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

#endif
