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

#include <QWidget>
#include <LibSol2dTexturePacker/Pack/GridPack.h>
#include "ui_GridPropertyEditorWidget.h"

class GridPropertyEditorWidget : public QWidget, private Ui::GridPropertyEditorWidget
{
    Q_OBJECT

public:
    explicit GridPropertyEditorWidget(QWidget * _parent = nullptr);
    void setPack(GridPack * _pack);

private slots:
    void onRowCountChanged(int _value);
    void onColumnCountChanged(int _value);
    void onSpriteWidthChanged(int _value);
    void onSpriteHeightChanged(int _value);
    void onMarginLeftChanged(int _value);
    void onMarginTopChanged(int _value);
    void onHorizontalSpacingChanged(int _value);
    void onVerticalSpacingChanged(int _value);

private:
    GridPack * m_pack;
};
