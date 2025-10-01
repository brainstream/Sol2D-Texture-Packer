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
#include <Sol2dTexturePackerGui/Settings.h>
#include <LibSol2dTexturePacker/Packers/MaxRectsBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/SkylineBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/GuillotineBinAtlaskPacker.h>
#include <LibSol2dTexturePacker/Packers/ShelfBinAtlasPacker.h>
#include <QList>
#include <QFileDialog>
#include <QPixmap>
#include <QAbstractListModel>

namespace {

enum class PackAlgorithm
{
    MaxRectsBin,
    SkylineBinPack,
    GuillotineBinPack,
    ShelfBinPack
};

} // namespace name

struct SpritePackerWidget::Packers
{
    MaxRectsBinAtlasPacker max_rects_bin;
    SkylineBinAtlasPacker skyline_bin;
    GuillotineBinAtlaskPacker guillotine_bin;
    ShelfBinAtlasPacker shelf_bin;
    AtlasPacker * current;
};

class SpritePackerWidget::SpriteListModel : public QAbstractListModel
{
public:
    explicit SpriteListModel(QObject * _parent);
    int rowCount(const QModelIndex & _parent) const override;
    QVariant data(const QModelIndex & _index, int _role) const override;
    void addSprite(const Sprite & _sprite);
    const QList<Sprite> & getSprites() const;

private:
    QList<Sprite> m_sprites;
};

SpritePackerWidget::SpriteListModel::SpriteListModel(QObject * _parent) :
    QAbstractListModel(_parent)
{
}

int SpritePackerWidget::SpriteListModel::rowCount(const QModelIndex & _parent) const
{
    if(_parent.isValid())
        return 0;
    return static_cast<int>(m_sprites.count());
}

QVariant SpritePackerWidget::SpriteListModel::data(const QModelIndex & _index, int _role) const
{
    switch(_role)
    {
    case Qt::DisplayRole:
        return m_sprites[_index.row()].name;
    case Qt::DecorationRole:
        return m_sprites[_index.row()].pixmap;
    default:
        return QVariant();
    }
}

void SpritePackerWidget::SpriteListModel::addSprite(const Sprite & _sprite)
{
    beginInsertRows(QModelIndex(), m_sprites.count(), m_sprites.count() + 1);
    m_sprites.append(_sprite);
    endInsertRows();
}

inline const QList<Sprite> & SpritePackerWidget::SpriteListModel::getSprites() const
{
    return m_sprites;
}

SpritePackerWidget::SpritePackerWidget(QWidget * _parent) :
    QWidget(_parent),
    m_packers(new Packers)
{
    setupUi(this);

    m_packers->current = nullptr;
    m_packers->max_rects_bin.setChoiceHeuristic(MaxRectsBinAtlasPackerChoiceHeuristic::BestAreaFit);

    m_sprites_model = new SpriteListModel(m_tree_sprites);
    m_tree_sprites->setModel(m_sprites_model);
    m_preview->setScene(new QGraphicsScene(m_preview));
    m_preview->setZoomModel(&m_zoom_widget->model());
    m_splitter->setSizes({100, 500});

    m_combo_algorithm->addItem("Max Rects", static_cast<int>(PackAlgorithm::MaxRectsBin));
    m_combo_algorithm->addItem("Skyline", static_cast<int>(PackAlgorithm::SkylineBinPack));
    m_combo_algorithm->addItem("Guillotine", static_cast<int>(PackAlgorithm::GuillotineBinPack));
    m_combo_algorithm->addItem("Shelf", static_cast<int>(PackAlgorithm::ShelfBinPack));

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
        tr("ShorterA xis"),
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

    connect(m_btn_add_sprites, &QPushButton::clicked, this, &SpritePackerWidget::addSprites);
    connect(m_spin_max_width, &QSpinBox::valueChanged, this, &SpritePackerWidget::renderPack);
    connect(m_spin_max_height, &QSpinBox::valueChanged, this, &SpritePackerWidget::renderPack);
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
}

SpritePackerWidget::~SpritePackerWidget()
{
    delete m_packers;
}

void SpritePackerWidget::addSprites()
{
    QSettings settings;
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        QString(),
        settings.value(gc_settings_key_pack_input_dir).toString()
    );
    if(files.isEmpty())
        return;
    settings.setValue(
        gc_settings_key_pack_input_dir,
        std::filesystem::absolute(std::filesystem::path(files[0].toStdString()).parent_path()).c_str()
    );
    foreach(const QString & file, files) {
        QPixmap pixmap;
        if(!pixmap.load(file)) continue;
        QFileInfo fi(file);
        m_sprites_model->addSprite(
            Sprite
            {
                .path = fi.absoluteFilePath(),
                .name = fi.fileName(),
                .pixmap = pixmap
            }
        );
    }
    renderPack();
}

void SpritePackerWidget::renderPack()
{
    m_preview->scene()->clear();
    AtlasPackerOptions options
    {
        .max_atlas_size = QSize(
            m_spin_max_width->value(),
            m_spin_max_height->value()
        ),
        .detect_duplicates = m_checkbox_detect_duplicates->isChecked(),
        .crop = m_checkbox_crop->isChecked()
    };
    QList<QPixmap> atlases = m_packers->current->pack(m_sprites_model->getSprites(), options);
    foreach(const QPixmap & atlas, atlases)
        m_preview->scene()->addPixmap(atlas);
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
