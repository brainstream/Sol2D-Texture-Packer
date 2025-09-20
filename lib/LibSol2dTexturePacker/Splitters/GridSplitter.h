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

#pragma once

#include <LibSol2dTexturePacker/Splitters/Splitter.h>

struct S2TP_EXPORT GridSplitterOptions
{
    int column_count;
    int row_count;
    int sprite_width;
    int sprite_height;
    int margin_top;
    int margin_left;
    int horizontal_spacing;
    int vertical_spacing;
};

class S2TP_EXPORT GridSplitter : public Splitter
{
    Q_OBJECT

public:
    explicit GridSplitter(QObject * _parent);
    void reconfigure(const GridSplitterOptions & _options);
    bool forEachFrame(std::function<void(const Frame &)> _cb) const override;
    qsizetype frameCount() const override;

public slots:
    void setColumnCount(int _count);
    void setRowCount(int _count);
    void setSpriteWidth(int _width);
    void setSpriteHeight(int _height);
    void setMarginTop(int _margin);
    void setMarginLeft(int _margin);
    void setHorizontalSpacing(int _spacing);
    void setVerticalSpacing(int _spacing);
    void reset() override;

private:
    void recalculate();

private:
    GridSplitterOptions m_options;
    bool m_is_valid;
};

