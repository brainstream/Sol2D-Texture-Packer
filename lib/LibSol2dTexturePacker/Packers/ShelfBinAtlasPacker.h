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

#include <LibSol2dTexturePacker/Packers/OnlineAlgorithmAtlasPacker.h>

enum class S2TP_EXPORT ShelfBinAtlasPackerChoiceHeuristic
{
    NextFit,
    FirstFit,
    BestAreaFit,
    WorstAreaFit,
    BestHeightFit,
    BestWidthFit,
    WorstWidthFit
};

class S2TP_EXPORT ShelfBinAtlasPacker : public OnlineAlgorithmAtlasPacker
{
public:
    explicit ShelfBinAtlasPacker(QObject * _parent = nullptr);
    void setChoiceHeuristic(ShelfBinAtlasPackerChoiceHeuristic _heuristic) { m_heuristic = _heuristic; }
    ShelfBinAtlasPackerChoiceHeuristic choiceHeuristic() const { return m_heuristic; }
    void enableWasteMap(bool _enable) { m_use_waste_map = _enable; }
    bool isWasteMapEnabled() const { return m_use_waste_map; }

protected:
    std::unique_ptr<AtlasPackerOnlineAlgorithm> createAlgorithm(const QSize & _max_atlas_size) const override;

private:
    ShelfBinAtlasPackerChoiceHeuristic m_heuristic;
    bool m_use_waste_map;
};
