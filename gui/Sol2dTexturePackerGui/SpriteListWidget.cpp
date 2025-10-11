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

#include <Sol2dTexturePackerGui/SpriteListWidget.h>
#include <Sol2dTexturePackerGui/ImageFormat.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <QFileDialog>
#include <QMimeData>
#include <QShortcut>

class SpriteListWidget::SpriteListModel : public QAbstractListModel
{
public:
    explicit SpriteListModel(QObject * _parent);
    int rowCount(const QModelIndex & _parent) const override;
    QVariant data(const QModelIndex & _index, int _role) const override;
    bool removeRows(int _row, int _count, const QModelIndex & _parent) override;
    void addSprite(const Sprite & _sprite);
    void setSprites(const QList<Sprite> & _sprites);
    const QList<Sprite> & sprites() const;
    bool canDropMimeData(
        const QMimeData * _data,
        Qt::DropAction _action,
        int _row,
        int _column,
        const QModelIndex & _parent) const override;
    bool dropMimeData(
        const QMimeData * _data,
        Qt::DropAction _action,
        int _row,
        int _column,
        const QModelIndex & _parent) override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::ItemFlags flags(const QModelIndex & _index) const override;
    QStringList mimeTypes() const override;
    QMimeData * mimeData(const QModelIndexList & _indexes) const override;

private:
    static const QString m_mime_sprite;
    QList<Sprite> m_sprites;
};

const QString SpriteListWidget::SpriteListModel::m_mime_sprite("application/vnd.s2tp.sprite");

SpriteListWidget::SpriteListModel::SpriteListModel(QObject * _parent) :
    QAbstractListModel(_parent)
{
}

int SpriteListWidget::SpriteListModel::rowCount(const QModelIndex & _parent) const
{
    if(_parent.isValid())
        return 0;
    return static_cast<int>(m_sprites.count());
}

QVariant SpriteListWidget::SpriteListModel::data(const QModelIndex & _index, int _role) const
{
    switch(_role)
    {
    case Qt::DisplayRole:
        return m_sprites[_index.row()].name;
    case Qt::DecorationRole:
        return m_sprites[_index.row()].image.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    default:
        return QVariant();
    }
}

void SpriteListWidget::SpriteListModel::addSprite(const Sprite & _sprite)
{
    auto it = std::find_if(
        m_sprites.begin(),
        m_sprites.end(),
        [&_sprite](const Sprite & __s) { return __s.path == _sprite.path; });
    if(it == m_sprites.end())
    {
        beginInsertRows(QModelIndex(), m_sprites.count(), m_sprites.count() + 1);
        m_sprites.append(_sprite);
        endInsertRows();
    }
}

void SpriteListWidget::SpriteListModel::setSprites(const QList<Sprite> & _sprites)
{
    beginResetModel();
    m_sprites = _sprites;
    endResetModel();
}

bool SpriteListWidget::SpriteListModel::removeRows(int _row, int _count, const QModelIndex & _parent)
{
    beginRemoveRows(_parent, _row, _row + _count);
    m_sprites.remove(_row, _count);
    endRemoveRows();
    return true;
}

bool SpriteListWidget::SpriteListModel::canDropMimeData(
    const QMimeData * _data,
    Qt::DropAction _action,
    int _row,
    int _column,
    const QModelIndex & _parent) const
{
    Q_UNUSED(_row)
    Q_UNUSED(_column)
    Q_UNUSED(_parent)
    return _action == Qt::IgnoreAction || (_action == Qt::MoveAction && _data->hasFormat(m_mime_sprite));
}

bool SpriteListWidget::SpriteListModel::dropMimeData(
    const QMimeData * _data,
    Qt::DropAction _action,
    int _row,
    int _column,
    const QModelIndex & _parent)
{
    if(!canDropMimeData(_data, _action, _row, _column, _parent))
        return false;
    if(_action == Qt::IgnoreAction)
        return true;

    int idx = _row == -1 ? rowCount(QModelIndex()) : _row;

    QList<Sprite> sprites;
    {
        QByteArray byte_array = _data->data(m_mime_sprite);
        QDataStream stream(&byte_array, QIODevice::ReadOnly);
        Sprite sprite;
        while(!stream.atEnd())
        {
            stream >> sprite;
            sprites.append(sprite);
        }
    }

    if(sprites.empty())
        return false;

    beginInsertRows(QModelIndex(), _row, _row + sprites.count() - 1);
    foreach(const Sprite & sprite, sprites)
        m_sprites.insert(idx++, sprite);
    endInsertRows();
    return true;
}

