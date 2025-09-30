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

#include <LibSol2dTexturePacker/Packers/SkylineBinAtlasPacker.h>
#include <RectangleBinPack/SkylineBinPack.h>
#include <QPainter>

namespace {

class SkylineBinPackAlgorithm : public AtlasPackerAlgorithm
{
public:
    SkylineBinPackAlgorithm(
        const QSize & _max_atlas_size,
        SkylineBinAtlasPackerLevelChoiceHeuristic _heuristic,
        bool _use_waste_map);
    QRect insert(int _width, int _height) override;
    void resetBin() override;

private:
    static rbp::SkylineBinPack::LevelChoiceHeuristic map(SkylineBinAtlasPackerLevelChoiceHeuristic _heuristic);

private:
    QSize m_max_atlas_size;
    rbp::SkylineBinPack::LevelChoiceHeuristic m_heuristic;
    rbp::SkylineBinPack m_pack;
    bool m_use_waste_map;
};

SkylineBinPackAlgorithm::SkylineBinPackAlgorithm(
    const QSize & _max_atlas_size,
    SkylineBinAtlasPackerLevelChoiceHeuristic _heuristic,
    bool _use_waste_map
) :
    m_max_atlas_size(_max_atlas_size),
    m_heuristic(map(_heuristic)),
    m_pack(_max_atlas_size.width(), _max_atlas_size.height(), _use_waste_map),
    m_use_waste_map(_use_waste_map)
{
}

rbp::SkylineBinPack::LevelChoiceHeuristic SkylineBinPackAlgorithm::map(
    SkylineBinAtlasPackerLevelChoiceHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case SkylineBinAtlasPackerLevelChoiceHeuristic::BottomLeft:
        return rbp::SkylineBinPack::LevelBottomLeft;
    case SkylineBinAtlasPackerLevelChoiceHeuristic::MinWasteFit:
        return rbp::SkylineBinPack::LevelMinWasteFit;
    default:
        throw std::runtime_error("Unknown SkylineBinAtlasPackerLevelChoiceHeuristic");
    }
}

} // namespace

QRect SkylineBinPackAlgorithm::insert(int _width, int _height)
{
    rbp::Rect rect = m_pack.Insert(_width, _height, m_heuristic);
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

void SkylineBinPackAlgorithm::resetBin()
{
    m_pack.Init(m_max_atlas_size.width(), m_max_atlas_size.height(), m_use_waste_map);
}

SkylineBinAtlasPacker::SkylineBinAtlasPacker(QObject *_parent) :
    AtlasPacker(_parent),
    m_heuristic(SkylineBinAtlasPackerLevelChoiceHeuristic::BottomLeft),
    m_use_waste_map(false)
{
}

std::unique_ptr<AtlasPackerAlgorithm> SkylineBinAtlasPacker::createAlgorithm(const QSize & _max_atlas_size) const
{
    return std::unique_ptr<AtlasPackerAlgorithm>(
        new SkylineBinPackAlgorithm(_max_atlas_size, m_heuristic, m_use_waste_map));
}
