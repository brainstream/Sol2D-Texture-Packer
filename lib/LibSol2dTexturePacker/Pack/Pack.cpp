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

void Pack::unpack(const QDir & _output_dir, const QString & _format) const
{
    forEachFrame([&](const Frame & __frame) {
        Sprite sprite = unpackFrame(__frame, _output_dir, _format);
        if(!sprite.image.save(sprite.path))
            throw FileOpenException(sprite.path, FileOpenException::Write);
    });
}

Sprite Pack::unpackFrame(const Frame & _frame, const QDir & _output_dir, const QString & _format) const
{
    QImage img(_frame.sprite_rect.width(), _frame.sprite_rect.height(), QImage::Format_ARGB32);
    img.fill(0);
    QPainter painter(&img);
    if(_frame.is_rotated)
    {
        QTransform rotation;
        rotation.rotate(90);
        painter.setTransform(rotation);
    }
    painter.drawImage(
        _frame.sprite_rect.topLeft(),
        this->texture(),
        _frame.texture_rect);
    const QString filename = makeUnpackFilename(_output_dir, _format, _frame);
    return Sprite { .path = filename, .name = _frame.name, .image = img };
}

QString Pack::makeUnpackFilename(const QDir & _output_dir, const QString & _format, const Frame & _frame) const
{
    const QString base_name = _frame.name.isEmpty() ? "sprite" : QFileInfo(_frame.name).baseName();
    {
        const QFileInfo fi(_output_dir.filePath(base_name + "." + _format));
        if(!fi.exists())
            return fi.absoluteFilePath();
    }
    const QString name_format = QString("%1 (%3).%2").arg(base_name, _format);
    for(int i = 1; ; ++i)
    {
        const QFileInfo fi(_output_dir.filePath(name_format.arg(i)));
        if(!fi.exists())
            return fi.absoluteFilePath();
    }
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
