/**
 * The source code is based on qpixeltool.h.
 * Original license follows.
 */

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <qwidget.h>
#include <qpixmap.h>

class PixelWidget : public QWidget
{
    Q_OBJECT
public:
    PixelWidget(QWidget *parent = 0);
    ~PixelWidget();
    
    void setSurface(const QImage &image);

    QColor colorAtCurrentPosition() const;

signals:
    void zoomChanged(int);
    void mousePosition(int x, int y);
    void gridGeometry(const QRect &rect);

public:
    void timerEvent(QTimerEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    QSize sizeHint() const override;

public slots:
    void setZoom(int zoom);
    void setGridSize(int gridSize);
    void toggleGrid();
    void copyToClipboard();
    void saveToFile();
    void increaseGridSize() { setGridSize(m_gridSize + 1); }
    void decreaseGridSize() { setGridSize(m_gridSize - 1); }
    void increaseZoom() { setZoom(m_zoom + 1); }
    void decreaseZoom() { setZoom(m_zoom - 1); }

private:
    void startGridSizeVisibleTimer();
    double zoomValue() const { return m_zoom; }

    bool m_displayGridSize;
    bool m_mouseDown;

    int m_gridActive;
    int m_zoom;
    int m_gridSize;

    int m_displayGridSizeId;

    QPoint m_lastMousePos;
    QPoint m_dragStart;
    QPoint m_dragCurrent;
    QImage m_surface;

    QSize m_initialSize;
    QColor m_currentColor;
};
