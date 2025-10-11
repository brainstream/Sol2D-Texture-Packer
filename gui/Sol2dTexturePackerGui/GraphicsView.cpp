/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Sol2D Texture Packer.                                                         *
 *                                                                                                        *
 * Sol2D Texture Packer is free software: you can redistribute it and/or modify it under  the terms of    *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Sol2D Texture Packer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#include <Sol2dTexturePackerGui/GraphicsView.h>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QShortcut>
#include <QScrollBar>

GraphicsView::GraphicsView(QWidget * _parent) :
    QGraphicsView(_parent),
    m_zoom_model(nullptr),
    m_is_in_drag_mode(false)
{
    setMouseTracking(true);

    connect(
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this),
        &QShortcut::activated,
        this,
        &GraphicsView::selectAll);
    connect(
        new QShortcut(QKeySequence(Qt::Key_Escape), this),
        &QShortcut::activated,
        this,
        &GraphicsView::clearSelection);
    connect(
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus), this),
        &QShortcut::activated,
        this,
        &GraphicsView::incrementZoom);
    connect(
        new QShortcut(QKeySequence(Qt::Key_ZoomIn), this),
        &QShortcut::activated,
        this,
        &GraphicsView::incrementZoom);
    connect(
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this),
        &QShortcut::activated,
        this,
        &GraphicsView::decrementZoom);
    connect(
        new QShortcut(QKeySequence(Qt::Key_ZoomOut), this),
        &QShortcut::activated,
        this,
        &GraphicsView::decrementZoom);
    connect(
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_0), this),
        &QShortcut::activated,
        this,
        &GraphicsView::resetZoom);
}

void GraphicsView::drawBackground(QPainter * _painter, const QRectF &)
{
    _painter->save();
    _painter->resetTransform();
    _painter->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    _painter->restore();
}

void GraphicsView::wheelEvent(QWheelEvent * _event)
{
    if(m_zoom_model && _event->modifiers().testFlag(Qt::ControlModifier))
    {
        QPoint delta = _event->angleDelta();
        if(delta.y() > 0)
            incrementZoom();
        else if(delta.y() < 0)
            decrementZoom();
    }
}

void GraphicsView::incrementZoom()
{
    if(m_zoom_model)
        m_zoom_model->increment();
}

void GraphicsView::decrementZoom()
{
    if(m_zoom_model)
        m_zoom_model->decrement();
}

void GraphicsView::resetZoom()
{
    if(m_zoom_model)
        m_zoom_model->setZoom(100);
}

void GraphicsView::mousePressEvent(QMouseEvent * _event)
{
    switch(_event->button())
    {
    case Qt::LeftButton:
        QGraphicsView::mousePressEvent(_event);
        break;
    case Qt::MiddleButton:
        m_is_in_drag_mode = true;
        m_prev_drag_point = _event->pos();
        setCursor(Qt::ClosedHandCursor);
        break;
    default:
        _event->ignore();
        break;
    }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent * _event)
{
    switch(_event->button())
    {
    case Qt::LeftButton:
        QGraphicsView::mouseReleaseEvent(_event);
        break;
    case Qt::MiddleButton:
        m_is_in_drag_mode = false;
        unsetCursor();
        break;
    default:
        _event->ignore();
        break;
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent * _event)
{
    if(m_is_in_drag_mode && _event->buttons().testFlag(Qt::MiddleButton))
    {
        QPoint delta = _event->pos() - m_prev_drag_point;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_prev_drag_point = _event->pos();
    }
    QGraphicsView::mouseMoveEvent(_event);
}

void GraphicsView::setZoomModel(ZoomModel * _model)
{
    if(m_zoom_model)
    {
        disconnect(m_zoom_model, &ZoomModel::zoomChanged, this, &GraphicsView::applyZoom);
        m_zoom_model = nullptr;
    }
    m_zoom_model = _model;
    if(m_zoom_model)
    {
        connect(m_zoom_model, &ZoomModel::zoomChanged, this, &GraphicsView::applyZoom);
        applyZoom(m_zoom_model->zoom());
    }
}

void GraphicsView::applyZoom(quint32 _zoom)
{
    double scale_factor = _zoom / 100.0;
    resetTransform();
    scale(scale_factor, scale_factor);
}

void GraphicsView::selectAll()
{
    if(QGraphicsScene * scene = this->scene())
    {
        foreach(QGraphicsItem * item, scene->items())
            item->setSelected(true);
    }
}

void GraphicsView::clearSelection()
{
    if(QGraphicsScene * scene = this->scene())
    {
        foreach(QGraphicsItem * item, scene->selectedItems())
        item->setSelected(false);
    }
}
