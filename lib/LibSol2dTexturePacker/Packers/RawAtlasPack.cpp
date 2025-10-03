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

#include <LibSol2dTexturePacker/Packers/RawAtlasPack.h>
#include <LibSol2dTexturePacker/Atlas/Sol2dAtlasSerializer.h>
#include <LibSol2dTexturePacker/Exception.h>

void RawAtlasPack::save(
    const QDir & _directory,
    const QString & _atlas_name,
    const QString & _image_format)
{
    if(m_atlases.empty())
        return;

    Sol2dAtlasSerializer serializer;

    size_t index = m_atlases.size() == 1 ? 0 : 1;
    for(const RawAtlas & ra : m_atlases)
    {
        const QString base_filename = _directory.absoluteFilePath(index == 0
            ? _atlas_name
            : QString("%1-%2").arg(_atlas_name).arg(index));
        QString filename = QString("%1.%2").arg(base_filename, _image_format);
        ++index;
        if(!ra.image.save(filename))
            throw ImageSavingException(filename);
        serializer.serialize(
            {
                .texture = filename,
                .frames = ra.frames
            },
            _directory.absoluteFilePath(QString("%1.%2").arg(base_filename, serializer.defaultFileExtenstion())));
    }
}
