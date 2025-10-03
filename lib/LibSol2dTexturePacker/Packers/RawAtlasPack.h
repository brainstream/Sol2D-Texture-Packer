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

#include <LibSol2dTexturePacker/Frame.h>
#include <QImage>
#include <QDir>
#include <list>

struct S2TP_EXPORT RawAtlas
{
    QImage image;
    QList<Frame> frames;
};

class S2TP_EXPORT RawAtlasPack final
{
    Q_DISABLE_COPY_MOVE(RawAtlasPack)

public:
    typedef RawAtlas value_type;
    typedef typename std::list<RawAtlas>::const_iterator iterator;

public:
    RawAtlasPack() { }

    void save(
        const QDir & _directory,
        const QString & _atlas_name,
        const QString & _image_format,
        bool _remove_file_ext);

    void add(RawAtlas && _atlas){ m_atlases.emplace_back(std::move(_atlas)); }
    void add(const RawAtlas & _atlas) { m_atlases.push_back(_atlas); }
    size_t count() const { return m_atlases.size(); }
    std::list<RawAtlas>::const_iterator begin() const { return m_atlases.cbegin(); }
    std::list<RawAtlas>::const_iterator end() const { return m_atlases.end(); }

private:
    std::list<RawAtlas> m_atlases;
};
