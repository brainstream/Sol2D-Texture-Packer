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
#include <Sol2dTexturePackerGui/BusySmartThread.h>
#include <LibSol2dTexturePacker/Packers/RawAtlasPack.h>
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

signals:
    void packNameChanged(const QString & _name);

private slots:
    void renderPack();
    void onRenderPackFinished();
    void onRenderPackError(const QString & _message);
    void exportPack();
    void browseForExportDir();
    void validateExportPackRequirements();
    void onColorToAlphaToggle();
    void pickColorToAlpha();
    void onTextureWidthChanged();
    void onTextureHeightChanged();
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
    Packers * m_packers;
    std::unique_ptr<RawAtlasPack> m_atlases;
    QSize m_last_calulated_size;
    BusySmartThread * m_thread;
};
