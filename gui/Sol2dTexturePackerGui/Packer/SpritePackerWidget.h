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

#pragma once

#include "ui_SpritePackerWidget.h"
#include <LibSol2dTexturePacker/Packers/RawAtlasPack.h>
#include <QMenu>
#include <memory>

class SpritePackerWidget : public QWidget, private Ui::SpritePackerWidget
{
    Q_OBJECT

private:
    struct Packers;
    class SpriteListModel;

public:
    explicit SpritePackerWidget(QWidget * _parent = nullptr);
    ~SpritePackerWidget() override;

private slots:
    void showTreeItemContextMentu(const QPoint & _pos);
    void addSprites();
    void removeSprites();
    void renderPack();
    void exportPack();
    void browseForExportDir();
    void validateExportPackRequirements();
    void onAlgorithmChanged();
    void onMaxRectesBiAllowFlipChanged(Qt::CheckState _state);
    void onMaxRectesBinHeuristicChanged(int _index);
    void onSkylineBinUseWasteMapChanged(Qt::CheckState _state);
    void onSkylineBinHeuristicChanged(int _index);
    void onGuillotineBinChoiceHeuristicChanged(int _index);
    void onGuillotineBinSplitHeuristicChanged(int _index);
    void onGuillotineBinAllowMergeChanged(Qt::CheckState _state);
    void onShelfBinSplitHeuristicChanged(int _index);
    void onShelfBinUseWasteMapChanged(Qt::CheckState _state);

private:
    SpriteListModel * m_sprites_model;
    QMenu * m_tree_item_context_menu;
    Packers * m_packers;
    const QString m_open_image_dialog_filter;
    std::unique_ptr<RawAtlasPack> m_atlases;
};
