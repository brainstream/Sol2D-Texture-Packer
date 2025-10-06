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

#include <LibSol2dTexturePacker/Def.h>
#include <QString>
#include <QObject>

class S2TP_EXPORT Exception
{
public:
    explicit Exception(QString _message) :
        m_message(std::move(_message))
    {
    }

    const QString & message() const
    {
        return m_message;
    }

private:
    const QString m_message;
};

class S2TP_EXPORT InvalidOperationExeption : public Exception
{
public:
    explicit InvalidOperationExeption(QString _message) :
        Exception(std::move(_message))
    {
    }
};

class S2TP_EXPORT IOExeption : public Exception
{
public:
    explicit IOExeption(QString _message) :
        Exception(std::move(_message))
    {
    }
};

class S2TP_EXPORT FileOpenException : public IOExeption
{
public:
    enum Mode
    {
        Read,
        Write
    };

    FileOpenException(const QString & _filename, Mode _mode) :
        IOExeption(getMessage(_filename, _mode))
    {
    }

private:
    static QString getMessage(const QString & _filename, Mode _mode)
    {
        switch(_mode)
        {
        case Read:
            return QObject::tr("Unable to open file \"%1\" for reading").arg(_filename);
        case Write:
            return QObject::tr("Unable to open file \"%1\" for writing").arg(_filename);
        default:
            return {};
        }
    }
};

class S2TP_EXPORT InvalidXmlException : public Exception
{
public:
    explicit InvalidXmlException(const QString & _filename) :
        Exception(QObject::tr("File \"%1\" is not valid XML").arg(_filename))
    {
    }
};

class InvalidFileFormatException : public Exception
{
public:
    explicit InvalidFileFormatException(const QString & _filename) :
        Exception(getMessage(_filename))
    {
    }

    InvalidFileFormatException(const QString & _filename, const QString & _additional_message) :
        Exception(getMessage(_filename, _additional_message))
    {
    }

private:
    static QString getMessage(const QString & _filename, const QString & _additional_message = QString())
    {
        QString message = QObject::tr("File \"%1\" has invalid format").arg(_filename);
        return _additional_message.isEmpty() ? message : QString("%1.\n%2").arg(message, _additional_message);
    }
};

class S2TP_EXPORT ImageLoadingException : public IOExeption
{
public:
    explicit ImageLoadingException(const QString & _filename) :
        IOExeption(getMessage(_filename))
    {
    }

private:
    static QString getMessage(const QString & _filename)
    {
        return QObject::tr("Unable to load image from file \"%1\"").arg(_filename);
    }
};

class S2TP_EXPORT ImageSavingException : public IOExeption
{
public:
    explicit ImageSavingException(const QString & _filename) :
        IOExeption(getMessage(_filename))
    {
    }

private:
    static QString getMessage(const QString & _filename)
    {
        return QObject::tr("Unable to save image to file \"%1\"").arg(_filename);
    }
};
