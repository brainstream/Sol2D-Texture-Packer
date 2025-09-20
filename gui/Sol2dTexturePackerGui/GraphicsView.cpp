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
#include <QStyleHints>
#include <QGuiApplication>
#include <QWheelEvent>

GraphicsView::GraphicsView(QWidget * _parent) :
    QGraphicsView(_parent),
    m_zoom_model(nullptr)
{
    QStyleHints * style_hints = QGuiApplication::styleHints();
    applyColorScheme(style_hints->colorScheme());
    connect(style_hints, &QStyleHints::colorSchemeChanged, this, &GraphicsView::applyColorScheme);
}

void GraphicsView::applyColorScheme(Qt::ColorScheme _scheme)
{
    QString brush_texture_file = _scheme == Qt::ColorScheme::Dark
        ? ":/image/transparent_dark"
        : ":/image/transparent_light";
    QBrush brush(Qt::TexturePattern);
    brush.setTexture(QPixmap(brush_texture_file));
    setBackgroundBrush(brush);
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
            m_zoom_model->increment();
        else if(delta.y() < 0)
            m_zoom_model->decrement();
    }
}

void GraphicsView::keyPressEvent(QKeyEvent * _event)
{
    bool accept = true;
    if(_event->modifiers().testFlag(Qt::ControlModifier))
    {
        switch(_event->key())
        {
        case Qt::Key_A:
            foreach(QGraphicsItem * item, scene()->items())
                item->setSelected(true);
            break;
        case Qt::Key_0:
            if(m_zoom_model) m_zoom_model->setZoom(100);
            break;
        case Qt::Key_Plus:
            if(m_zoom_model) m_zoom_model->increment();
            break;
        case Qt::Key_Minus:
            if(m_zoom_model) m_zoom_model->decrement();
            break;
        default:
            accept = false;
            break;
        }
    }
    else
    {
        switch(_event->key())
        {
        case Qt::Key_Escape:
            foreach(QGraphicsItem * item, scene()->items())
                item->setSelected(false);
            break;
        case Qt::Key_ZoomIn:
            if(m_zoom_model) m_zoom_model->increment();
            break;
        case Qt::Key_ZoomOut:
            if(m_zoom_model) m_zoom_model->decrement();
            break;
        default:
            accept = false;
            break;
        }
    }
    if(accept)
        _event->accept();
    else
        QGraphicsView::keyPressEvent(_event);
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
