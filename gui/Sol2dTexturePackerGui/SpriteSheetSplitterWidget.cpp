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

#include <Sol2dTexturePackerGui/SpriteSheetSplitterWidget.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <LibSol2dTexturePacker/Atlas/DefaultAtlasSerializer.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>

SpriteSheetSplitterWidget::SpriteSheetSplitterWidget(QWidget *parent) :
    QWidget(parent),
    m_pixmap(new QPixmap()),
    m_sheet_pen(QColor(0, 0, 0, 150)),
    m_sheet_brush(QColor(255, 255, 255, 180)),
    m_sprite_pen(QColor(255, 0, 0, 80)),
    m_sprite_brush(QColor(255, 0, 0, 50))
{
    m_open_image_dialog_filter = QString(tr("All supported image formats (*.%1)"))
        .arg(QImageReader().supportedImageFormats().join(" *."));
    setupUi(this);
    m_preview->setScene(new QGraphicsScene(this));
    m_preview->setZoomModel(&m_zoom_widget->model());
    setExportControlsEnabled(false);
    m_grid_splitter = new GridSplitter(this);
    m_atlas_splitter = new AtlasSplitter(this);
    m_current_splitter = m_grid_splitter;

    connect(m_btn_browse_texture_file, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::openTexture);
    connect(m_btn_export_sprites, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::exportSprites);
    connect(m_btn_export_to_atlas, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::exportToAtlas);
    connect(m_btn_brows_data_file, &QPushButton::clicked, this, &SpriteSheetSplitterWidget::openAtlas);
    connect(m_spin_rows, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setRowCount);
    connect(m_spin_columns, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setColumnCount);
    connect(m_spin_sprite_width, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setSpriteWidth);
    connect(m_spin_sprite_height, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setSpriteHeight);
    connect(m_spin_margin_left, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setMarginLeft);
    connect(m_spin_margin_top, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setMarginTop);
    connect(m_spin_hspacing, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setHorizontalSpacing);
    connect(m_spin_vspacing, &QSpinBox::valueChanged, m_grid_splitter, &GridSplitter::setVerticalSpacing);
    connect(m_grid_splitter, &Splitter::framesChanged, this, &SpriteSheetSplitterWidget::syncWithSplitter);
    connect(m_atlas_splitter, &Splitter::framesChanged, this, &SpriteSheetSplitterWidget::syncWithSplitter);
    connect(m_atlas_splitter, &AtlasSplitter::error, this, &SpriteSheetSplitterWidget::showError);
    connect(m_tabs_source, &QTabWidget::currentChanged, this, [this](int __index) {
        switch(__index)
        {
        case 0:
            m_current_splitter = m_grid_splitter;
            break;
        case 1:
            m_current_splitter = m_atlas_splitter;
            break;
        default:
            return;
        }
        syncWithSplitter();
    });
}

SpriteSheetSplitterWidget::~SpriteSheetSplitterWidget()
{
    delete m_pixmap;
}

void SpriteSheetSplitterWidget::openTexture()
{
    QSettings settings;
    QString last_dir = settings.value(gc_settings_key_sheet_dir).toString();
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Open a Sprite Sheet"),
        last_dir,
        m_open_image_dialog_filter);
    if(!filename.isEmpty())
    {
        m_grid_splitter->reset();
        m_atlas_splitter->reset();
        QFileInfo file_info(filename);
        settings.setValue(gc_settings_key_sheet_dir, file_info.absolutePath());
        loadImage(filename);
    }
}

void SpriteSheetSplitterWidget::loadImage(const QString & _path)
{
    if(m_pixmap->load(_path))
    {
        m_preview->scene()->setSceneRect(0, 0, m_pixmap->width(), m_pixmap->height());
        m_edit_texture_size->setText(QString("%1x%2").arg(m_pixmap->width()).arg(m_pixmap->height()));
        m_edit_texture_file->setText(_path);
        m_spin_rows->setValue(0);
        m_spin_columns->setValue(0);
        m_spin_sprite_width->setValue(0);
        m_spin_sprite_height->setValue(0);
        m_spin_vspacing->setValue(0);
        m_spin_hspacing->setValue(0);
        m_spin_margin_left->setValue(0);
        m_spin_margin_top->setValue(0);
        m_tabs_source->setEnabled(true);
        m_zoom_widget->model().setZoom(100);
        syncWithSplitter();
        emit sheetLoaded(_path);
    }
    else
    {
        QMessageBox::critical(this, nullptr, tr("Unable to load texture"));
    }
}

void SpriteSheetSplitterWidget::setExportControlsEnabled(bool _enabled)
{
    m_btn_export_sprites->setEnabled(_enabled);
    m_btn_export_to_atlas->setEnabled(_enabled);
}

void SpriteSheetSplitterWidget::syncWithSplitter()
{
    QGraphicsScene * scene = m_preview->scene();
    scene->clear();
    QGraphicsPixmapItem * pixmap_item = scene->addPixmap(*m_pixmap);
    scene->addRect({pixmap_item->pos(), m_pixmap->size()}, m_sheet_pen, m_sheet_brush);
    pixmap_item->setZValue(1);
    bool is_valid = m_current_splitter->forEachFrame([this, scene](const Frame & __frame) {
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
    m_current_splitter->apply(
        Texture { .path = dir_path.toStdString(), .image = m_pixmap->toImage() },
        QDir(dir_path)
    );
}

void SpriteSheetSplitterWidget::exportToAtlas()
{
    DefaultAtlasSerializer serializer;
    std::filesystem::path texture_file_path(m_edit_texture_file->text().toStdString());
    QString default_filename = m_last_atlas_export_file;
    if(default_filename.isEmpty())
    {
        std::filesystem::path atlas_file_path = texture_file_path;
        atlas_file_path.replace_extension(serializer.defaultFileExtenstion());
        default_filename = atlas_file_path.c_str();
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
        atlas.frames.reserve(m_current_splitter->frameCount());
        m_current_splitter->forEachFrame([&atlas](const Frame & __frame) {
            atlas.frames.append(__frame);
        });
        try
        {
            serializer.serialize(atlas, filename.toStdString());
        }
        catch(const Exception & _exception)
        {
            QMessageBox::critical(this, nullptr, _exception.message());
        }
    }
}

void SpriteSheetSplitterWidget::openAtlas()
{
    QString defatult_dir = m_edit_data_file->text();
    if(defatult_dir.isEmpty())
        defatult_dir = m_edit_texture_file->text();
    if(!defatult_dir.isEmpty())
        defatult_dir = QFileInfo(defatult_dir).path();
    QString filename = QFileDialog::getOpenFileName(this, QString(), defatult_dir);
    if(!filename.isEmpty())
    {
        m_edit_data_file->setText(filename);
        m_atlas_splitter->setDataFile(filename);
    }
}

void SpriteSheetSplitterWidget::showError(const QString & _message)
{
    QMessageBox::critical(this, nullptr, _message);
}
