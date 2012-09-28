#include "heatmapverticalaxiswidget.h"

#include <qmath.h>
#include <QPainter>
#include <QMouseEvent>

HeatmapVerticalAxisWidget::HeatmapVerticalAxisWidget(QWidget* parent) :
    GraphAxisWidget(parent),
    m_data(NULL)
{
    m_rowHeight = 20;
}

void HeatmapVerticalAxisWidget::setDataProvider(HeatmapDataProvider* data)
{
    delete m_data;
    m_data = data;

    m_valueMin = 0;
    m_valueMax = (data->dataRows() + data->headerRows()) * m_rowHeight;

    m_valueBegin = m_valueMin;
    m_valueEnd = m_valueMax;

    update();
}

void HeatmapVerticalAxisWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->pos().y() < m_data->headerRows() * m_rowHeight) {
        m_selectionState->type = SelectionState::None;
        emit selectionChanged();
    } else {
        int row = e->pos().y();
        row -= m_data->headerRows() * m_rowHeight;
        row += m_valueBegin;
        row /= m_rowHeight;

        if (row >= m_data->dataRows()) {
            m_selectionState->type = SelectionState::None;
            emit selectionChanged();
        } else {
            m_selectionState->type = SelectionState::Vertical;
            m_selectionState->start = m_data->dataRowAt(row);
            m_selectionState->end = m_selectionState->start;
            emit selectionChanged();
        }
    }
}

void HeatmapVerticalAxisWidget::paintEvent(QPaintEvent *)
{
    if (!m_data) {
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    /* Draw scrollable rows */
    painter.translate(0, m_data->headerRows() * m_rowHeight - m_valueBegin % m_rowHeight);
    int rowStart = m_valueBegin / m_rowHeight;
    int rowEnd = qMin<int>(qCeil(m_valueEnd / (double)m_rowHeight), m_data->dataRows());

    for (unsigned i = rowStart; i < rowEnd; ++i) {
        painter.drawText(0, 0, width(), m_rowHeight, Qt::AlignHCenter | Qt::AlignVCenter, m_data->dataLabel(i));
        painter.drawLine(0, m_rowHeight, width(), m_rowHeight);
        painter.translate(0, m_rowHeight);
    }

    /* Draw fixed position headers */
    painter.resetTransform();
    painter.drawRect(0, 0, width() - 1, m_data->headerRows() * m_rowHeight);

    for (unsigned i = 0; i < m_data->headerRows(); ++i) {
        painter.drawText(0, 0, width(), m_rowHeight, Qt::AlignHCenter | Qt::AlignVCenter, m_data->headerLabel(i));
        painter.drawLine(0, m_rowHeight, width(), m_rowHeight);
        painter.translate(0, m_rowHeight);
    }
}
