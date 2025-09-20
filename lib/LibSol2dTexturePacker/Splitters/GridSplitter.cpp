/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Open Sprite Sheet Tools.                                                      *
 *                                                                                                        *
 * Open Sprite Sheet Tools is free software: you can redistribute it and/or modify it under  the terms of *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Open Sprite Sheet Tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#include <LibSol2dTexturePacker/Splitters/GridSplitter.h>

GridSplitter::GridSplitter(QObject * _parent) :
    Splitter(_parent),
    m_column_count(0),
    m_row_count(0),
    m_sprite_width(0),
    m_sprite_height(0),
    m_margin_top(0),
    m_margin_left(0),
    m_horizontal_spacing(0),
    m_vertical_spacing(0),
    m_is_valid(false)
{
}

void GridSplitter::recalculate()
{
    bool has_changes = false;
    if(m_margin_left < 0) m_margin_left = 0;
    if(m_margin_top < 0) m_margin_top = 0;
    if(m_horizontal_spacing < 0) m_horizontal_spacing = 0;
    if(m_vertical_spacing < 0) m_vertical_spacing = 0;
    if(m_row_count <= 0 || m_column_count <= 0 || m_sprite_width <= 0 || m_sprite_height <= 0)
    {
        has_changes = m_is_valid;
        m_is_valid = false;
    }
    else
    {
        m_is_valid = true;
        has_changes = true;
    }
    if(has_changes)
    {
        emit framesChanged();
    }
}

bool GridSplitter::forEachFrame(std::function<void (const Frame &)> _cb) const
{
    if(!m_is_valid)
    {
        return false;
    }
    Frame frame;
    frame.width = m_sprite_width;
    frame.height = m_sprite_height;
    for(qint32 row = 0; row < m_row_count; ++row)
    {
        frame.y = m_margin_top + row * m_sprite_height + row * m_vertical_spacing;
        for(qint32 col = 0; col < m_column_count; ++col)
        {
            frame.x = m_margin_left + col * m_sprite_width + col * m_horizontal_spacing;
            _cb(frame);
        }
    }
    return true;
}

qsizetype GridSplitter::frameCount() const
{
    return m_is_valid ? m_column_count * m_row_count : 0;
}

void GridSplitter::reset()
{
    bool has_changes = m_is_valid;
    m_column_count = 0;
    m_row_count = 0;
    m_sprite_width = 0;
    m_sprite_height = 0;
    m_margin_top = 0;
    m_margin_left = 0;
    m_horizontal_spacing = 0;
    m_vertical_spacing = 0;
    m_is_valid = false;
    if(has_changes)
        emit framesChanged();
}
