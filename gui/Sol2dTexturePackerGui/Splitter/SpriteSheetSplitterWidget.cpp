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

#include <Sol2dTexturePackerGui/Splitter/SpriteSheetSplitterWidget.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <LibSol2dTexturePacker/Pack/GridPack.h>
#include <LibSol2dTexturePacker/Pack/AtlasPack.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <LibSol2dTexturePacker/Atlas/DefaultAtlasSerializer.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>

SpriteSheetSplitterWidget::SpriteSheetSplitterWidget(QWidget * _parent) :
    QWidget(_parent),
    m_sheet_pen(QColor(0, 0, 0, 150)),
    m_sheet_brush(QColor(255, 255, 255, 180)),
    m_sprite_pen(QColor(255, 0, 0, 80)),
    m_sprite_brush(QColor(255, 0, 0, 50)),
    m_pack(nullptr)
{
    m_open_image_dialog_filter = QString(tr("All supported image formats (*.%1)"))
        .arg(QImageReader().supportedImageFormats().join(" *."));
    setupUi(this);
    m_preview->setScene(new QGraphicsScene(this));
    m_preview->setZoomModel(&m_zoom_widget->model());
    setExportControlsEnabled(false);
    connect(m_btn_open_texture, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::openTexture);
    connect(m_btn_open_atlas, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::openAtlas);
    connect(m_btn_export_sprites, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::exportSprites);
    connect(m_btn_export_to_atlas, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::exportToAtlas);
}

void SpriteSheetSplitterWidget::openTexture()
{
    QSettings settings;
    QString last_dir = settings.value(gc_settings_key_sheet_dir).toString();
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Open a Texture"),
        last_dir,
        m_open_image_dialog_filter);
    if(!filename.isEmpty())
    {
        QFileInfo file_info(filename);
        settings.setValue(gc_settings_key_sheet_dir, file_info.absolutePath());
        GridPack * grid_pack = new GridPack(filename, this);
        connect(grid_pack, &GridPack::framesChanged, this, &SpriteSheetSplitterWidget::syncWithPack);
        m_pack = QSharedPointer<Pack>(grid_pack, [this, grid_pack](Pack *) {
            disconnect(grid_pack, &GridPack::framesChanged, this, &SpriteSheetSplitterWidget::syncWithPack);
            grid_pack->deleteLater();
        });
        m_page_grid->setPack(grid_pack);
        m_page_atlas->setPack(nullptr);
        m_stacked_widget_properties->setCurrentWidget(m_page_grid);
        applyNewTexture();
    }
}

void SpriteSheetSplitterWidget::applyNewTexture()
{
        m_preview->scene()->setSceneRect(0, 0, m_pack->texture().width(), m_pack->texture().height());
        m_zoom_widget->model().setZoom(100);
        syncWithPack();
        emit sheetLoaded(m_pack->textureFilename());
}

void SpriteSheetSplitterWidget::setExportControlsEnabled(bool syncWithPack)
{
    m_btn_export_sprites->setEnabled(syncWithPack);
    m_btn_export_to_atlas->setEnabled(syncWithPack);
}

void SpriteSheetSplitterWidget::syncWithPack()
{
    QGraphicsScene * scene = m_preview->scene();
    scene->clear();
    QGraphicsPixmapItem * pixmap_item = scene->addPixmap(QPixmap::fromImage(m_pack->texture()));
    scene->addRect({pixmap_item->pos(), m_pack->texture().size()}, m_sheet_pen, m_sheet_brush);
    pixmap_item->setZValue(1);
    bool is_valid = m_pack->forEachFrame([this, scene](const Frame & __frame) {
        QGraphicsRectItem * item = scene->addRect(
            __frame.x,
            __frame.y,
            __frame.width,
            __frame.height,
            m_sprite_pen,
            m_sprite_brush);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
    });
    setExportControlsEnabled(is_valid);
}

void SpriteSheetSplitterWidget::exportSprites()
{
    QSettings settings;
    QString last_dir = settings.value(gc_settings_key_split_dir, gc_settings_key_sheet_dir).toString();
    QString dir_path = QFileDialog::getExistingDirectory(this, QString(), last_dir);
    if(dir_path.isEmpty())
        return;
    settings.setValue(gc_settings_key_split_dir, dir_path);
    m_pack->unpack(dir_path);
}

void SpriteSheetSplitterWidget::exportToAtlas()
{
    if(!m_pack)
    {
        return;
    }
    DefaultAtlasSerializer serializer;
    QString texture_file_path = m_pack->textureFilename(); // TODO: bad default!
    QString default_filename = m_last_atlas_export_file;
    if(default_filename.isEmpty())
    {
        const QFileInfo texture_fi(texture_file_path);
        default_filename = texture_fi
            .dir()
            .absoluteFilePath(QString("%1.%2")
            .arg(texture_fi.baseName(), serializer.defaultFileExtenstion()));
    }
    QString filename = QFileDialog::getSaveFileName(
        this,
        QString(),
        default_filename,
        QString(tr("Atlas (%1) (*.%1)")).arg(serializer.defaultFileExtenstion()));
    if(!filename.isEmpty())
    {
        m_last_atlas_export_file = filename;
        Atlas atlas
        {
            .texture = texture_file_path,
            .frames = QList<Frame>()
        };
        atlas.frames.reserve(m_pack->frameCount());
        m_pack->forEachFrame([&atlas](const Frame & __frame) {
            atlas.frames.append(__frame);
        });
        try
        {
            serializer.serialize(atlas, filename);
        }
        catch(const Exception & _exception)
        {
            QMessageBox::critical(this, nullptr, _exception.message());
        }
    }
}

void SpriteSheetSplitterWidget::openAtlas()
{
    QSettings settings;
    QString last_dir = settings.value(gc_settings_key_atlas_dir).toString();
    QString filename = QFileDialog::getOpenFileName(this, QString(), last_dir);
    if(!filename.isEmpty())
    {
        try
        {
            QFileInfo file_info(filename);
            filename = file_info.absoluteFilePath();
            Atlas atlas;
            DefaultAtlasSerializer serializer;
            serializer.deserialize(filename, atlas);
            AtlasPack * atlas_pack = new AtlasPack(atlas, this);
            m_pack = QSharedPointer<Pack>(atlas_pack);
            m_page_atlas->setPack(atlas_pack);
            m_page_grid->setPack(nullptr);
            m_stacked_widget_properties->setCurrentWidget(m_page_atlas);
            settings.setValue(gc_settings_key_atlas_dir, file_info.absolutePath());
            applyNewTexture();
        }
        catch(const Exception & exception)
        {
            QMessageBox::critical(this, nullptr, exception.message());
        }
    }
}
