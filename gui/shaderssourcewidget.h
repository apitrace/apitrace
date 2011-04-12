#ifndef SHADERSSOURCEWIDGET_H
#define SHADERSSOURCEWIDGET_H

#include "ui_shaderssourcewidget.h"
#include <QWidget>

class GLSLEdit;

class ShadersSourceWidget : public QWidget
{
    Q_OBJECT
public:
    ShadersSourceWidget(QWidget *parent=0);

public slots:
    void setShaders(const QStringList &sources);

private slots:
    void changeShader(int idx);
private:
    Ui::ShadersSourceWidget m_ui;
    QStringList m_sources;
    GLSLEdit *m_edit;
};

#endif
