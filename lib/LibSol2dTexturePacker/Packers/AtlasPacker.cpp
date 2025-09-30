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

#include <LibSol2dTexturePacker/Packers/AtlasPacker.h>
#include <QPainter>

namespace {

struct Item
{
    const Sprite & sprite;
    QRect rect;
    bool is_rotated;
};

QPixmap render(const QList<Item> & _items)
{
    int max_x = 0;
    int max_y = 0;
    foreach(const Item & item, _items)
    {
        int x = item.rect.x() + item.rect.width();
        int y = item.rect.y() + item.rect.height();
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
            item.rect.x(),
            item.rect.y(),
            item.rect.width(),
            item.rect.height(),
            item.is_rotated ? item.sprite.pixmap.transformed(rotation) : item.sprite.pixmap);
    }
    return pixmap;
}

} // namespace name

QList<QPixmap> AtlasPacker::pack(const QList<Sprite> & _sprites, const AtlasPackerOptions & _options) const
{
    QList<Item> items;
    QList<QPixmap> result;
    items.reserve(_sprites.size());
    std::unique_ptr<AtlasPackerAlgorithm> algorithm = createAlgorithm(_options.max_atlas_size);
    foreach(const Sprite & sprite, _sprites)
    {
        QRect pixmap_rect = sprite.pixmap.rect();
        QRect rect = algorithm->insert(pixmap_rect.width(), pixmap_rect.height());
        if(rect.isNull() && !items.empty())
        {
            result.append(render(items));
            items.clear();
            algorithm->resetBin();
        }
        items.append(
            Item
            {
                .sprite = sprite,
                .rect = rect,
                .is_rotated = rect.width() == pixmap_rect.height()
            }
        );
    }
    if(!items.empty())
    {
        result.append(render(items));
    }
    return result;
}
