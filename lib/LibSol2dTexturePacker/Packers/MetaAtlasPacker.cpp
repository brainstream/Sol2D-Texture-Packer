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

#include <LibSol2dTexturePacker/Packers/MetaAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/MaxRectsBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/SkylineBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/GuillotineBinAtlaskPacker.h>
#include <LibSol2dTexturePacker/Packers/ShelfBinAtlasPacker.h>

namespace {

int calculateAtlasPackArea(const RawAtlasPack & _pack)
{
    int area = 0;
    for(const RawAtlas & atlas : _pack)
        area += atlas.image.width() * atlas.image.height();
    return area;
}

bool operator < (const RawAtlasPack & _left, const RawAtlasPack & _rigth)
{
    return calculateAtlasPackArea(_left) < calculateAtlasPackArea(_rigth);
}

} // namespace

MetaAtlasPacker::MetaAtlasPacker(QObject * _parent) :
    AtlasPacker(_parent)
{
}

std::unique_ptr<RawAtlasPack> MetaAtlasPacker::pack(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::unique_ptr<RawAtlasPack> result = packBestMaxRectsBinAtlasPacker(_sprites, _options);
    std::unique_ptr<RawAtlasPack> temp = packBestSkylineBinAtlasPacker(_sprites, _options);
    if(*temp < *result) result.reset(temp.release());
    temp = packBestGuillotineBinAtlaskPacker(_sprites, _options);
    if(*temp < *result) result.reset(temp.release());
    temp = packBestShelfBinAtlasPacker(_sprites, _options);
    if(*temp < *result) result.reset(temp.release());
    return result;
}

std::unique_ptr<RawAtlasPack> MetaAtlasPacker::packBestMaxRectsBinAtlasPacker(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::unique_ptr<RawAtlasPack> result;
    MaxRectsBinAtlasPacker packer;
    for(auto heuristic : {
        MaxRectsBinAtlasPackerChoiceHeuristic::BestLongSideFit,
        MaxRectsBinAtlasPackerChoiceHeuristic::BestShortSideFit,
        MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit,
        MaxRectsBinAtlasPackerChoiceHeuristic::BottomLeftRule,
        MaxRectsBinAtlasPackerChoiceHeuristic::ContactPointRule
    })
    {
        for(bool allow_flip : { false, true })
        {
            packer.allowFlip(allow_flip);
            packer.setChoiceHeuristic(heuristic);
            std::unique_ptr<RawAtlasPack> pack = packer.pack(_sprites, _options);
            if(!result || *pack < *result)
                result.reset(pack.release());
        }
    }
    return result;
}

std::unique_ptr<RawAtlasPack> MetaAtlasPacker::packBestSkylineBinAtlasPacker(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::unique_ptr<RawAtlasPack> result;
    SkylineBinAtlasPacker packer;
    for(auto heuristic : {
        SkylineBinAtlasPackerLevelChoiceHeuristic::BottomLeft,
        SkylineBinAtlasPackerLevelChoiceHeuristic::MinWasteFit
    })
    {
        for(bool use_waste_map : { false, true })
        {
            packer.enableWasteMap(use_waste_map);
            packer.setLevelChoiceHeuristic(heuristic);
            std::unique_ptr<RawAtlasPack> pack = packer.pack(_sprites, _options);
            if(!result || *pack < *result)
                result.reset(pack.release());
        }
    }
    return result;
}

std::unique_ptr<RawAtlasPack> MetaAtlasPacker::packBestGuillotineBinAtlaskPacker(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::unique_ptr<RawAtlasPack> result;
    GuillotineBinAtlaskPacker packer;
    for(auto choice_heuristic : {
        GuillotineBinAtlasPackerChoiceHeuristic::BestAreaFit,
        GuillotineBinAtlasPackerChoiceHeuristic::BestShortSideFit,
        GuillotineBinAtlasPackerChoiceHeuristic::BestLongSideFit,
        GuillotineBinAtlasPackerChoiceHeuristic::WorstAreaFit,
        GuillotineBinAtlasPackerChoiceHeuristic::WorstShortSideFit,
        GuillotineBinAtlasPackerChoiceHeuristic::WorstLongSideFit
    })
    {
        for(auto split_heuristic : {
            GuillotineBinAtlasPackerSplitHeuristic::ShorterLeftoverAxis,
            GuillotineBinAtlasPackerSplitHeuristic::LongerLeftoverAxis,
            GuillotineBinAtlasPackerSplitHeuristic::MinimizeArea,
            GuillotineBinAtlasPackerSplitHeuristic::MaximizeArea,
            GuillotineBinAtlasPackerSplitHeuristic::ShorterAxis,
            GuillotineBinAtlasPackerSplitHeuristic::LongerAxis
        })
        {
            for(bool enable_merge : { false, true })
            {
                packer.enableMerge(enable_merge);
                packer.setChoiceHeuristic(choice_heuristic);
                packer.setSplitHeuristic(split_heuristic);
                std::unique_ptr<RawAtlasPack> pack = packer.pack(_sprites, _options);
                if(!result || *pack < *result)
                    result.reset(pack.release());
            }
        }
    }
    return result;
}

std::unique_ptr<RawAtlasPack> MetaAtlasPacker::packBestShelfBinAtlasPacker(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::unique_ptr<RawAtlasPack> result;
    ShelfBinAtlasPacker packer;
    for(auto heuristic : {
        ShelfBinAtlasPackerChoiceHeuristic::NextFit,
        ShelfBinAtlasPackerChoiceHeuristic::FirstFit,
        ShelfBinAtlasPackerChoiceHeuristic::BestAreaFit,
        ShelfBinAtlasPackerChoiceHeuristic::WorstAreaFit,
        ShelfBinAtlasPackerChoiceHeuristic::BestHeightFit,
        ShelfBinAtlasPackerChoiceHeuristic::BestWidthFit,
        ShelfBinAtlasPackerChoiceHeuristic::WorstWidthFit
    })
    {
        for(bool use_waste_map : { false, true })
        {
            packer.enableWasteMap(use_waste_map);
            packer.setChoiceHeuristic(heuristic);
            std::unique_ptr<RawAtlasPack> pack = packer.pack(_sprites, _options);
            if(!result || *pack < *result)
                result.reset(pack.release());
        }
    }
    return result;
}
