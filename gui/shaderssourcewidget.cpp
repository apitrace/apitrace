#include "shaderssourcewidget.h"

#include "glsledit.h"

ShadersSourceWidget::ShadersSourceWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_edit = new GLSLEdit(this);
    m_edit->setReadOnly(true);

    m_ui.verticalLayout->addWidget(m_edit);

    connect(m_ui.shadersCB, SIGNAL(currentIndexChanged(const QString &)),
            SLOT(changeShader(const QString &)));
}

void ShadersSourceWidget::setShaders(const QMap<QString, QString> &sources)
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

    QMap<QString, QString>::const_iterator itr;
    for (itr = m_sources.constBegin(); itr != m_sources.constEnd(); ++itr) {
        m_ui.shadersCB->addItem(itr.key());
    }
    m_ui.shadersCB->setCurrentIndex(0);
}

void ShadersSourceWidget::changeShader(const QString &key)
{
    m_edit->setPlainText(m_sources.value(key));
}

#include "shaderssourcewidget.moc"
