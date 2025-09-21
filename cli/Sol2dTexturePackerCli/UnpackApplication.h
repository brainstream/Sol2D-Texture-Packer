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

#include <Sol2dTexturePackerCli/Application.h>
#include <LibSol2dTexturePacker/Splitters/GridSplitter.h>
#include <variant>

class UnpackApplication : public Application
{
public:
    struct GridOptions
    {
        QString texture;
        GridSplitterOptions splitter_options;
    };

public:
    UnpackApplication(const GridOptions & _grid, QString  _out_directory);
    UnpackApplication(const QString & _atlas, QString  _out_directory);
    int exec() override;

private:
    static QImage loadImage(const QString & _path);

private:
    const std::variant<GridOptions, QString> m_input;
    const QString m_out_directory;
};