Qt::DropActions SpriteListWidget::SpriteListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions SpriteListWidget::SpriteListModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

inline const QList<Sprite> & SpriteListWidget::SpriteListModel::sprites() const
{
    return m_sprites;
}

Qt::ItemFlags SpriteListWidget::SpriteListModel::flags(const QModelIndex & _index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(_index);
    if(_index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList SpriteListWidget::SpriteListModel::mimeTypes() const
{
    return QStringList { m_mime_sprite };
}

QMimeData * SpriteListWidget::SpriteListModel::mimeData(const QModelIndexList & _indexes) const
{
    QMimeData * data = new QMimeData;
    QByteArray byte_array;
    QDataStream stream(&byte_array, QIODevice::WriteOnly);
    foreach(const QModelIndex & idx, _indexes)
        stream << m_sprites[idx.row()];
    data->setData(m_mime_sprite, byte_array);
    return data;
}

SpriteListWidget::SpriteListWidget(QWidget * _parent) :
    QWidget(_parent),
    m_open_image_dialog_filter(makeAllReadSupportedImageFormatsFilterString())
{
    setupUi(this);
    m_sprites_model = new SpriteListModel(m_tree_sprites);
    m_tree_sprites->setModel(m_sprites_model);
    m_tree_item_context_menu = new QMenu(this);
    m_tree_item_context_menu->addAction(m_action_remove_sprite);
    connect(m_btn_add_sprites, &QPushButton::clicked, this, &SpriteListWidget::addSprites);
    connect(m_action_remove_sprite, &QAction::triggered, this, &SpriteListWidget::removeSprites);
    if(!m_action_remove_sprite->shortcut().isEmpty())
    {
        connect(
            new QShortcut(m_action_remove_sprite->shortcut(), this),
            &QShortcut::activated,
            this,
            &SpriteListWidget::removeSprites);
    }
    connect(m_tree_sprites, &QTreeView::customContextMenuRequested, this, &SpriteListWidget::showTreeItemContextMentu);
    connect(m_sprites_model, &QAbstractListModel::rowsRemoved, this, &SpriteListWidget::spriteListChanged);
    connect(m_sprites_model, &QAbstractListModel::rowsInserted, this, &SpriteListWidget::spriteListChanged);
    connect(m_sprites_model, &QAbstractListModel::modelReset, this, &SpriteListWidget::spriteListChanged);
}

void SpriteListWidget::addSprites()
{
    QSettings settings;
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        QString(),
        settings.value(Settings::Input::sprite_directory).toString(),
        m_open_image_dialog_filter
        );
    if(files.isEmpty())
        return;
    settings.setValue(
        Settings::Input::sprite_directory,
        std::filesystem::absolute(std::filesystem::path(files[0].toStdString()).parent_path()).c_str()
        );
    foreach(const QString & file, files) {
        QImage image;
        if(!image.load(file)) continue;
        QFileInfo fi(file);
        m_sprites_model->addSprite(
            Sprite
            {
                .path = fi.absoluteFilePath(),
                .name = fi.fileName(),
                .image = image
            }
        );
    }
}

void SpriteListWidget::removeSprites()
{
    const QItemSelection selection = m_tree_sprites->selectionModel()->selection();
    std::list<int> rows;
    foreach(const QModelIndex & idx, selection.indexes())
    {
        const int r = idx.row();
        if(rows.empty() || r < rows.back()) // sort desc
            rows.push_back(r);
        else
            rows.push_front(r);
    }
    for(const int row : rows)
        m_sprites_model->removeRow(row);
}

void SpriteListWidget::showTreeItemContextMentu(const QPoint & _pos)
{
    QModelIndex index = m_tree_sprites->indexAt(_pos);
    if(index.isValid())
        m_tree_item_context_menu->exec(m_tree_sprites->mapToGlobal(_pos));
}

const QList<Sprite> & SpriteListWidget::sprites() const
{
    return m_sprites_model->sprites();
}

void SpriteListWidget::setSprites(const QList<Sprite> & _sprites)
{
    m_sprites_model->setSprites(_sprites);
}
