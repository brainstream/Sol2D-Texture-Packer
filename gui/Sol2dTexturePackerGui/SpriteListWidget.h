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

#include <LibSol2dTexturePacker/Sprite.h>
#include "ui_SpriteListWidget.h"
#include <QMenu>

class SpriteListWidget : public QWidget, private Ui::SpriteListWidget
{
    Q_OBJECT

private:
    class SpriteListModel;

public:
    explicit SpriteListWidget(QWidget * _parent = nullptr);
    const QList<Sprite> & getSprites() const;

signals:
    void spriteListChanged();

private slots:
    void addSprites();
    void removeSprites();
    void showTreeItemContextMentu(const QPoint & _pos);

private:
    SpriteListModel * m_sprites_model;
    QMenu * m_tree_item_context_menu;
    const QString m_open_image_dialog_filter;
};
