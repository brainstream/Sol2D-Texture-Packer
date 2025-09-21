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

#include <LibSol2dTexturePacker/Atlas/Atlas.h>

class S2TP_EXPORT AtlasSerializer
{
    Q_DISABLE_COPY_MOVE(AtlasSerializer)

public:
    AtlasSerializer() { }
    virtual ~AtlasSerializer() { }
    virtual void serialize(const Atlas & _atlas, const QString & _file) = 0;
    virtual void deserialize(const QString & _file, Atlas & _atlas) = 0;
    virtual const char * defaultFileExtenstion() const = 0;

protected:
    static QString makeDefaultFrameName(const Atlas & _atlas, quint32 _index);
    static QString makeTextureRelativePath(const Atlas & _atlas, const QString & _data_file_path);
};
