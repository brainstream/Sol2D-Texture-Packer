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

class SpriteListWidget::SpriteListModel : public QAbstractListModel
{
public:
    explicit SpriteListModel(QObject * _parent);
    int rowCount(const QModelIndex & _parent) const override;
    QVariant data(const QModelIndex & _index, int _role) const override;
    bool removeRows(int _row, int _count, const QModelIndex & _parent) override;
    void addSprite(const Sprite & _sprite);
    const QList<Sprite> & getSprites() const;

private:
    QList<Sprite> m_sprites;
};

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

bool SpriteListWidget::SpriteListModel::removeRows(int _row, int _count, const QModelIndex & _parent)
{
    beginRemoveRows(_parent, _row, _row + _count);
    m_sprites.remove(_row, _count);
    endRemoveRows();
    return true;
}

inline const QList<Sprite> & SpriteListWidget::SpriteListModel::getSprites() const
{
    return m_sprites;
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
    connect(m_tree_sprites, &QTreeView::customContextMenuRequested, this, &SpriteListWidget::showTreeItemContextMentu);
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
    emit spriteListChanged();
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
    emit spriteListChanged();
}

void SpriteListWidget::showTreeItemContextMentu(const QPoint & _pos)
{
    QModelIndex index = m_tree_sprites->indexAt(_pos);
    if(index.isValid())
        m_tree_item_context_menu->exec(m_tree_sprites->mapToGlobal(_pos));
}

const QList<Sprite> & SpriteListWidget::getSprites() const
{
    return m_sprites_model->getSprites();
}
