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

#include <Sol2dTexturePackerGui/LambdaThread.h>

LambdaThread::LambdaThread(std::function<void()> _lambda, QObject * _parent) :
    QThread(_parent),
    m_lambda(_lambda)
{
    setObjectName("LambdaThread");
}

void LambdaThread::run()
{
    try
    {
        m_lambda();
    }
    catch(const Exception & ex)
    {
        emit exception(ex.message());
    }
    catch(const std::exception & err)
    {
        emit exception(QString::fromStdString(err.what()));
    }
    catch(...)
    {
        emit exception(tr("An unknown error has occurred"));
    }
}
