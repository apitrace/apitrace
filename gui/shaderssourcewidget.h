#pragma once

#include "ui_shaderssourcewidget.h"
#include <QWidget>

class GLSLEdit;

class ShadersSourceWidget : public QWidget
{
    Q_OBJECT
public:
    ShadersSourceWidget(QWidget *parent=0);

public slots:
    void setShaders(const QMap<QString, QString> &sources);

private slots:
    void changeShader(const QString &key);
private:
    Ui::ShadersSourceWidget m_ui;
    QMap<QString, QString> m_sources;
    GLSLEdit *m_edit;
};
