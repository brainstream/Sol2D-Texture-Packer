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

#include <LibSol2dTexturePacker/Packers/MaxRectsBinAtlasPacker.h>
#include <RectangleBinPack/MaxRectsBinPack.h>

class MaxRectsBinAtlasPacker::MaxRectsBinPackAlgorithm : public AtlasPacker::Algorithm
{
public:
    MaxRectsBinPackAlgorithm(
        int _max_bin_width,
        int _max_bin_height,
        MaxRectsBinAtlasPackerChoiceHeuristic _heuristic,
        bool _allow_flip);
    void init(int _width, int _height) override;
    QRect insert(int _width, int _height) override;

private:
    static rbp::MaxRectsBinPack::FreeRectChoiceHeuristic map(MaxRectsBinAtlasPackerChoiceHeuristic _heuristic);

private:
    int m_max_bin_width;
    int m_max_bin_height;
    rbp::MaxRectsBinPack::FreeRectChoiceHeuristic m_heuristic;
    rbp::MaxRectsBinPack m_pack;
    bool m_allow_flip;
};

MaxRectsBinAtlasPacker::MaxRectsBinPackAlgorithm::MaxRectsBinPackAlgorithm(
    int _max_bin_width,
    int _max_bin_height,
    MaxRectsBinAtlasPackerChoiceHeuristic _heuristic,
    bool _allow_flip
) :
    m_heuristic(map(_heuristic)),
    m_pack(_max_bin_width, _max_bin_height, _allow_flip),
    m_allow_flip(_allow_flip)
{
}

void MaxRectsBinAtlasPacker::MaxRectsBinPackAlgorithm::init(int _width, int _height)
{
    m_pack.Init(_width, _height, m_allow_flip);
}

QRect MaxRectsBinAtlasPacker::MaxRectsBinPackAlgorithm::insert(int _width, int _height)
{
    rbp::Rect rect = m_pack.Insert(_width, _height, m_heuristic);
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

rbp::MaxRectsBinPack::FreeRectChoiceHeuristic MaxRectsBinAtlasPacker::MaxRectsBinPackAlgorithm::map(
    MaxRectsBinAtlasPackerChoiceHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case MaxRectsBinAtlasPackerChoiceHeuristic::BestShortSideFit:
        return rbp::MaxRectsBinPack::RectBestShortSideFit;
    case MaxRectsBinAtlasPackerChoiceHeuristic::BestLongSideFit:
        return rbp::MaxRectsBinPack::RectBestLongSideFit;
    case MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit:
        return rbp::MaxRectsBinPack::RectBestAreaFit;
    case MaxRectsBinAtlasPackerChoiceHeuristic::BottomLeftRule:
        return rbp::MaxRectsBinPack::RectBottomLeftRule;
    case MaxRectsBinAtlasPackerChoiceHeuristic::ContactPointRule:
        return rbp::MaxRectsBinPack::RectContactPointRule;
    default:
        throw std::runtime_error("Unknown MaxRectsBinAtlasPackerChoiceHeuristic");
    }
}

MaxRectsBinAtlasPacker::MaxRectsBinAtlasPacker(QObject * _parent) :
    AtlasPacker(_parent),
    m_heuristic(MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit),
    m_allow_flip(false)
{
}

std::unique_ptr<AtlasPacker::Algorithm> MaxRectsBinAtlasPacker::createAlgorithm(int _width, int _height) const
{
    return std::unique_ptr<Algorithm>(new MaxRectsBinPackAlgorithm(_width, _height, m_heuristic, m_allow_flip));
}
