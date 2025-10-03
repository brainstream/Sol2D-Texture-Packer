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
#include <LibSol2dTexturePacker/Pack/AtlasPack.h>
#include <LibSol2dTexturePacker/Atlas/Sol2dAtlasSerializer.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QImage>

class UnpackApplication::Runner
{
public:
    Runner(const QString & _out_directory) :
        m_out_directory(_out_directory)
    {
    }

    virtual ~Runner() = default;
    virtual void run() = 0;

protected:
    const QString m_out_directory;
};

class UnpackApplication::GridRunner : public UnpackApplication::Runner
{
public:
    GridRunner(const QString & _texture, const GridOptions & _grid, const QString & _out_directory) :
        UnpackApplication::Runner(_out_directory),
        m_texture(_texture),
        m_grid(_grid)
    {
    }

    void run() override;

private:
    const QString m_texture;
    const GridOptions m_grid;
};

void UnpackApplication::GridRunner::run()
{
    GridPack pack(m_texture);
    pack.reconfigure(m_grid);
    pack.unpack(m_out_directory);
}

class UnpackApplication::AtlasRunner : public UnpackApplication::Runner
{
public:
    AtlasRunner(const QString & _atlas, const QString & _out_directory) :
        UnpackApplication::Runner(_out_directory),
        m_atlas(_atlas)
    {
    }

    void run() override;

private:
    const QString m_atlas;
};

void UnpackApplication::AtlasRunner::run()
{
    Atlas atlas;
    {
        Sol2dAtlasSerializer serializer;
        serializer.deserialize(m_atlas, atlas);
    }
    AtlasPack pack(atlas);
    pack.unpack(m_out_directory);
}

UnpackApplication::UnpackApplication(
    const QString & _texture,
    const GridOptions & _grid,
    const QString & _out_directory
) :
    m_runner(new GridRunner(_texture, _grid, _out_directory))
{
}

UnpackApplication::UnpackApplication(const QString & _atlas, const QString & _out_directory) :
    m_runner(new AtlasRunner(_atlas, _out_directory))
{
}

UnpackApplication::~UnpackApplication()
{
    delete m_runner;
}

int UnpackApplication::exec()
{
    m_runner->run();
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
