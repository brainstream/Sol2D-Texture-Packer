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

#include <LibSol2dTexturePacker/Pack/Pack.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QPainter>

void Pack::unpack(const QDir & _output_dir) const
{
    const QFileInfo fi(textureFilename());
    const QString format = _output_dir.filePath(QString("%1_%2.png").arg(fi.baseName()));
    quint32 index = 0;
    QImage texture = this->texture();
    forEachFrame([&](const Frame & _frame) {
        QImage img(
            static_cast<int>(_frame.width),
            static_cast<int>(_frame.height),
            QImage::Format_ARGB32);
        img.fill(0);
        QPainter painter(&img);
        painter.drawImage(
            0,
            0,
            texture,
            _frame.x,
            _frame.y,
            static_cast<int>(_frame.width),
            static_cast<int>(_frame.height));
        const QString filename = format.arg(++index, 4, 10, QChar('0'));
        if(!img.save(filename))
            throw FileOpenException(filename, FileOpenException::Write);
    });
}

const QImage & Pack::texture() const
{
    if(m_texture.isNull())
    {
        if(!m_texture.load(m_texture_filename))
            throw ImageLoadingException(m_texture_filename);
    }
    return m_texture;
}
