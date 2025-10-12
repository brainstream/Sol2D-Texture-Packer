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

#include <Sol2dTexturePackerGui/Packer/SpritePackerWidget.h>
#include <Sol2dTexturePackerGui/TransparentGraphicsPixmapItem.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <LibSol2dTexturePacker/Packers/MaxRectsBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/SkylineBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/GuillotineBinAtlaskPacker.h>
#include <LibSol2dTexturePacker/Packers/ShelfBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/MetaAtlasPacker.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractListModel>
#include <QImageWriter>

namespace {

enum class PackAlgorithm
{
    MaxRectsBin,
    SkylineBinPack,
    GuillotineBinPack,
    ShelfBinPack,
    Meta
};

} // namespace name

struct SpritePackerWidget::Packers
{
    MaxRectsBinAtlasPacker max_rects_bin;
    SkylineBinAtlasPacker skyline_bin;
    GuillotineBinAtlaskPacker guillotine_bin;
    ShelfBinAtlasPacker shelf_bin;
    MetaAtlasPacker meta;
    AtlasPacker * current;
};


SpritePackerWidget::SpritePackerWidget(QWidget * _parent) :
    QWidget(_parent),
    m_packers(new Packers),
    m_last_calulated_size(2048, 2048)
{
    setupUi(this);

    m_thread = new BusySmartThread(this);
    m_packers->current = nullptr;
    m_packers->max_rects_bin.setChoiceHeuristic(MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit);

    m_spin_max_width->setValue(m_last_calulated_size.width());
    m_spin_max_height->setValue(m_last_calulated_size.height());
    m_preview->setScene(new QGraphicsScene(m_preview));
    m_preview->setZoomModel(&m_zoom_widget->model());
    m_splitter->setSizes({100, 500});

    m_combo_algorithm->addItem("Max Rects", static_cast<int>(PackAlgorithm::MaxRectsBin));
    m_combo_algorithm->addItem("Skyline", static_cast<int>(PackAlgorithm::SkylineBinPack));
    m_combo_algorithm->addItem("Guillotine", static_cast<int>(PackAlgorithm::GuillotineBinPack));
    m_combo_algorithm->addItem("Shelf", static_cast<int>(PackAlgorithm::ShelfBinPack));
    m_combo_algorithm->addItem("Auto", static_cast<int>(PackAlgorithm::Meta));

    m_combo_mrb_heuristic->addItem(
        tr("Best Long Side Fit"),
        static_cast<int>(MaxRectsBinAtlasPackerChoiceHeuristic::BestLongSideFit));
    m_combo_mrb_heuristic->addItem(
        tr("Best Short Side Fit"),
        static_cast<int>(MaxRectsBinAtlasPackerChoiceHeuristic::BestShortSideFit));
    m_combo_mrb_heuristic->addItem(
        tr("Best Area Fit"),
        static_cast<int>(MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit));
    m_combo_mrb_heuristic->addItem(
        tr("Bottom Left Rule"),
        static_cast<int>(MaxRectsBinAtlasPackerChoiceHeuristic::BottomLeftRule));
    m_combo_mrb_heuristic->addItem(
        tr("Contact Point Rule"),
        static_cast<int>(MaxRectsBinAtlasPackerChoiceHeuristic::ContactPointRule));

    m_combo_skyline_heuristic->addItem(
        tr("Bottom Left"),
        static_cast<int>(SkylineBinAtlasPackerLevelChoiceHeuristic::BottomLeft));
    m_combo_skyline_heuristic->addItem(
        tr("Min Waste Fit"),
        static_cast<int>(SkylineBinAtlasPackerLevelChoiceHeuristic::MinWasteFit));

    m_combo_guillotine_choice_heuristic->addItem(
        tr("Best Area Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::BestAreaFit));
    m_combo_guillotine_choice_heuristic->addItem(
        tr("Best Long Side Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::BestLongSideFit));
    m_combo_guillotine_choice_heuristic->addItem(
        tr("Best Short Side Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::BestShortSideFit));
    m_combo_guillotine_choice_heuristic->addItem(
        tr("Worst Area Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::WorstAreaFit));
    m_combo_guillotine_choice_heuristic->addItem(
        tr("Worst Short Side Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::WorstShortSideFit));
    m_combo_guillotine_choice_heuristic->addItem(
        tr("Worst Long Side Fit"),
        static_cast<int>(GuillotineBinAtlasPackerChoiceHeuristic::WorstLongSideFit));

    m_combo_guillotine_split_heuristic->addItem(
        tr("Shorter Leftover Axis)"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::ShorterLeftoverAxis));
    m_combo_guillotine_split_heuristic->addItem(
        tr("Longer Leftover Axis"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::LongerLeftoverAxis));
    m_combo_guillotine_split_heuristic->addItem(
        tr("Minimize Area"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::MinimizeArea));
    m_combo_guillotine_split_heuristic->addItem(
        tr("Maximize Area"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::MaximizeArea));
    m_combo_guillotine_split_heuristic->addItem(
        tr("Shorter Axis"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::ShorterAxis));
    m_combo_guillotine_split_heuristic->addItem(
        tr("Longer Axis"),
        static_cast<int>(GuillotineBinAtlasPackerSplitHeuristic::LongerAxis));

    m_combo_shelf_choice_heuristic->addItem(
        tr("Next Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::NextFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("First Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::FirstFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("Best Area Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::BestAreaFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("Worst Area Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::WorstAreaFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("Best Height Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::BestHeightFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("Best Width Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::BestWidthFit));
    m_combo_shelf_choice_heuristic->addItem(
        tr("Worst Width Fit"),
        static_cast<int>(ShelfBinAtlasPackerChoiceHeuristic::WorstWidthFit));

    m_combo_atlas_format->addItem(tr("Sol2D"));

    {
        QList<QByteArray> supported_image_formats = QImageWriter::supportedImageFormats();
        int png_idx = -1;
        for(int i = 0; i < supported_image_formats.count(); ++i)
        {
            QString format(supported_image_formats[i]);
            m_combo_texture_format->addItem(format);
            if(format == "png") png_idx = i;
        }
        m_combo_texture_format->setCurrentIndex(png_idx);
    }

    QTimer::singleShot(100, this, [this]() {
        emit packNameChanged(m_edit_export_name->text());
    });

    connect(m_thread, &BusySmartThread::success, this, &SpritePackerWidget::onRenderPackFinished);
    connect(m_thread, &BusySmartThread::failed, this, &SpritePackerWidget::onRenderPackError);
    connect(m_widget_sprite_list, &SpriteListWidget::spriteListChanged, this, &SpritePackerWidget::renderPack);
    connect(m_checkbox_crop, &QCheckBox::checkStateChanged, this, &SpritePackerWidget::renderPack);
    connect(m_checkbox_detect_duplicates, &QCheckBox::checkStateChanged, this, &SpritePackerWidget::renderPack);
    connect(m_spin_max_width, &QSpinBox::editingFinished, this, &SpritePackerWidget::onTextureWidthChanged);
    connect(m_spin_max_height, &QSpinBox::editingFinished, this, &SpritePackerWidget::onTextureHeightChanged);
    connect(m_btn_export, &QPushButton::clicked, this, &SpritePackerWidget::exportPack);
    connect(m_btn_browse_export_directory, &QPushButton::clicked, this, &SpritePackerWidget::browseForExportDir);
    connect(m_edit_export_directory, &QLineEdit::textChanged, this, &SpritePackerWidget::validateExportPackRequirements);
    connect(m_edit_export_name, &QLineEdit::textChanged, this, &SpritePackerWidget::validateExportPackRequirements);
    connect(m_edit_export_name, &QLineEdit::textChanged, this, &SpritePackerWidget::packNameChanged);
    connect(
        m_checkbox_mrb_allow_flip,
        &QCheckBox::checkStateChanged,
        this,
        &SpritePackerWidget::onMaxRectesBiAllowFlipChanged);
    connect(
        m_combo_mrb_heuristic,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onMaxRectesBinHeuristicChanged);
    connect(
        m_combo_algorithm,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onAlgorithmChanged);
    connect(
        m_combo_skyline_heuristic,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onSkylineBinHeuristicChanged);
    connect(
        m_checkbox_skyline_use_waste_map,
        &QCheckBox::checkStateChanged,
        this,
        &SpritePackerWidget::onSkylineBinUseWasteMapChanged);
    connect(
        m_combo_guillotine_choice_heuristic,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onGuillotineBinChoiceHeuristicChanged);
    connect(
        m_combo_guillotine_split_heuristic,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onGuillotineBinSplitHeuristicChanged);
    connect(
        m_checkbox_guillotine_allow_merge,
        &QCheckBox::checkStateChanged,
        this,
        &SpritePackerWidget::onGuillotineBinAllowMergeChanged);
    connect(
        m_combo_shelf_choice_heuristic,
        &QComboBox::currentIndexChanged,
        this,
        &SpritePackerWidget::onShelfBinSplitHeuristicChanged);
    connect(
        m_checkbox_shelf_use_waste_map,
        &QCheckBox::checkStateChanged,
        this,
        &SpritePackerWidget::onShelfBinUseWasteMapChanged);

    m_packers->max_rects_bin.setChoiceHeuristic(
        static_cast<MaxRectsBinAtlasPackerChoiceHeuristic>(m_combo_mrb_heuristic->currentData().toInt()));
    m_packers->max_rects_bin.allowFlip(m_checkbox_mrb_allow_flip->isChecked());
    m_packers->skyline_bin.setLevelChoiceHeuristic(
        static_cast<SkylineBinAtlasPackerLevelChoiceHeuristic>(m_combo_skyline_heuristic->currentData().toInt()));
    m_packers->skyline_bin.enableWasteMap(m_checkbox_skyline_use_waste_map->isChecked());
    m_packers->guillotine_bin.setChoiceHeuristic(
        static_cast<GuillotineBinAtlasPackerChoiceHeuristic>(m_combo_guillotine_choice_heuristic->currentData().toInt()));
    m_packers->guillotine_bin.setSplitHeuristic(
        static_cast<GuillotineBinAtlasPackerSplitHeuristic>(m_combo_guillotine_split_heuristic->currentData().toInt()));
    m_packers->guillotine_bin.enableMerge(m_checkbox_guillotine_allow_merge->isChecked());
    m_packers->shelf_bin.setChoiceHeuristic(
        static_cast<ShelfBinAtlasPackerChoiceHeuristic>(m_combo_shelf_choice_heuristic->currentData().toInt()));
    m_packers->shelf_bin.enableWasteMap(m_checkbox_shelf_use_waste_map->isChecked());

    onAlgorithmChanged();
    validateExportPackRequirements();
}

SpritePackerWidget::~SpritePackerWidget()
{
    delete m_packers;
}

void SpritePackerWidget::renderPack()
{
    if(m_widget_sprite_list->sprites().isEmpty())
        return;
    m_thread->start([this](QPromise<void> & __promise) {
        QList<Sprite> sprites_snapshot = m_widget_sprite_list->sprites();
        m_atlases = m_packers->current->pack(
            __promise,
            sprites_snapshot,
            {
                .max_atlas_size = QSize(
                    m_spin_max_width->value(),
                    m_spin_max_height->value()
                    ),
                .detect_duplicates = m_checkbox_detect_duplicates->isChecked(),
                .crop = m_checkbox_crop->isChecked(),
                .remove_file_extensions = m_checkbox_remove_file_ext->isChecked()
            });
    });
}

void SpritePackerWidget::onRenderPackFinished()
{
    m_preview->scene()->clear();
    if(m_atlases == nullptr)
        return;
    const qreal y_gap = 100.0;
    qreal max_width = .0;
    qreal y_offset = .0;
    for(const RawAtlas & atlas : *m_atlases)
    {
        TransparentGraphicsPixmapItem * item = new TransparentGraphicsPixmapItem(QPixmap::fromImage(atlas.image));
        m_preview->scene()->addItem(item);
        item->setPos(-atlas.image.width() / 2.0, y_offset);
        y_offset += y_gap + item->boundingRect().height();
        if(atlas.image.width() > max_width)
            max_width = atlas.image.width();
    }
    m_preview->scene()->setSceneRect(-max_width / 2.0, 0, max_width, y_offset);
    validateExportPackRequirements();
}

void SpritePackerWidget::onRenderPackError(const QString & _message)
{
    QMessageBox::critical(this, QString(), _message);
}

void SpritePackerWidget::onTextureWidthChanged()
{
    // Workaround: prevent duplicated events
    // https://forum.qt.io/topic/105335
    if(m_spin_max_width->value() != m_last_calulated_size.width())
    {
        m_last_calulated_size.setWidth(m_spin_max_width->value());
        renderPack();
    }
}

void SpritePackerWidget::onTextureHeightChanged()
{
    // Workaround: prevent duplicated events
    // https://forum.qt.io/topic/105335
    if(m_spin_max_height->value() != m_last_calulated_size.height())
    {
        m_last_calulated_size.setHeight(m_spin_max_height->value());
        renderPack();
    }
}

void SpritePackerWidget::exportPack()
{
    try
    {
        m_atlases->save(
            m_edit_export_directory->text(),
            m_edit_export_name->text(),
            m_combo_texture_format->currentText());
    }
    catch(const Exception & _exception)
    {
        QMessageBox::critical(this, QString(), _exception.message());
    }
}

void SpritePackerWidget::validateExportPackRequirements()
{
    bool valid =
        m_atlases != nullptr &&
        m_atlases->count() > 0 &&
        !m_edit_export_directory->text().isEmpty() &&
        !m_edit_export_name->text().isEmpty();
    m_btn_export->setEnabled(valid);
}

void SpritePackerWidget::browseForExportDir()
{
    QSettings settings;
    QString default_dir = m_edit_export_directory->text();
    if(default_dir.isEmpty())
        default_dir = settings.value(Settings::Output::atlas_directory, Settings::Input::sprite_directory).toString();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory to save atlases"), default_dir);
    if(!dir.isEmpty())
    {
        settings.setValue(Settings::Output::atlas_directory, dir);
        m_edit_export_directory->setText(dir);
    }
}

void SpritePackerWidget::onAlgorithmChanged()
{
    AtlasPacker * new_packer;
    switch(static_cast<PackAlgorithm>(m_combo_algorithm->currentData().toInt()))
    {
    case PackAlgorithm::MaxRectsBin:
        new_packer = &m_packers->max_rects_bin;
        break;
    case PackAlgorithm::SkylineBinPack:
        new_packer = &m_packers->skyline_bin;
        break;
    case PackAlgorithm::GuillotineBinPack:
        new_packer = &m_packers->guillotine_bin;
        break;
    case PackAlgorithm::ShelfBinPack:
        new_packer = &m_packers->shelf_bin;
        break;
    case PackAlgorithm::Meta:
        new_packer = &m_packers->meta;
        break;
    default:
        new_packer = m_packers->current;
        break;
    }
    if(m_packers->current != new_packer)
    {
        m_label_mrb_heuristic->setVisible(new_packer == &m_packers->max_rects_bin);
        m_combo_mrb_heuristic->setVisible(new_packer == &m_packers->max_rects_bin);
        m_combo_mrb_heuristic->setVisible(new_packer == &m_packers->max_rects_bin);
        m_checkbox_mrb_allow_flip->setVisible(new_packer == &m_packers->max_rects_bin);

        m_label_skyline_heuristic->setVisible(new_packer == &m_packers->skyline_bin);
        m_combo_skyline_heuristic->setVisible(new_packer == &m_packers->skyline_bin);
        m_checkbox_skyline_use_waste_map->setVisible(new_packer == &m_packers->skyline_bin);

        m_label_guillotine_choice_heuristic->setVisible(new_packer == &m_packers->guillotine_bin);
        m_combo_guillotine_choice_heuristic->setVisible(new_packer == &m_packers->guillotine_bin);
        m_label_guillotine_split_heuristic->setVisible(new_packer == &m_packers->guillotine_bin);
        m_combo_guillotine_split_heuristic->setVisible(new_packer == &m_packers->guillotine_bin);
        m_checkbox_guillotine_allow_merge->setVisible(new_packer == &m_packers->guillotine_bin);

        m_label_shelf_choice_heuristic->setVisible(new_packer == &m_packers->shelf_bin);
        m_combo_shelf_choice_heuristic->setVisible(new_packer == &m_packers->shelf_bin);
        m_checkbox_shelf_use_waste_map->setVisible(new_packer == &m_packers->shelf_bin);

        m_packers->current = new_packer;
        renderPack();
    }
}

void SpritePackerWidget::onMaxRectesBiAllowFlipChanged(Qt::CheckState _state)
{
    m_packers->max_rects_bin.allowFlip(_state == Qt::Checked);
    renderPack();
}

void SpritePackerWidget::onMaxRectesBinHeuristicChanged(int _index)
{
    int heuristic = m_combo_mrb_heuristic->itemData(_index).toInt();
    m_packers->max_rects_bin.setChoiceHeuristic(static_cast<MaxRectsBinAtlasPackerChoiceHeuristic>(heuristic));
    renderPack();
}

void SpritePackerWidget::onSkylineBinUseWasteMapChanged(Qt::CheckState _state)
{
    m_packers->skyline_bin.enableWasteMap(_state == Qt::Checked);
    renderPack();
}

void SpritePackerWidget::onSkylineBinHeuristicChanged(int _index)
{
    int heuristic = m_combo_skyline_heuristic->itemData(_index).toInt();
    m_packers->skyline_bin.setLevelChoiceHeuristic(static_cast<SkylineBinAtlasPackerLevelChoiceHeuristic>(heuristic));
    renderPack();
}

void SpritePackerWidget::onGuillotineBinChoiceHeuristicChanged(int _index)
{
    int heuristic = m_combo_guillotine_choice_heuristic->itemData(_index).toInt();
    m_packers->guillotine_bin.setChoiceHeuristic(static_cast<GuillotineBinAtlasPackerChoiceHeuristic>(heuristic));
    renderPack();
}

void SpritePackerWidget::onGuillotineBinSplitHeuristicChanged(int _index)
{
    int heuristic = m_combo_guillotine_split_heuristic->itemData(_index).toInt();
    m_packers->guillotine_bin.setSplitHeuristic(static_cast<GuillotineBinAtlasPackerSplitHeuristic>(heuristic));
    renderPack();
}

void SpritePackerWidget::onGuillotineBinAllowMergeChanged(Qt::CheckState _state)
{
    m_packers->guillotine_bin.enableMerge(_state == Qt::Checked);
    renderPack();
}

void SpritePackerWidget::onShelfBinSplitHeuristicChanged(int _index)
{
    int heuristic = m_combo_shelf_choice_heuristic->itemData(_index).toInt();
    m_packers->shelf_bin.setChoiceHeuristic(static_cast<ShelfBinAtlasPackerChoiceHeuristic>(heuristic));
    renderPack();
}

void SpritePackerWidget::onShelfBinUseWasteMapChanged(Qt::CheckState _state)
{
    m_packers->shelf_bin.enableWasteMap(_state == Qt::Checked);
    renderPack();
}
