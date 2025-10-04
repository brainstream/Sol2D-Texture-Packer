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

enum S2TP_EXPORT class MaxRectsBinAtlasPackerChoiceHeuristic
{
    BestLongSideFit,
    BestShortSideFit,
    BestAreaFit,
    BottomLeftRule,
    ContactPointRule
};

class S2TP_EXPORT MaxRectsBinAtlasPacker final : public AtlasPacker
{
public:
    explicit MaxRectsBinAtlasPacker(QObject * _parent = nullptr);
    void allowFlip(bool _allow) { m_allow_flip = _allow; }
    bool isFlipAllowed() const { return m_allow_flip; }
    void setChoiceHeuristic(MaxRectsBinAtlasPackerChoiceHeuristic _heuristic) { m_heuristic = _heuristic; }
    MaxRectsBinAtlasPackerChoiceHeuristic choiceHeuristic() const { return m_heuristic; }

protected:
    std::unique_ptr<AtlasPackerAlgorithm> createAlgorithm(const QSize & _max_atlas_size) const override;

private:
    MaxRectsBinAtlasPackerChoiceHeuristic m_heuristic;
    bool m_allow_flip;
};
