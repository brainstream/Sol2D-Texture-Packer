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

#include <LibSol2dTexturePacker/Pack/GridPack.h>
#include <LibSol2dTexturePacker/Exception.h>

GridPack::GridPack(const QString & _texture_filename, QObject * _parent) :
    Pack(_texture_filename, _parent),
    m_options {},
    m_is_valid(false)
{
}

qsizetype GridPack::frameCount() const
{
    return m_is_valid ? m_options.column_count * m_options.row_count : 0;
}

void GridPack::reconfigure(const GridOptions & _options)
{
    m_options = _options;
    recalculate();
}

void GridPack::recalculate()
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

void GridPack::setColumnCount(int _count)
{
    m_options.column_count = _count;
    recalculate();
}

void GridPack::setRowCount(int _count)
{
    m_options.row_count = _count;
    recalculate();
}

void GridPack::setSpriteWidth(int _width)
{
    m_options.sprite_width = _width;
    recalculate();
}

void GridPack::setSpriteHeight(int _height)
{
    m_options.sprite_height = _height;
    recalculate();
}

void GridPack::setMarginTop(int _margin)
{
    m_options.margin_top = _margin;
    recalculate();
}

void GridPack::setMarginLeft(int _margin)
{
    m_options.margin_left = _margin;
    recalculate();
}

void GridPack::setHorizontalSpacing(int _spacing)
{
    m_options.horizontal_spacing = _spacing;
    recalculate();
}

void GridPack::setVerticalSpacing(int _spacing)
{
    m_options.vertical_spacing = _spacing;
    recalculate();
}

bool GridPack::forEachFrame(std::function<void (const Frame &)> _cb) const
{
    if(!m_is_valid)
    {
        return false;
    }
    const QString frame_name_format = QString("%1_%2").arg(QFileInfo(textureFilename()).baseName());
    Frame frame
    {
        .texture_rect = QRect(0, 0, m_options.sprite_width, m_options.sprite_height),
        .sprite_rect = QRect(0, 0, m_options.sprite_width, m_options.sprite_height),
        .name = QString(),
        .is_rotated = false
    };
    int index = 0;
    for(qint32 row = 0; row < m_options.row_count; ++row)
    {
        const int y = m_options.margin_top + row * m_options.sprite_height + row * m_options.vertical_spacing;
        for(qint32 col = 0; col < m_options.column_count; ++col)
        {
            const int x = m_options.margin_left + col * m_options.sprite_width + col * m_options.horizontal_spacing;
            frame.texture_rect.moveTo(x, y);
            frame.name = frame_name_format.arg(++index, 4, 10, QChar('0'));
            _cb(frame);
        }
    }
    return true;
}
