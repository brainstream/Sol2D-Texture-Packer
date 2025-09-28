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

#include <Sol2dTexturePackerGui/Splitter/GridPropertyEditorWidget.h>

GridPropertyEditorWidget::GridPropertyEditorWidget(QWidget * _parent) :
    QWidget(_parent),
    m_pack(nullptr)
{
    setupUi(this);
    connect(m_spin_rows, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onRowCountChanged);
    connect(m_spin_columns, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onColumnCountChanged);
    connect(m_spin_sprite_width, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onSpriteWidthChanged);
    connect(m_spin_sprite_height, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onSpriteHeightChanged);
    connect(m_spin_margin_left, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onMarginLeftChanged);
    connect(m_spin_margin_top, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onMarginTopChanged);
    connect(m_spin_hspacing, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onHorizontalSpacingChanged);
    connect(m_spin_vspacing, &QSpinBox::valueChanged, this, &GridPropertyEditorWidget::onVerticalSpacingChanged);
}

void GridPropertyEditorWidget::onRowCountChanged(int _value)
{
    if(m_pack) m_pack->setRowCount(_value);
}

void GridPropertyEditorWidget::onColumnCountChanged(int _value)
{
    if(m_pack) m_pack->setColumnCount(_value);
}

void GridPropertyEditorWidget::onSpriteWidthChanged(int _value)
{
    if(m_pack) m_pack->setSpriteWidth(_value);
}

void GridPropertyEditorWidget::onSpriteHeightChanged(int _value)
{
    if(m_pack) m_pack->setSpriteHeight(_value);
}

void GridPropertyEditorWidget::onMarginLeftChanged(int _value)
{
    if(m_pack) m_pack->setMarginLeft(_value);
}

void GridPropertyEditorWidget::onMarginTopChanged(int _value)
{
    if(m_pack) m_pack->setMarginTop(_value);
}

void GridPropertyEditorWidget::onHorizontalSpacingChanged(int _value)
{
    if(m_pack) m_pack->setHorizontalSpacing(_value);
}

void GridPropertyEditorWidget::onVerticalSpacingChanged(int _value)
{
    if(m_pack) m_pack->setVerticalSpacing(_value);
}

void GridPropertyEditorWidget::setPack(GridPack * _pack)
{
    m_pack = _pack;
    if(m_pack)
    {
        const GridOptions & options = m_pack ? m_pack->options() : GridOptions {};
        m_edit_texture_size->setText(QString("%1x%2").arg(m_pack->texture().width()).arg(m_pack->texture().height()));
        m_edit_texture_file->setText(m_pack->textureFilename());
        m_spin_rows->setValue(options.row_count);
        m_spin_columns->setValue(options.column_count);
        m_spin_sprite_width->setValue(options.sprite_width);
        m_spin_sprite_height->setValue(options.sprite_height);
        m_spin_vspacing->setValue(options.vertical_spacing);
        m_spin_hspacing->setValue(options.horizontal_spacing);
        m_spin_margin_left->setValue(options.margin_left);
        m_spin_margin_top->setValue(options.margin_top);
    }
    else
    {
        m_edit_texture_size->clear();
        m_edit_texture_file->clear();
        m_spin_rows->clear();
        m_spin_columns->clear();
        m_spin_sprite_width->clear();
        m_spin_sprite_height->clear();
        m_spin_vspacing->clear();
        m_spin_hspacing->clear();
        m_spin_margin_left->clear();
        m_spin_margin_top->clear();
    }
}
