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

#pragma once

#include <LibSol2dTexturePacker/Splitters/Splitter.h>

class S2TP_EXPORT GridSplitter : public Splitter
{
    Q_OBJECT

public:
    explicit GridSplitter(QObject * _parent);
    bool forEachFrame(std::function<void(const Frame &)> _cb) const override;
    virtual qsizetype frameCount() const override;

public slots:
    void setColumnCount(int _count)
    {
        m_column_count = _count;
        recalculate();
    }

    void setRowCount(int _count)
    {
        m_row_count = _count;
        recalculate();
    }

    void setSpriteWidth(int _width)
    {
        m_sprite_width = _width;
        recalculate();
    }

    void setSpriteHeight(int _height)
    {
        m_sprite_height = _height;
        recalculate();
    }

    void setMarginTop(int _margin)
    {
        m_margin_top = _margin;
        recalculate();
    }

    void setMarginLeft(int _margin)
    {
        m_margin_left = _margin;
        recalculate();
    }

    void setHorizontalSpacing(int _spacing)
    {
        m_horizontal_spacing = _spacing;
        recalculate();
    }

    void setVerticalSpacing(int _spacing)
    {
        m_vertical_spacing = _spacing;
        recalculate();
    }

    void reset() override;

private:
    void recalculate();

private:
    int m_column_count;
    int m_row_count;
    int m_sprite_width;
    int m_sprite_height;
    int m_margin_top;
    int m_margin_left;
    int m_horizontal_spacing;
    int m_vertical_spacing;
    bool m_is_valid;
};

