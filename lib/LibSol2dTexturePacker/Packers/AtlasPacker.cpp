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
#include <QCryptographicHash>
#include <QPainter>
#include <QRect>

namespace {

struct Item
{
    Item(
        const QImage * _image,
        const QString & _name,
        const QRect & _source_rect,
        const QRect & _destination_rect,
        bool _is_rotated
    ) :
        image(_image),
        name(_name),
        source_rect(_source_rect),
        destination_rect(_destination_rect),
        is_rotated(_is_rotated)
    {
    }

    const QImage * image;
    QString name;
    QRect source_rect;
    QRect destination_rect;
    QByteArray hash_sum;
    bool is_rotated;
};

QImage render(const std::list<Item> & _items)
{
    int max_x = 0;
    int max_y = 0;
    foreach(const Item & item, _items)
    {
        int x = item.destination_rect.x() + item.destination_rect.width();
        int y = item.destination_rect.y() + item.destination_rect.height();
        if(x > max_x) max_x = x;
        if(y > max_y) max_y = y;
    }
    QImage image(max_x, max_y, QImage::Format_RGBA8888);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QTransform rotation;
    rotation.rotate(90);
    for(const Item & item : _items)
    {
        if(item.image == nullptr)
            continue;
        painter.drawImage(
            item.destination_rect.topLeft(),
            item.is_rotated ? item.image->transformed(rotation) : *item.image,
            item.is_rotated
                ? QRect(item.source_rect.y(), item.source_rect.x(), item.source_rect.height(), item.source_rect.width())
                : item.source_rect);
    }
    return image;
}

QRect crop(const QImage & _image)
{
    int top = 0, bottom = 0, left = 0, right = 0;

    for(int y = 0; y < _image.height(); ++y)
    {
        bool exit = false;
        for(int x = 0; x < _image.width(); ++x)
        {
            if(qAlpha(_image.pixel(x, y)) != 0)
            {
                exit = true;
                break;
            }
        }
        if(exit) break;
        ++top;
    }

    for(int y = _image.height() - 1; y >= 0; --y)
    {
        bool exit = false;
        for(int x = 0; x < _image.width(); ++x)
        {
            if(qAlpha(_image.pixel(x, y)) != 0)
            {
                exit = true;
                break;
            }
        }
        if(exit) break;
        ++bottom;
    }

    for(int x = 0; x < _image.width(); ++x)
    {
        bool exit = false;
        for(int y = _image.height() - bottom - 1; y > top; --y)
        {
            if(qAlpha(_image.pixel(x, y)) != 0)
            {
                exit = true;
                break;
            }
        }
        if(exit) break;
        ++left;
    }

    for(int x = _image.width() - 1; x >= 0; --x)
    {
        bool exit = false;
        for(int y = _image.height() - bottom - 1; y > top; --y)
        {
            if(qAlpha(_image.pixel(x, y)) != 0)
            {
                exit = true;
                break;
            }
        }
        if(exit) break;
        ++right;
    }

    return QRect(left, top, _image.width() - left - right, _image.height() - top - bottom);
}

QList<Frame> itemsToFrames(std::list<Item> _items)
{
    QList<Frame> frames;
    frames.reserve(_items.size());
    for(const Item & item : _items)
    {
        frames.append({
            .texture_rect = item.destination_rect,
            .sprite_rect = item.source_rect,
            .name = item.name,
            .is_rotated = item.is_rotated
        });
    }
    return frames;
}

const Item * findDuplicate(const std::list<Item> & _items, const QByteArray & _hash_sum)
{
    for(const Item & item : _items)
    {
        if(item.hash_sum.compare(_hash_sum) == 0)
            return &item;
    }
    return nullptr;
}

} // namespace name

std::unique_ptr<RawAtlasPack> AtlasPacker::pack(
    const QList<Sprite> & _sprites,
    const AtlasPackerOptions & _options) const
{
    std::list<Item> items;
    std::unique_ptr<RawAtlasPack> result = std::make_unique<RawAtlasPack>();
    std::unique_ptr<AtlasPackerAlgorithm> algorithm = createAlgorithm(_options.max_atlas_size);
    foreach(const Sprite & sprite, _sprites)
    {
        QByteArray hash_sum;
        const Item * duplicate = nullptr;
        if(_options.detect_duplicates)
        {
            hash_sum = QCryptographicHash::hash(
                QByteArrayView(sprite.image.constBits(), sprite.image.sizeInBytes()),
                QCryptographicHash::Md5);
            duplicate = findDuplicate(items, hash_sum);
        }
        if(duplicate)
        {
            items.push_back(*duplicate);
            Item & item = items.back();
            item.image = nullptr;
            item.name = sprite.name;
        }
        else
        {
            QRect sprite_rect = _options.crop ? crop(sprite.image) : sprite.image.rect();
            QRect dest_rect = algorithm->insert(sprite_rect.width(), sprite_rect.height());
            if(dest_rect.isNull() && !items.empty())
            {
                result->add({
                    .image = render(items),
                    .frames = itemsToFrames(items)
                });
                items.clear();
                algorithm->resetBin();
            }
            items.emplace_back(
                &sprite.image,
                sprite.name,
                sprite_rect,
                dest_rect,
                dest_rect.width() == sprite_rect.height());
            if(_options.detect_duplicates)
                items.back().hash_sum = hash_sum;
        }
    }
    if(!items.empty())
    {
        result->add({
            .image = render(items),
            .frames = itemsToFrames(items)
        });
    }
    return result;
}
