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

#include <LibSol2dTexturePacker/Packers/ShelfBinAtlasPacker.h>
#include <RectangleBinPack/ShelfBinPack.h>

namespace {

class ShelfBinAtlasPackerAlgorithm : public AtlasPackerAlgorithm
{
public:
    ShelfBinAtlasPackerAlgorithm(
        int _max_bin_width,
        int _max_bin_height,
        ShelfBinAtlasPackerChoiceHeuristic _heuristic,
        bool _use_waste_map);
    QRect insert(int _width, int _height) override;
    void resetBin() override;

private:
    static rbp::ShelfBinPack::ShelfChoiceHeuristic map(ShelfBinAtlasPackerChoiceHeuristic _heuristic);

private:
    int m_max_bin_width;
    int m_max_bin_height;
    rbp::ShelfBinPack m_pack;
    rbp::ShelfBinPack::ShelfChoiceHeuristic m_heuristic;
    bool m_use_waste_map;
};

ShelfBinAtlasPackerAlgorithm::ShelfBinAtlasPackerAlgorithm(
    int _max_bin_width,
    int _max_bin_height,
    ShelfBinAtlasPackerChoiceHeuristic _heuristic,
    bool _use_waste_map
) :
    m_max_bin_width(_max_bin_width),
    m_max_bin_height(_max_bin_height),
    m_pack(_max_bin_width, _max_bin_height, _use_waste_map),
    m_heuristic(map(_heuristic)),
    m_use_waste_map(_use_waste_map)
{
}

rbp::ShelfBinPack::ShelfChoiceHeuristic ShelfBinAtlasPackerAlgorithm::map(ShelfBinAtlasPackerChoiceHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case ShelfBinAtlasPackerChoiceHeuristic::NextFit:
        return rbp::ShelfBinPack::ShelfNextFit;
    case ShelfBinAtlasPackerChoiceHeuristic::FirstFit:
        return rbp::ShelfBinPack::ShelfFirstFit;
    case ShelfBinAtlasPackerChoiceHeuristic::BestAreaFit:
        return rbp::ShelfBinPack::ShelfBestAreaFit;
    case ShelfBinAtlasPackerChoiceHeuristic::WorstAreaFit:
        return rbp::ShelfBinPack::ShelfWorstAreaFit;
    case ShelfBinAtlasPackerChoiceHeuristic::BestHeightFit:
        return rbp::ShelfBinPack::ShelfBestHeightFit;
    case ShelfBinAtlasPackerChoiceHeuristic::BestWidthFit:
        return rbp::ShelfBinPack::ShelfBestWidthFit;
    case ShelfBinAtlasPackerChoiceHeuristic::WorstWidthFit:
        return rbp::ShelfBinPack::ShelfWorstWidthFit;
    default:
        throw std::runtime_error("Unknown ShelfBinAtlasPackerChoiceHeuristic");
    }
}

QRect ShelfBinAtlasPackerAlgorithm::insert(int _width, int _height)
{
    rbp::Rect rect = m_pack.Insert(_width, _height, m_heuristic);
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

void ShelfBinAtlasPackerAlgorithm::resetBin()
{
    m_pack.Init(m_max_bin_width, m_max_bin_height, m_use_waste_map);
}

} // namespace

ShelfBinAtlasPacker::ShelfBinAtlasPacker(QObject * _parent) :
    AtlasPacker(_parent),
    m_heuristic(ShelfBinAtlasPackerChoiceHeuristic::NextFit),
    m_use_waste_map(false)
{
}

std::unique_ptr<AtlasPackerAlgorithm> ShelfBinAtlasPacker::createAlgorithm(int _width, int _height) const
{
    return std::unique_ptr<AtlasPackerAlgorithm>(
        new ShelfBinAtlasPackerAlgorithm(_width, _height, m_heuristic, m_use_waste_map));
}
