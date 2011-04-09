#include "shaderssourcewidget.h"

ShadersSourceWidget::ShadersSourceWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.shadersCB, SIGNAL(currentIndexChanged(int)),
            SLOT(changeShader(int)));
}

void ShadersSourceWidget::setShaders(const QStringList &sources)
{
    m_sources = sources;

    m_ui.shadersCB->clear();
    m_ui.shadersTextEdit->clear();

    if (m_sources.isEmpty()) {
        m_ui.shadersCB->setDisabled(true);
        m_ui.shadersTextEdit->setPlainText(
            tr("No bound shaders."));
        m_ui.shadersTextEdit->setDisabled(true);
        return;
    }

    m_ui.shadersCB->setEnabled(true);
    m_ui.shadersTextEdit->setEnabled(true);

    for (int i = 0; i < m_sources.count(); ++i) {
        QString source = m_sources[i];
        m_ui.shadersCB->insertItem(
            i,
            tr("Shader %1").arg(i));
    }
    m_ui.shadersCB->setCurrentIndex(0);
}

void ShadersSourceWidget::changeShader(int idx)
{
    m_ui.shadersTextEdit->setPlainText(m_sources.value(idx));
}

#include "shaderssourcewidget.moc"
