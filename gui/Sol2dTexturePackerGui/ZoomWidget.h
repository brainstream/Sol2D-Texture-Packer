/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Open Sprite Sheet Tools.                                                      *
 *                                                                                                        *
 * Open Sprite Sheet Tools is free software: you can redistribute it and/or modify it under  the terms of *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Open Sprite Sheet Tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#pragma once

#include "ui_ZoomWidget.h"
#include <Sol2dTexturePackerGui/ZoomModel.h>

class ZoomWidget : public QWidget, private Ui::ZoomWidget
{
    Q_OBJECT

public:
    explicit ZoomWidget(QWidget * _parent = nullptr);
    const ZoomModel & model() const { return *m_model; }
    ZoomModel & model() { return *m_model; }

private slots:
    void onComboboxTextChanged(const QString & _text);
    void onSliderValueCahnged(int _value);
    void applyZoomValue(quint32 _zoom);

private:
    ZoomModel * m_model;
};
