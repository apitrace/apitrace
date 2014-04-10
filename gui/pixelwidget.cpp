/**
 * The source code is based on qpixeltool.cpp.
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

#include "pixelwidget.h"

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qapplication.h>
#ifndef QT_NO_CLIPBOARD
#include <qclipboard.h>
#endif
#include <qpainter.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qmenu.h>
#include <qactiongroup.h>

#include <qdebug.h>

PixelWidget::PixelWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(QLatin1String("PixelTool"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_gridSize = 1;
    m_gridActive = 0;
    m_zoom = 1;
    m_displayGridSize = false;
    m_displayGridSizeId = 0;

    m_mouseDown = false;

    m_initialSize = QSize(250, 250);

    setMouseTracking(true);
    setAutoFillBackground(true);
}

PixelWidget::~PixelWidget()
{
}

void PixelWidget::setSurface(const QImage &image)
{
    m_surface = image;
    updateGeometry();
    update();
}

void PixelWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_displayGridSizeId) {
        killTimer(m_displayGridSizeId);
        m_displayGridSize = false;
    }
}

void render_string(QPainter *p, int w, int h, const QString &text, int flags)
{
    p->setBrush(QColor(255, 255, 255, 191));
    p->setPen(Qt::black);
    QRect bounds;
    p->drawText(0, 0, w, h, Qt::TextDontPrint | flags, text, &bounds);

    if (bounds.x() == 0) bounds.adjust(0, 0, 10, 0);
    else bounds.adjust(-10, 0, 0, 0);

    if (bounds.y() == 0) bounds.adjust(0, 0, 0, 10);
    else bounds.adjust(0, -10, 0, 0);

    p->drawRect(bounds);
    p->drawText(bounds, flags, text);
}

void PixelWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    int w = width();
    int h = height();

    p.save();
    p.scale(zoomValue(), zoomValue());
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0, 0, m_surface);
    p.scale(1/zoomValue(), 1/zoomValue());
    p.restore();

    // Draw the grid on top.
    if (m_gridActive) {
        p.setPen(m_gridActive == 1 ? Qt::black : Qt::white);
        int incr = m_gridSize * zoomValue();
        for (int x=0; x<w; x+=incr)
            p.drawLine(x, 0, x, h);
        for (int y=0; y<h; y+=incr)
            p.drawLine(0, y, w, y);
    }

    QFont f(QLatin1String("courier"));
    f.setBold(true);
    p.setFont(f);

    if (m_displayGridSize) {
        render_string(&p, w, h,
            QString::fromLatin1("Grid size: %1").arg(m_gridSize),
                      Qt::AlignBottom | Qt::AlignLeft);
    }

    if (m_mouseDown && m_dragStart != m_dragCurrent) {
        int x1 = (m_dragStart.x() / zoomValue()) * zoomValue();
        int y1 = (m_dragStart.y() / zoomValue()) * zoomValue();
        int x2 = (m_dragCurrent.x() / zoomValue()) * zoomValue();
        int y2 = (m_dragCurrent.y() / zoomValue()) * zoomValue();
        QRect r = QRect(x1, y1, x2 - x1, y2 - y1).normalized();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::red, 3, Qt::SolidLine));
        p.drawRect(r);
        p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        p.drawRect(r);

        QRect rect(
            int(r.x() / zoomValue()),
            int(r.y() / zoomValue()),
            int(r.width() / zoomValue()),
            int(r.height() / zoomValue()));
        emit gridGeometry(rect);
    } else {
        QRect empty;
        emit gridGeometry(empty);
    }


}

void PixelWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Plus:
        increaseZoom();
        break;
    case Qt::Key_Minus:
        decreaseZoom();
        break;
    case Qt::Key_PageUp:
        setGridSize(m_gridSize + 1);
        break;
    case Qt::Key_PageDown:
        setGridSize(m_gridSize - 1);
        break;
    case Qt::Key_G:
        toggleGrid();
        break;
    case Qt::Key_C:
        if (e->modifiers() & Qt::ControlModifier)
            copyToClipboard();
        break;
    case Qt::Key_S:
        if (e->modifiers() & Qt::ControlModifier) {
            releaseKeyboard();
            saveToFile();
        }
        break;
    case Qt::Key_Control:
        grabKeyboard();
        break;
    }
}

void PixelWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Qt::Key_Control:
        releaseKeyboard();
        break;
    default:
        break;
    }
}

void PixelWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void PixelWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mouseDown)
        m_dragCurrent = e->pos();

    int x = e->x() / zoomValue();
    int y = e->y() / zoomValue();

    if (x < m_surface.width() && y < m_surface.height() && x >= 0 && y >= 0) {
        m_currentColor = m_surface.pixel(x, y);
    } else
        m_currentColor = QColor();

    emit mousePosition(x, y);
    update();
}

void PixelWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_mouseDown = true;
    m_dragStart = e->pos();
}

void PixelWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_mouseDown = false;
}

void PixelWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;

    QAction title(QLatin1String("Surface Pixel Tool"), &menu);
    title.setEnabled(false);

    // Grid color options...
    QActionGroup gridGroup(this);
    QAction whiteGrid(QLatin1String("White grid"), &gridGroup);
    whiteGrid.setCheckable(true);
    whiteGrid.setChecked(m_gridActive == 2);
    whiteGrid.setShortcut(QKeySequence(Qt::Key_G));
    QAction blackGrid(QLatin1String("Black grid"), &gridGroup);
    blackGrid.setCheckable(true);
    blackGrid.setChecked(m_gridActive == 1);
    blackGrid.setShortcut(QKeySequence(Qt::Key_G));
    QAction noGrid(QLatin1String("No grid"), &gridGroup);
    noGrid.setCheckable(true);
    noGrid.setChecked(m_gridActive == 0);
    noGrid.setShortcut(QKeySequence(Qt::Key_G));

    // Grid size options
    QAction incrGrid(QLatin1String("Increase grid size"), &menu);
    incrGrid.setShortcut(QKeySequence(Qt::Key_PageUp));
    connect(&incrGrid, SIGNAL(triggered()), this, SLOT(increaseGridSize()));
    QAction decrGrid(QLatin1String("Decrease grid size"), &menu);
    decrGrid.setShortcut(QKeySequence(Qt::Key_PageDown));
    connect(&decrGrid, SIGNAL(triggered()), this, SLOT(decreaseGridSize()));

    // Zoom options
    QAction incrZoom(QLatin1String("Zoom in"), &menu);
    incrZoom.setShortcut(QKeySequence(Qt::Key_Plus));
    connect(&incrZoom, SIGNAL(triggered()), this, SLOT(increaseZoom()));
    QAction decrZoom(QLatin1String("Zoom out"), &menu);
    decrZoom.setShortcut(QKeySequence(Qt::Key_Minus));
    connect(&decrZoom, SIGNAL(triggered()), this, SLOT(decreaseZoom()));

    // Copy to clipboard / save
    QAction save(QLatin1String("Save as image"), &menu);
    save.setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    connect(&save, SIGNAL(triggered()), this, SLOT(saveToFile()));
#ifndef QT_NO_CLIPBOARD
    QAction copy(QLatin1String("Copy to clipboard"), &menu);
    copy.setShortcut(QKeySequence(QLatin1String("Ctrl+C")));
    connect(&copy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
#endif

    menu.addAction(&title);
    menu.addSeparator();
    menu.addAction(&whiteGrid);
    menu.addAction(&blackGrid);
    menu.addAction(&noGrid);
    menu.addSeparator();
    menu.addAction(&incrGrid);
    menu.addAction(&decrGrid);
    menu.addSeparator();
    menu.addAction(&incrZoom);
    menu.addAction(&decrZoom);
    menu.addSeparator();
    menu.addAction(&save);
#ifndef QT_NO_CLIPBOARD
    menu.addAction(&copy);
#endif

    menu.exec(mapToGlobal(e->pos()));

    if (noGrid.isChecked()) m_gridActive = 0;
    else if (blackGrid.isChecked()) m_gridActive = 1;
    else m_gridActive = 2;
}

QSize PixelWidget::sizeHint() const
{
    if (m_surface.isNull())
        return m_initialSize;

    QSize sz(m_surface.width() * zoomValue(),
             m_surface.height() * zoomValue());
    return sz;
}

void PixelWidget::startGridSizeVisibleTimer()
{
    if (m_gridActive) {
        if (m_displayGridSizeId > 0)
            killTimer(m_displayGridSizeId);
        m_displayGridSizeId = startTimer(5000);
        m_displayGridSize = true;
        update();
    }
}

void PixelWidget::setZoom(int zoom)
{
    if (zoom > 0 && zoom != m_zoom) {
        QPoint pos = m_lastMousePos;
        m_lastMousePos = QPoint();
        m_zoom = zoom;
        m_lastMousePos = pos;
        m_dragStart = m_dragCurrent = QPoint();

        if (m_zoom == 1)
            m_gridActive = 0;
        else if (!m_gridActive)
            m_gridActive = 1;

        zoomChanged(m_zoom);
        updateGeometry();
        update();
    }
}

void PixelWidget::toggleGrid()
{
    if (++m_gridActive > 2)
        m_gridActive = 0;
    update();
}

void PixelWidget::setGridSize(int gridSize)
{
    if (m_gridActive && gridSize > 0) {
        m_gridSize = gridSize;
        startGridSizeVisibleTimer();
        update();
    }
}

void PixelWidget::copyToClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setImage(m_surface);
}

void PixelWidget::saveToFile()
{
    QString name = QFileDialog::getSaveFileName(this, QLatin1String("Save as image"), QString(), QLatin1String("*.png"));
    if (!name.isEmpty()) {
        if (!name.endsWith(QLatin1String(".png")))
            name.append(QLatin1String(".png"));
        m_surface.save(name, "PNG");
    }
}

QColor PixelWidget::colorAtCurrentPosition() const
{
    return m_currentColor;
}

#include "pixelwidget.moc"
