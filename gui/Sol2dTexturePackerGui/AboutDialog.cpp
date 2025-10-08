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

#include <Sol2dTexturePackerGui/AboutDialog.h>

AboutDialog::AboutDialog(QWidget * _parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    setupUi(this);
    m_label_title->setText(__S2TP_APP);
    static const int start_development_year = 2025;
    int build_year = QString(__DATE__).right(4).toInt();
    QString years = (start_development_year < build_year) ?
        QString("%1 - %2").arg(start_development_year).arg(build_year) :
        QString::number(build_year);
    m_label_version->setText(m_label_version->text().arg(__S2TP_VERSION));
    m_label_description->setText(m_label_description->text().arg(years));
    adjustSize();
}
