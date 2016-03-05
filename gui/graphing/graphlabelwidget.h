#pragma once

#include <QWidget>
#include <QPainter>

/**
 * A very simple label widget, basically a box with text in.
 */
class GraphLabelWidget : public QWidget {
public:
    GraphLabelWidget(QString text = QString(), QWidget* parent = 0) :
        QWidget(parent),
        m_flags(Qt::AlignHCenter | Qt::AlignVCenter),
        m_text(text)
    {
    }

    void setText(const QString& text)
    {
        m_text = text;
    }

    void setFlags(int flags)
    {
        m_flags = flags;
    }

    virtual void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setPen(Qt::black);
        painter.fillRect(rect(), Qt::lightGray);
        painter.drawText(rect(), m_flags, m_text);
    }

protected:
    int m_flags;
    QString m_text;
};
