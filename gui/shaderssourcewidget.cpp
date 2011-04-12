#include "shaderssourcewidget.h"

#include "glsledit.h"

ShadersSourceWidget::ShadersSourceWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_edit = new GLSLEdit(this);
    m_edit->setReadOnly(true);

    m_ui.verticalLayout->addWidget(m_edit);

    connect(m_ui.shadersCB, SIGNAL(currentIndexChanged(int)),
            SLOT(changeShader(int)));
}

void ShadersSourceWidget::setShaders(const QStringList &sources)
{
    m_sources = sources;

    m_ui.shadersCB->clear();
    m_edit->clear();

    if (m_sources.isEmpty()) {
        m_ui.shadersCB->setDisabled(true);
        m_edit->setPlainText(tr("No bound shaders."));
        m_edit->setDisabled(true);
        return;
    }

    m_ui.shadersCB->setEnabled(true);
    m_edit->setEnabled(true);

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
    m_edit->setPlainText(m_sources.value(idx));
}

#include "shaderssourcewidget.moc"
