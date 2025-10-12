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

#include <LibSol2dTexturePacker/Packers/AtlasPacker.h>

class S2TP_EXPORT MetaAtlasPacker final : public AtlasPacker
{
public:
    explicit MetaAtlasPacker(QObject * _parent = nullptr);

    std::unique_ptr<RawAtlasPack> pack(
        QPromise<void> & _promise,
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const override;

private:
    std::unique_ptr<RawAtlasPack> packBestMaxRectsBinAtlasPacker(
        QPromise<void> & _promise,
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const;
    std::unique_ptr<RawAtlasPack> packBestSkylineBinAtlasPacker(
        QPromise<void> & _promise,
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const;
    std::unique_ptr<RawAtlasPack> packBestGuillotineBinAtlaskPacker(
        QPromise<void> & _promise,
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const;
    std::unique_ptr<RawAtlasPack> packBestShelfBinAtlasPacker(
        QPromise<void> & _promise,
        const QList<Sprite> & _sprites,
        const AtlasPackerOptions & _options) const;
};
