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

constexpr double g_zoom_step = 1.0;
constexpr double g_min_zoom = 1.0;
constexpr double g_max_zoom = 100000.0;

} // namespace

ZoomModel::ZoomModel(quint32 _zoom, QObject * _parent) :
    QObject(_parent),
    m_zoom(_zoom)
{
}

void ZoomModel::increment()
{
    const double factor = 1.0 + g_zoom_step / std::log(m_zoom + 1.0);
    const double new_zoom = m_zoom * factor;
    if(new_zoom <= g_max_zoom)
        setZoom(static_cast<quint32>(new_zoom));
}

void ZoomModel::decrement()
{
    const double factor = 1.0 + g_zoom_step / std::log(m_zoom + 1.0);
    const double new_zoom = m_zoom / factor;
    if(new_zoom >= g_min_zoom)
        setZoom(static_cast<quint32>(new_zoom));
}

void ZoomModel::setZoom(quint32 _zoom)
{
    if(_zoom > 0 && _zoom != m_zoom)
    {
        m_zoom = _zoom;
        emit zoomChanged(_zoom);
    }
}
