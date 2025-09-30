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

#include <LibSol2dTexturePacker/Packers/GuillotineBinAtlaskPacker.h>
#include <RectangleBinPack/GuillotineBinPack.h>

namespace {

class GuillotineBinPackAlgorithm : public AtlasPackerAlgorithm
{
public:
    GuillotineBinPackAlgorithm(
        int _max_bin_width,
        int _max_bin_height,
        GuillotineBinAtlasPackerChoiceHeuristic _choice_heuristic,
        GuillotineBinAtlasPackerSplitHeuristic _split_heuristic,
        bool _is_merge_enabled);
    QRect insert(int _width, int _height) override;
    void resetBin() override;

private:
    static rbp::GuillotineBinPack::FreeRectChoiceHeuristic map(GuillotineBinAtlasPackerChoiceHeuristic _heuristic);
    static rbp::GuillotineBinPack::GuillotineSplitHeuristic map(GuillotineBinAtlasPackerSplitHeuristic _heuristic);

private:
    int m_max_bin_width;
    int m_max_bin_height;
    rbp::GuillotineBinPack::FreeRectChoiceHeuristic m_choice_heuristic;
    rbp::GuillotineBinPack::GuillotineSplitHeuristic m_split_heuristic;
    rbp::GuillotineBinPack m_pack;
    bool m_is_merge_enabled;
};

GuillotineBinPackAlgorithm::GuillotineBinPackAlgorithm(
    int _max_bin_width,
    int _max_bin_height,
    GuillotineBinAtlasPackerChoiceHeuristic _choice_heuristic,
    GuillotineBinAtlasPackerSplitHeuristic _split_heuristic,
    bool _is_merge_enabled
) :
    m_max_bin_width(_max_bin_width),
    m_max_bin_height(_max_bin_height),
    m_choice_heuristic(map(_choice_heuristic)),
    m_split_heuristic(map(_split_heuristic)),
    m_pack(_max_bin_width, _max_bin_height),
    m_is_merge_enabled(_is_merge_enabled)
{
}

rbp::GuillotineBinPack::FreeRectChoiceHeuristic GuillotineBinPackAlgorithm::map(
    GuillotineBinAtlasPackerChoiceHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case GuillotineBinAtlasPackerChoiceHeuristic::BestAreaFit:
        return rbp::GuillotineBinPack::RectBestAreaFit;
    case GuillotineBinAtlasPackerChoiceHeuristic::BestShortSideFit:
        return rbp::GuillotineBinPack::RectBestShortSideFit;
    case GuillotineBinAtlasPackerChoiceHeuristic::BestLongSideFit:
        return rbp::GuillotineBinPack::RectBestLongSideFit;
    case GuillotineBinAtlasPackerChoiceHeuristic::WorstAreaFit:
        return rbp::GuillotineBinPack::RectWorstAreaFit;
    case GuillotineBinAtlasPackerChoiceHeuristic::WorstShortSideFit:
        return rbp::GuillotineBinPack::RectWorstShortSideFit;
    case GuillotineBinAtlasPackerChoiceHeuristic::WorstLongSideFit:
        return rbp::GuillotineBinPack::RectWorstLongSideFit;
    default:
        throw std::runtime_error("Unknown GuillotineBinAtlasPackerChoiceHeuristic");
    }
}

rbp::GuillotineBinPack::GuillotineSplitHeuristic GuillotineBinPackAlgorithm::map(
    GuillotineBinAtlasPackerSplitHeuristic _heuristic)
{
    switch(_heuristic)
    {
    case GuillotineBinAtlasPackerSplitHeuristic::ShorterLeftoverAxis:
        return rbp::GuillotineBinPack::SplitShorterLeftoverAxis;
    case GuillotineBinAtlasPackerSplitHeuristic::LongerLeftoverAxis:
        return rbp::GuillotineBinPack::SplitLongerLeftoverAxis;
    case GuillotineBinAtlasPackerSplitHeuristic::MinimizeArea:
        return rbp::GuillotineBinPack::SplitMinimizeArea;
    case GuillotineBinAtlasPackerSplitHeuristic::MaximizeArea:
        return rbp::GuillotineBinPack::SplitMaximizeArea;
    case GuillotineBinAtlasPackerSplitHeuristic::ShorterAxis:
        return rbp::GuillotineBinPack::SplitShorterAxis;
    case GuillotineBinAtlasPackerSplitHeuristic::LongerAxis:
        return rbp::GuillotineBinPack::SplitLongerAxis;
    default:
        throw std::runtime_error("Unknown GuillotineBinAtlasPackerSplitHeuristic");
    }
}

QRect GuillotineBinPackAlgorithm::insert(int _width, int _height)
{
    rbp::Rect rect = m_pack.Insert(_width, _height, m_is_merge_enabled, m_choice_heuristic, m_split_heuristic);
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

void GuillotineBinPackAlgorithm::resetBin()
{
    m_pack.Init(m_max_bin_width, m_max_bin_height);
}

} // namespace

GuillotineBinAtlaskPacker::GuillotineBinAtlaskPacker(QObject * _parent) :
    AtlasPacker(_parent),
    m_choice_heuristic(GuillotineBinAtlasPackerChoiceHeuristic::BestAreaFit),
    m_split_heuristic(GuillotineBinAtlasPackerSplitHeuristic::ShorterLeftoverAxis),
    m_is_merge_enabled(false)
{
}

std::unique_ptr<AtlasPackerAlgorithm> GuillotineBinAtlaskPacker::createAlgorithm(int _width, int _height) const
{
    return std::unique_ptr<AtlasPackerAlgorithm>(
        new GuillotineBinPackAlgorithm(_width, _height, m_choice_heuristic, m_split_heuristic, m_is_merge_enabled));
}
