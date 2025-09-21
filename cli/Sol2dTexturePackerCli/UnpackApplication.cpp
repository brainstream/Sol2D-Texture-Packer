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

#include <Sol2dTexturePackerCli/UnpackApplication.h>
#include <LibSol2dTexturePacker/Splitters/GridSplitter.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QImage>
#include <utility>

UnpackApplication::UnpackApplication(const GridOptions & _grid, QString _out_directory) :
    m_input(_grid),
    m_out_directory(std::move(_out_directory))
{
}

UnpackApplication::UnpackApplication(const QString & _atlas, QString _out_directory) :
    m_input(_atlas),
    m_out_directory(std::move(_out_directory))
{
}

int UnpackApplication::exec()
{
    struct Visitor
    {
        const UnpackApplication * app;

        void operator ()(const GridOptions & _grid) const
        {
            GridSplitter splitter(nullptr);
            splitter.reconfigure(_grid.splitter_options);
            splitter.apply(
                {
                    .path = _grid.texture.toStdString(),
                    .image = loadImage(_grid.texture)
                },
                QDir(app->m_out_directory)
            );
        }

        void operator ()(const QString & _atlas) const
        {
            // TODO: unpack atlas
        }
    };
    Visitor visitor { .app = this };
    std::visit(visitor, m_input);
    return 0;
}

QImage UnpackApplication::loadImage(const QString & _path)
{
    QImage image;
    if(!image.load(_path))
    {
        throw ImageLoadingException(_path);
    }
    return image;
}
