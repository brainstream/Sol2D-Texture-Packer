/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Open Sprite Sheet Tools.                                                      *
 *                                                                                                        *
 * Open Sprite Sheet Tools is free software: you can redistribute it and/or modify it under  the terms of *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Open Sprite Sheet Tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
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
    BestShortSideFit,
    BestLongSideFit,
    BestAreaFit,
    BottomLeftRule,
    ContactPointRule
};

struct S2TP_EXPORT MaxRectsBinAtlasPackerOptions
{
    QSize max_atlas_size;
    MaxRectsBinAtlasPackerChoiceHeuristic heuristic;
    bool allow_flip;
};

class S2TP_EXPORT FreeRectAtlasPacker : public AtlasPacker
{
    Q_OBJECT

public:
    explicit FreeRectAtlasPacker(const MaxRectsBinAtlasPackerOptions & _options, QObject * _parent);
    QList<QPixmap> pack(const QList<Sprite> & _sprites) const override;

private:
    const MaxRectsBinAtlasPackerOptions m_options;
};
