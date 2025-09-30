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

enum class S2TP_EXPORT GuillotineBinAtlasPackerChoiceHeuristic
{
    BestAreaFit,
    BestShortSideFit,
    BestLongSideFit,
    WorstAreaFit,
    WorstShortSideFit,
    WorstLongSideFit
};

enum class S2TP_EXPORT GuillotineBinAtlasPackerSplitHeuristic
{
    ShorterLeftoverAxis,
    LongerLeftoverAxis,
    MinimizeArea,
    MaximizeArea,
    ShorterAxis,
    LongerAxis
};

class S2TP_EXPORT GuillotineBinAtlaskPacker : public AtlasPacker
{
public:
    explicit GuillotineBinAtlaskPacker(QObject * _parent = nullptr);

    void setChoiceHeuristic(GuillotineBinAtlasPackerChoiceHeuristic _heuristic) { m_choice_heuristic = _heuristic; }
    GuillotineBinAtlasPackerChoiceHeuristic ChoiceHeuristic() const { return m_choice_heuristic; }
    void setSplitHeuristic(GuillotineBinAtlasPackerSplitHeuristic _heuristic) { m_split_heuristic = _heuristic; }
    GuillotineBinAtlasPackerSplitHeuristic splitHeuristic() const { return m_split_heuristic; }
    void enableMerge(bool _enabled) { m_is_merge_enabled = _enabled; }
    bool isMergeEnabled() const { return m_is_merge_enabled; }

protected:
    std::unique_ptr<AtlasPackerAlgorithm> createAlgorithm(const QSize & _max_atlas_size) const override;

private:
    GuillotineBinAtlasPackerChoiceHeuristic m_choice_heuristic;
    GuillotineBinAtlasPackerSplitHeuristic m_split_heuristic;
    bool m_is_merge_enabled;
};
