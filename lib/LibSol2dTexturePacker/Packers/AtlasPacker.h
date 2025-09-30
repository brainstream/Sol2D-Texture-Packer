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

#include <LibSol2dTexturePacker/Sprite.h>
#include <QObject>
#include <QList>

class S2TP_EXPORT AtlasPacker : public QObject
{
protected:
    class Algorithm
    {
    public:
        virtual ~Algorithm() { }
        virtual QRect insert(int _width, int _height) = 0;
        virtual void init(int _width, int _height) = 0;
    };

public:
    explicit AtlasPacker(QObject * _parent) :
        QObject(_parent),
        m_max_atlas_size{1024, 1024}
    {
    }

    void setMaxAtlasSize(int _w, int _h) { m_max_atlas_size = { _w, _h }; }
    void setMaxAtlasSize(const QSize & _size) { m_max_atlas_size = _size; }
    const QSize & maxAtlasSize() const { return m_max_atlas_size; }
    QList<QPixmap> pack(const QList<Sprite> & _sprites) const;

protected:
    virtual std::unique_ptr<Algorithm> createAlgorithm(int _width, int _height) const = 0;

protected:
    QSize m_max_atlas_size;
};
