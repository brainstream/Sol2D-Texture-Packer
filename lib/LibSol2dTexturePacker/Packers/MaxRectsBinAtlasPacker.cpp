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

namespace {

class MaxRectsBinPackAlgorithm : public AtlasPackerAlgorithm
{
public:
    MaxRectsBinPackAlgorithm(
        const QSize & _max_atlas_size,
        MaxRectsBinAtlasPackerChoiceHeuristic _heuristic,
        bool _allow_flip);
    QRect insert(int _width, int _height) override;
    void resetBin() override;

private:
    static rbp::MaxRectsBinPack::FreeRectChoiceHeuristic map(MaxRectsBinAtlasPackerChoiceHeuristic _heuristic);

private:
    QSize m_max_atlas_size;
    rbp::MaxRectsBinPack::FreeRectChoiceHeuristic m_heuristic;
    rbp::MaxRectsBinPack m_pack;
    bool m_allow_flip;
};

MaxRectsBinPackAlgorithm::MaxRectsBinPackAlgorithm(
    const QSize & _max_atlas_size,
    MaxRectsBinAtlasPackerChoiceHeuristic _heuristic,
    bool _allow_flip
) :
    m_max_atlas_size(_max_atlas_size),
    m_heuristic(map(_heuristic)),
    m_pack(m_max_atlas_size.width(), m_max_atlas_size.height(), _allow_flip),
    m_allow_flip(_allow_flip)
{
}

QRect MaxRectsBinPackAlgorithm::insert(int _width, int _height)
{
    rbp::Rect rect = m_pack.Insert(_width, _height, m_heuristic);
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

void MaxRectsBinPackAlgorithm::resetBin()
{
    m_pack.Init(m_max_atlas_size.width(), m_max_atlas_size.height(), m_allow_flip);
}

} // namespace

rbp::MaxRectsBinPack::FreeRectChoiceHeuristic MaxRectsBinPackAlgorithm::map(
    MaxRectsBinAtlasPackerChoiceHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case MaxRectsBinAtlasPackerChoiceHeuristic::BestLongSideFit:
        return rbp::MaxRectsBinPack::RectBestLongSideFit;
    case MaxRectsBinAtlasPackerChoiceHeuristic::BestShortSideFit:
        return rbp::MaxRectsBinPack::RectBestShortSideFit;
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
    m_heuristic(MaxRectsBinAtlasPackerChoiceHeuristic::BestLongSideFit),
    m_allow_flip(true)
{
}

std::unique_ptr<AtlasPackerAlgorithm> MaxRectsBinAtlasPacker::createAlgorithm(const QSize & _max_atlas_size) const
{
    return std::unique_ptr<AtlasPackerAlgorithm>(
        new MaxRectsBinPackAlgorithm(_max_atlas_size, m_heuristic, m_allow_flip));
}
