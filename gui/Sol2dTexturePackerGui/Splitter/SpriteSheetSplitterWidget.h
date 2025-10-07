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

#include "ui_SpriteSheetSplitterWidget.h"
#include <LibSol2dTexturePacker/Pack/Pack.h>

class SpriteSheetSplitterWidget : public QWidget, private Ui::SpriteSheetSplitterWidget
{
    Q_OBJECT

public:
    explicit SpriteSheetSplitterWidget(QWidget * _parent = nullptr);

signals:
    void sheetLoaded(const QString & _filename);

private slots:
    void openTexture();
    void syncWithPack();
    void exportSprites();
    void exportToAtlas();
    void openAtlas();

private:
    void applyNewTexture();
    void setExportControlsEnabled(bool _enabled);

private:
    const QString m_open_image_dialog_filter;
    QString m_last_atlas_export_file;
    QPen m_sprite_pen;
    QBrush m_sprite_brush;
    QSharedPointer<Pack> m_pack;
};
