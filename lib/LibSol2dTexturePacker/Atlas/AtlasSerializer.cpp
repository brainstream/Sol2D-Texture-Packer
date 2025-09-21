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

#include <LibSol2dTexturePacker/Atlas/AtlasSerializer.h>
#include <QFileInfo>
#include <QDir>

QString AtlasSerializer::makeDefaultFrameName(const Atlas & _atlas, quint32 _index)
{
    QFileInfo fi(_atlas.texture);
    return QString("%1_%2.%3")
        .arg(fi.baseName())
        .arg(_index, 4, 10, QChar('0'))
        .arg(fi.suffix());
}

QString AtlasSerializer::makeTextureRelativePath(const Atlas & _atlas, const QString & _data_file_path)
{
    return QFileInfo(_data_file_path).dir().relativeFilePath(_atlas.texture);
}
