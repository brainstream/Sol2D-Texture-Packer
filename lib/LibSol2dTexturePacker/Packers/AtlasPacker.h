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

#include <LibSol2dTexturePacker/Packers/RawAtlasPack.h>
#include <LibSol2dTexturePacker/Sprite.h>
#include <QObject>

struct S2TP_EXPORT AtlasPackerOptions
{
    QSize max_atlas_size = QSize(2048, 2048);
    bool detect_duplicates = false;
    bool crop = false;
    bool remove_file_extensions = true;
};

class S2TP_EXPORT AtlasPacker : public QObject
{
public:
    explicit AtlasPacker(QObject * _parent) :
        QObject(_parent),
        m_max_atlas_size{1024, 1024}
    {
    }

    virtual std::unique_ptr<RawAtlasPack> pack(
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const = 0;

protected:
    QSize m_max_atlas_size;
};
