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

#include <LibSol2dTexturePacker/Packers/AtlasPacker.h>
#include <Sol2dTexturePackerCli/Application.h>

class PackApplication : public Application
{
    Q_DISABLE_COPY_MOVE(PackApplication)

public:
    PackApplication(
        QList<Sprite> && _sprites,
        std::unique_ptr<AtlasPacker> && _packer,
        const AtlasPackerOptions & _options,
        const QDir & _output_directory,
        const QString & _atlas_name,
        const QString & _texture_format);
    int exec() override;

private:
    const QList<Sprite> m_sprites;
    const std::unique_ptr<AtlasPacker> m_packer;
    const AtlasPackerOptions m_options;
    const QDir m_output_directory;
    const QString m_atlas_name;
    const QString m_texture_format;
};
