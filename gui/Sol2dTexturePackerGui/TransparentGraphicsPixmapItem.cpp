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

#include <Sol2dTexturePackerGui/TransparentGraphicsPixmapItem.h>
#include <QStyleHints>
#include <QGuiApplication>
#include <QPainter>

TransparentGraphicsPixmapItem::TransparentGraphicsPixmapItem(const QPixmap & _pixmap) :
    m_pixmap(_pixmap)
{
}

QRectF TransparentGraphicsPixmapItem::boundingRect() const
{
    return m_pixmap.rect();
}

void TransparentGraphicsPixmapItem::paint(QPainter * _painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QStyleHints * style_hints = QGuiApplication::styleHints();
    QString brush_texture_file = style_hints->colorScheme() == Qt::ColorScheme::Dark
        ? ":/image/transparent_dark"
        : ":/image/transparent_light";
    QBrush brush(Qt::TexturePattern);
    brush.setTexture(QPixmap(brush_texture_file));
    const QRectF rect = m_pixmap.rect();
    _painter->fillRect(rect, brush);
    _painter->drawPixmap(rect, m_pixmap, rect);
}
