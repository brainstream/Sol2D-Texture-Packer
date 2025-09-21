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
#include <LibSol2dTexturePacker/Pack/GridPack.h>

class UnpackApplication : public Application
{
    Q_DISABLE_COPY_MOVE(UnpackApplication)

private:
    class Runner;
    class GridRunner;
    class AtlasRunner;

public:
    UnpackApplication(const QString & _texture, const GridOptions & _grid, const QString & _out_directory);
    UnpackApplication(const QString & _atlas, const QString & _out_directory);
    ~UnpackApplication() override;
    int exec() override;

private:
    static QImage loadImage(const QString & _path);

private:
    Runner * m_runner;
};
