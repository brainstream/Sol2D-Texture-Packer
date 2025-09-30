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

enum S2TP_EXPORT class SkylineBinAtlasPackerLevelChoiceHeuristic
{
    BottomLeft,
    MinWasteFit
};


class S2TP_EXPORT SkylineBinAtlasPacker : public AtlasPacker
{
public:
    explicit SkylineBinAtlasPacker(QObject * _parent = nullptr);

    void setLevelChoiceHeuristic(SkylineBinAtlasPackerLevelChoiceHeuristic _heuristic)
    {
        m_heuristic = _heuristic;
    }

    SkylineBinAtlasPackerLevelChoiceHeuristic levelChoiceHeuristic() const
    {
        return m_heuristic;
    }

    void enableWasteMap(bool _enable) { m_use_waste_map = _enable; }
    bool isWasteMapEnabled() const { return m_use_waste_map; }

protected:
    std::unique_ptr<AtlasPackerAlgorithm> createAlgorithm(int _width, int _height) const override;

private:
    SkylineBinAtlasPackerLevelChoiceHeuristic m_heuristic;
    bool m_use_waste_map;
};
