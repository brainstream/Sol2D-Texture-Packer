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
#include <QDragEnterEvent>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

constexpr char g_mime_sprite[] = "application/vnd.s2tp.sprite";
constexpr char g_mime_uri_list[] = "text/uri-list";

} // namespace

class SpriteListWidget::SpriteTreeView : public QTreeView
{
public:
    explicit SpriteTreeView(QWidget * _parent);

protected:
    void dragEnterEvent(QDragEnterEvent * _event) override;
    void dragMoveEvent(QDragMoveEvent * _event) override;
    void dropEvent(QDropEvent * _event) override;

private:
    SpriteListWidget::SpriteListModel * spriteListModel() const;
};

class SpriteListWidget::SpriteListModel : public QAbstractListModel
{
public:
    explicit SpriteListModel(QObject * _parent);
    int rowCount(const QModelIndex & _parent) const override;
    QVariant data(const QModelIndex & _index, int _role) const override;
    bool removeRows(int _row, int _count, const QModelIndex & _parent) override;
    void addSprite(const Sprite & _sprite);
    void addSprites(const QList<Sprite> & _sprites);
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
    QList<Sprite> m_sprites;
};

inline SpriteListWidget::SpriteTreeView::SpriteTreeView(QWidget * _parent) :
    QTreeView(_parent)
{
}

inline SpriteListWidget::SpriteListModel * SpriteListWidget::SpriteTreeView::spriteListModel() const
{
    return static_cast<SpriteListWidget::SpriteListModel *>(model());
}

void SpriteListWidget::SpriteTreeView::dragEnterEvent(QDragEnterEvent * _event)
{
    if(_event->mimeData()->hasFormat(g_mime_uri_list) && _event->mimeData()->hasUrls())
    {
        _event->setDropAction(Qt::CopyAction);
        _event->accept();
    }
    else if(_event->mimeData()->hasFormat(g_mime_sprite))
    {
        _event->acceptProposedAction();
    }
    else
    {
        QTreeView::dragMoveEvent(_event);
    }
}

void SpriteListWidget::SpriteTreeView::dragMoveEvent(QDragMoveEvent * _event)
{
    if(_event->mimeData()->hasFormat(g_mime_uri_list) && _event->mimeData()->hasUrls())
    {
        _event->setDropAction(Qt::CopyAction);
        _event->accept();
    }
    else
    {
        QTreeView::dragMoveEvent(_event);
    }
}

void SpriteListWidget::SpriteTreeView::dropEvent(QDropEvent * _event)
{
    if(_event->mimeData()->hasFormat(g_mime_uri_list) && _event->mimeData()->hasUrls())
    {
        QList<Sprite> sprites;
        foreach(const QUrl & url, _event->mimeData()->urls())
        {
            if(!url.isLocalFile())
                continue;
            const QFileInfo fi(url.toLocalFile());
            const QString absolute_path = fi.absoluteFilePath();
            const QImage img(absolute_path);
            if(img.isNull())
                continue;
            sprites.append({
                .path = absolute_path,
                .name = fi.baseName(),
                .image = img
            });
        }
        spriteListModel()->addSprites(sprites);
        _event->acceptProposedAction();
    }
    else
    {
        QTreeView::dropEvent(_event);
    }
}

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
    addSprites({ _sprite });
}

