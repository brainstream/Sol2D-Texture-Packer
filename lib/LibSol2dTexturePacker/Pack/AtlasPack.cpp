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

#include <LibSol2dTexturePacker/Pack/AtlasPack.h>

AtlasPack::AtlasPack(const Atlas & _atlas, QObject * _parent) :
    Pack(_atlas.texture, _parent),
    m_frames(_atlas.frames)
{
}

qsizetype AtlasPack::frameCount() const
{
    return m_frames.count();
}

bool AtlasPack::forEachFrame(std::function<void (const Frame &)> _cb) const
{
    foreach(const Frame & frame, m_frames)
        _cb(frame);
    return !m_frames.empty();
}
