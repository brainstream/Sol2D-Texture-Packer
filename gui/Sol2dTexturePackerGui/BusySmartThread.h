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

#include <QTimer>
#include <Sol2dTexturePackerGui/LambdaThread.h>
#include <Sol2dTexturePackerGui/BusyDialog.h>

class BusySmartThread : public QObject
{
    Q_OBJECT

public:
    BusySmartThread(std::function<void()> _lambda, QWidget * _parent_widget);
    ~BusySmartThread() override;
    void setSpinnerDisplayTimeout(uint32_t _timeout_ms);
    void start();

signals:
    void exception(QString _message);
    void finished();

private:
    void showBusyDialog();
    void destroyBusyDialog();
    void finish();

private:
    uint32_t m_spinner_display_timeout;
    QWidget * m_parent_widget;
    BusyDialog * m_dialog;
    LambdaThread * m_thread;
    QTimer * m_timer;
};
