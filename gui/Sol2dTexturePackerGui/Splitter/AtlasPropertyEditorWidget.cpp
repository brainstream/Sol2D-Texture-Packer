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

#include <Sol2dTexturePackerGui/Splitter/AtlasPropertyEditorWidget.h>

AtlasPropertyEditorWidget::AtlasPropertyEditorWidget(QWidget * _parent) :
    QWidget(_parent),
    m_pack(nullptr)
{
    setupUi(this);
}

void AtlasPropertyEditorWidget::unsetPack()
{
    setPack(QString(), nullptr);
}

void AtlasPropertyEditorWidget::setPack(const QString & _data_file, AtlasPack * _pack)
{
    m_pack = _pack;
    if(m_pack)
    {
        m_edit_texture_size->setText(QString("%1x%2").arg(m_pack->texture().width()).arg(m_pack->texture().height()));
        m_edit_texture_file->setText(m_pack->textureFilename());
        m_edit_data_file->setText(_data_file);
    }
    else
    {
        m_edit_texture_size->clear();
        m_edit_texture_file->clear();
        m_combo_format->clear();
        m_edit_data_file->clear();
    }
}