void SpriteListWidget::SpriteListModel::addSprites(const QList<Sprite> & _sprites)
{
    QList<Sprite> sprites;
    sprites.reserve(_sprites.count());
    foreach(const Sprite & s, _sprites)
    {
        auto it = std::find_if(
            m_sprites.begin(),
            m_sprites.end(),
            [&s](const Sprite & __s) { return __s.path == s.path; });
        if(it == m_sprites.end())
            sprites.append(s);
    }
    if(!sprites.isEmpty())
    {
        beginInsertRows(QModelIndex(), m_sprites.count(), m_sprites.count() + sprites.count());
        m_sprites.append(sprites);
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
    return _action == Qt::IgnoreAction || (_action == Qt::MoveAction && _data->hasFormat(g_mime_sprite));
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
        QByteArray byte_array = _data->data(g_mime_sprite);
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
    Qt::ItemFlags default_flags = QAbstractListModel::flags(_index);
    if(_index.isValid())
        return Qt::ItemIsDragEnabled | default_flags;
    else
        return Qt::ItemIsDropEnabled | default_flags;
}

QStringList SpriteListWidget::SpriteListModel::mimeTypes() const
{
    return QStringList { g_mime_sprite };
}

QMimeData * SpriteListWidget::SpriteListModel::mimeData(const QModelIndexList & _indexes) const
{
    QMimeData * data = new QMimeData;
    QByteArray byte_array;
    QDataStream stream(&byte_array, QIODevice::WriteOnly);
    foreach(const QModelIndex & idx, _indexes)
        stream << m_sprites[idx.row()];
    data->setData(g_mime_sprite, byte_array);
    return data;
}

SpriteListWidget::SpriteListWidget(QWidget * _parent) :
    QWidget(_parent),
    m_open_image_dialog_filter(makeAllReadSupportedImageFormatsFilterString())
{
    m_tree_sprites = new SpriteTreeView(this);
    m_tree_sprites->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    m_tree_sprites->setAcceptDrops(true);
    m_tree_sprites->setDragEnabled(true);
    m_tree_sprites->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    m_tree_sprites->setDefaultDropAction(Qt::DropAction::MoveAction);
    m_tree_sprites->setDropIndicatorShown(true);
    m_tree_sprites->setAlternatingRowColors(true);
    m_tree_sprites->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    m_tree_sprites->setRootIsDecorated(false);
    m_tree_sprites->setUniformRowHeights(true);
    m_tree_sprites->setItemsExpandable(false);
    m_tree_sprites->header()->setVisible(false);
    m_tree_sprites->setModel(m_sprites_model = new SpriteListModel(m_tree_sprites));

    QShortcut * shortcut_remove_sprite = new QShortcut(
        QKeySequence(Qt::Key_Delete),
        m_tree_sprites,
        nullptr,
        nullptr,
        Qt::WidgetShortcut);
    QAction * action_remove_sprite = new QAction(QIcon(":/icons/list-remove"), tr("Remove"), this);
    action_remove_sprite->setShortcut(action_remove_sprite->shortcut());
    QPushButton * btn_add_sprites = new QPushButton(QIcon(":/icons/list-add"), tr("Add Sprites"), this);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_tree_sprites);
    layout->addWidget(btn_add_sprites);

    m_tree_item_context_menu = new QMenu(this);
    m_tree_item_context_menu->addAction(action_remove_sprite);

    connect(btn_add_sprites, &QPushButton::clicked, this, &SpriteListWidget::addSprites);
    connect(action_remove_sprite, &QAction::triggered, this, &SpriteListWidget::removeSprites);
    connect(shortcut_remove_sprite, &QShortcut::activated, this, &SpriteListWidget::removeSprites);
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
    QList<Sprite> sprites;
    sprites.reserve(files.count());
    foreach(const QString & file, files) {
        QImage image;
        if(!image.load(file)) continue;
        QFileInfo fi(file);
        sprites.append(            {
            .path = fi.absoluteFilePath(),
            .name = fi.fileName(),
            .image = image
        });
    }
    m_sprites_model->addSprites(sprites);
}

void SpriteListWidget::removeSprites()
{
    const QItemSelection selection = m_tree_sprites->selectionModel()->selection();
    int next_selected_row = INT_MAX;
    std::list<int> rows;
    foreach(const QModelIndex & idx, selection.indexes())
    {
        const int r = idx.row();
        if(rows.empty() || r < rows.back()) // sort desc
            rows.push_back(r);
        else
            rows.push_front(r);
        if(r < next_selected_row)
            next_selected_row = r;
    }
    for(const int row : rows)
        m_sprites_model->removeRow(row);
    if(next_selected_row >= m_sprites_model->sprites().count())
        next_selected_row = m_sprites_model->sprites().count() - 1;
    m_tree_sprites->setCurrentIndex(m_sprites_model->index(next_selected_row));
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
