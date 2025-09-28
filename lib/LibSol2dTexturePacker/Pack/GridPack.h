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

#include <LibSol2dTexturePacker/Pack/Pack.h>
#include <QObject>

struct S2TP_EXPORT GridOptions
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

class S2TP_EXPORT GridPack : public Pack
{
    Q_OBJECT

public:
    explicit GridPack(const QString & _texture_filename, QObject * _parent = nullptr);
    qsizetype frameCount() const override;
    void reconfigure(const GridOptions & _options);
    bool isValid() const { return m_is_valid; }
    bool forEachFrame(std::function<void (const Frame &)> _cb) const override;
    const GridOptions options() const { return m_options; }

public slots:
    void setColumnCount(int _count);
    void setRowCount(int _count);
    void setSpriteWidth(int _width);
    void setSpriteHeight(int _height);
    void setMarginTop(int _margin);
    void setMarginLeft(int _margin);
    void setHorizontalSpacing(int _spacing);
    void setVerticalSpacing(int _spacing);

signals:
    void framesChanged();

private:
    void recalculate();

private:
    GridOptions m_options;
    bool m_is_valid;
};
