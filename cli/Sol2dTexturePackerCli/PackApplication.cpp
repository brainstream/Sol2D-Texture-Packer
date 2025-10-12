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

#include <Sol2dTexturePackerCli/PackApplication.h>

PackApplication::PackApplication(
    QList<Sprite> && _sprites,
    std::unique_ptr<AtlasPacker> && _packer,
    const AtlasPackerOptions & _options,
    const QDir & _output_directory,
    const QString & _atlas_name,
    const QString & _texture_format
) :
    m_sprites(std::move(_sprites)),
    m_packer(std::move(_packer)),
    m_options(_options),
    m_output_directory(_output_directory),
    m_atlas_name(_atlas_name),
    m_texture_format(_texture_format)
{
}

int PackApplication::exec()
{
    QPromise<void> promise;
    std::unique_ptr<RawAtlasPack> pack = m_packer->pack(promise, m_sprites, m_options);
    pack->save(m_output_directory, m_atlas_name, m_texture_format);
    return 0;
}
