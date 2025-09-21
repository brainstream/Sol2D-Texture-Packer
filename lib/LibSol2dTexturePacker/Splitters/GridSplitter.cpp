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

#include <LibSol2dTexturePacker/Splitters/GridSplitter.h>

GridSplitter::GridSplitter(QObject * _parent) :
    Splitter(_parent),
    m_options({}),
    m_is_valid(false)
{
}

void GridSplitter::reconfigure(const GridSplitterOptions & _options)
{
    m_options = _options;
    recalculate();
}

void GridSplitter::recalculate()
{
    bool has_changes = false;
    if(m_options.margin_left < 0) m_options.margin_left = 0;
    if(m_options.margin_top < 0) m_options.margin_top = 0;
    if(m_options.horizontal_spacing < 0) m_options.horizontal_spacing = 0;
    if(m_options.vertical_spacing < 0) m_options.vertical_spacing = 0;
    if(
        m_options.row_count <= 0 ||
        m_options.column_count <= 0 ||
        m_options.sprite_width <= 0 ||
        m_options.sprite_height <= 0)
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

void GridSplitter::setColumnCount(int _count)
{
    m_options.column_count = _count;
    recalculate();
}

void GridSplitter::setRowCount(int _count)
{
    m_options.row_count = _count;
    recalculate();
}

void GridSplitter::setSpriteWidth(int _width)
{
    m_options.sprite_width = _width;
    recalculate();
}

void GridSplitter::setSpriteHeight(int _height)
{
    m_options.sprite_height = _height;
    recalculate();
}

void GridSplitter::setMarginTop(int _margin)
{
    m_options.margin_top = _margin;
    recalculate();
}

void GridSplitter::setMarginLeft(int _margin)
{
    m_options.margin_left = _margin;
    recalculate();
}

void GridSplitter::setHorizontalSpacing(int _spacing)
{
    m_options.horizontal_spacing = _spacing;
    recalculate();
}

void GridSplitter::setVerticalSpacing(int _spacing)
{
    m_options.vertical_spacing = _spacing;
    recalculate();
}

bool GridSplitter::forEachFrame(std::function<void (const Frame &)> _cb) const
{
    if(!m_is_valid)
    {
        return false;
    }
    Frame frame;
    frame.width = m_options.sprite_width;
    frame.height = m_options.sprite_height;
    for(qint32 row = 0; row < m_options.row_count; ++row)
    {
        frame.y = m_options.margin_top + row * m_options.sprite_height + row * m_options.vertical_spacing;
        for(qint32 col = 0; col < m_options.column_count; ++col)
        {
            frame.x = m_options.margin_left + col * m_options.sprite_width + col * m_options.horizontal_spacing;
            _cb(frame);
        }
    }
    return true;
}

qsizetype GridSplitter::frameCount() const
{
    return m_is_valid ? m_options.column_count * m_options.row_count : 0;
}

void GridSplitter::reset()
{
    reconfigure({});
}
