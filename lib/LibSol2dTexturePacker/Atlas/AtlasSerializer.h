/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Open Sprite Sheet Tools.                                                      *
 *                                                                                                        *
 * Open Sprite Sheet Tools is free software: you can redistribute it and/or modify it under  the terms of *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Open Sprite Sheet Tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#pragma once

#include <LibSol2dTexturePacker/Atlas/Atlas.h>
#include <QObject>

class S2TP_EXPORT AtlasSerializer
{
    Q_DISABLE_COPY_MOVE(AtlasSerializer)

public:
    AtlasSerializer() { }
    virtual ~AtlasSerializer() { }
    virtual void serialize(const Atlas & _atlas, const std::filesystem::path & _file) = 0;
    virtual void deserialize(const std::filesystem::path & _file, Atlas & _atlas) = 0;
    virtual const char * defaultFileExtenstion() const = 0;

protected:
    static QString makeDefaultFrameName(const Atlas & _atlas, quint32 _index)
    {
        return QString("%1_%2%3")
            .arg(_atlas.texture.stem().string())
            .arg(_index, 4, 10, QChar('0'))
            .arg(_atlas.texture.extension().string());
    }

    static QString makeTextureRelativePath(const Atlas & _atlas, const std::filesystem::path & _data_file_path)
    {
        return QString::fromStdString(std::filesystem::relative(_atlas.texture, _data_file_path.parent_path()));
    }
};
