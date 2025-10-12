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

#include <Sol2dTexturePackerGui/BusySmartThread.h>
#include <Sol2dTexturePackerGui/BusyDialog.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QtConcurrentRun>

BusySmartThread::BusySmartThread(QWidget * _parent_widget) :
    QObject(_parent_widget),
    m_spinner_display_timeout(400),
    m_parent_widget(_parent_widget)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
}

void BusySmartThread::setSpinnerDisplayTimeout(uint32_t _timeout_ms)
{
    m_spinner_display_timeout = _timeout_ms;
}

void BusySmartThread::start(std::function<void()> _lambda)
{
    BusyDialog * dialog = new BusyDialog(m_parent_widget);
    connect(m_timer, &QTimer::timeout, dialog, &BusyDialog::show);
    m_timer->setInterval(m_spinner_display_timeout);
    m_timer->start();
    QFuture future = QtConcurrent::run([_lambda, dialog, this]() {
        try
        {
            _lambda();
            emit success();
        }
        catch(const Exception & error)
        {
            emit failed(error.message());
        }
        catch(...)
        {
            emit failed(tr("An unknown error has occurred"));
        }
        dialog->deleteLater();
        m_timer->stop();
        emit complete();
    });
}
