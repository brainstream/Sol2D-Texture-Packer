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
#include <QObject>

class S2TP_EXPORT Pack : public QObject
{
    Q_OBJECT

public:
    explicit Pack(const QString & _texture_filename, QObject * _parent = nullptr) :
        QObject(_parent),
        m_texture_filename(_texture_filename)
    {
    }

    virtual ~Pack() = default;
    void unpack(const QDir & _output_dir) const;
    const QImage & texture() const;
    const QString & textureFilename() const { return m_texture_filename; }
    virtual qsizetype frameCount() const = 0;
    virtual bool forEachFrame(std::function<void(const Frame &)> _cb) const = 0;

private:
    QString makeUnpackFilename(const QDir & _output_dir, const Frame & _frame) const;

private:
    const QString m_texture_filename;
    mutable QImage m_texture;
};
