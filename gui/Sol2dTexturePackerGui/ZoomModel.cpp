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

#include <Sol2dTexturePackerGui/ZoomModel.h>

namespace {

constexpr quint32 g_zoom_step = 5;

} // namespace

ZoomModel::ZoomModel(quint32 _zoom, QObject * _parent) :
    QObject(_parent),
    m_zoom(_zoom)
{
}

void ZoomModel::increment()
{
    setZoom(m_zoom + g_zoom_step);
}

void ZoomModel::decrement()
{
    if(m_zoom <= g_zoom_step)
        setZoom(1);
    else
        setZoom(m_zoom - g_zoom_step);
}

void ZoomModel::setZoom(quint32 _zoom)
{
    if(_zoom > 0 && _zoom != m_zoom)
    {
        m_zoom = _zoom;
        emit zoomChanged(_zoom);
    }
}
