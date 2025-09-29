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
#include <QPainter>

namespace {

struct Item
{
    const Sprite & sprite;
    rbp::Rect rect;
    bool is_rotated;
};

QPixmap render(const QList<Item> & _items)
{
    int max_x = 0;
    int max_y = 0;
    foreach(const Item & item, _items)
    {
        int x = item.rect.x + item.rect.width;
        int y = item.rect.y + item.rect.height;
        if(x > max_x) max_x = x;
        if(y > max_y) max_y = y;
    }
    QPixmap pixmap(max_x, max_y);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QTransform rotation;
    rotation.rotate(90);
    foreach(const Item & item, _items)
    {
        painter.drawPixmap(
            item.rect.x,
            item.rect.y,
            item.rect.width,
            item.rect.height,
            item.is_rotated ? item.sprite.pixmap.transformed(rotation) : item.sprite.pixmap);
    }
    return pixmap;
}

inline rbp::MaxRectsBinPack::FreeRectChoiceHeuristic map(MaxRectsBinAtlasPackerChoiceHeuristic _heuristic)
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

} // namespace name

MaxRectsBinAtlasPacker::MaxRectsBinAtlasPacker(QObject * _parent) :
    AtlasPacker(_parent),
    m_max_atlas_size{1024, 1024},
    m_heuristic(MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit),
    m_allow_flip(false)
{
}

QList<QPixmap> MaxRectsBinAtlasPacker::pack(const QList<Sprite> & _sprites) const
{
    QList<Item> items;
    QList<QPixmap> result;
    items.reserve(_sprites.size());
    rbp::MaxRectsBinPack algorithm(
        m_max_atlas_size.width(),
        m_max_atlas_size.height(),
        m_allow_flip);
    const rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = map(m_heuristic);
    foreach(const Sprite & sprite, _sprites)
    {
        QRect pixmap_rect = sprite.pixmap.rect();
        rbp::Rect rect = algorithm.Insert(
            pixmap_rect.width(),
            pixmap_rect.height(),
            heuristic);
        if((rect.width == 0 || rect.height == 0) && !items.empty())
        {
            result.append(render(items));
            items.clear();
            algorithm.Init(
                m_max_atlas_size.width(),
                m_max_atlas_size.height(),
                m_allow_flip);
        }
        items.append(
            Item
            {
                .sprite = sprite,
                .rect = rect,
                .is_rotated = rect.width == pixmap_rect.height()
            }
        );
    }
    if(!items.empty())
        result.append(render(items));
    return result; // TODO: return atlas frames
}
