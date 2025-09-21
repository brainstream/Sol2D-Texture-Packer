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

#include <LibSol2dTexturePacker/Splitters/Splitter.h>
#include <QPainter>
#include <QImage>
#include <QFileInfo>

void Splitter::apply(const Texture & _texture, const QDir & _out_dir) const
{
    const QFileInfo fi(_texture.path);
    const QString format = _out_dir.filePath(QString("%1_%2.png").arg(fi.baseName()));
    quint32 index = 0;
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
            _texture.image,
            _frame.x,
            _frame.y,
            static_cast<int>(_frame.width),
            static_cast<int>(_frame.height));
        img.save(format.arg(++index, 4, 10, QChar('0')));
    });
}
