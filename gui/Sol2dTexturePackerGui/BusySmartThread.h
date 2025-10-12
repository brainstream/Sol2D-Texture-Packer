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
#include <QFuture>

class BusySmartThread : public QObject
{
    Q_OBJECT

public:
    explicit BusySmartThread(QWidget * _parent_widget);
    void setSpinnerDisplayTimeout(uint32_t _timeout_ms);
    void start(std::function<void(QPromise<void> &)> _lambda);

signals:
    void failed(QString _message);
    void success();
    void complete();

private:
    uint32_t m_spinner_display_timeout;
    QWidget * m_parent_widget;
    QTimer * m_timer;
    QFuture<void> m_future;
};
