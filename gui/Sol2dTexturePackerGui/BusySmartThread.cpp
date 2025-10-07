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

BusySmartThread::BusySmartThread(std::function<void()> _lambda, QWidget * _parent_widget) :
    QObject(_parent_widget),
    m_spinner_display_timeout(400),
    m_parent_widget(_parent_widget),
    m_dialog(nullptr)
{
    m_thread = new LambdaThread(_lambda, this);
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_thread, &LambdaThread::exception, this, &BusySmartThread::exception);
    connect(m_thread, &LambdaThread::finished, this, &BusySmartThread::finish);
    connect(m_timer, &QTimer::timeout, this, &BusySmartThread::showBusyDialog);
}

BusySmartThread::~BusySmartThread()
{
    destroyBusyDialog();
}

void BusySmartThread::destroyBusyDialog()
{
    if(m_dialog)
    {
        m_dialog->hide();
        delete m_dialog;
        m_dialog = nullptr;
    }
}

void BusySmartThread::setSpinnerDisplayTimeout(uint32_t _timeout_ms)
{
    m_spinner_display_timeout = _timeout_ms;
}

void BusySmartThread::start()
{
    m_timer->setInterval(m_spinner_display_timeout);
    m_timer->start();
    m_thread->start();
}

void BusySmartThread::finish()
{
    m_timer->stop();
    destroyBusyDialog();
    emit finished();
}

void BusySmartThread::showBusyDialog()
{
    if(m_dialog)
        return;
    m_dialog = new BusyDialog(m_parent_widget);
    m_dialog->show();
}
