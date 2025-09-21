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
#include <filesystem>
#include <utility>

class S2TP_EXPORT Exception : public std::exception
{
public:
    explicit Exception(QString _message) :
        m_message(std::move(_message))
    {
    }

    [[nodiscard]] const QString & message() const
    {
        return m_message;
    }

    [[nodiscard]] const char * what() const noexcept override
    {
        return m_message.toStdString().c_str();
    }

private:
    const QString m_message;
};

class S2TP_EXPORT FileOpenException : public Exception
{
public:
    enum Mode
    {
        Read,
        Write
    };

    FileOpenException(const QString & _filename, Mode _mode) :
        Exception(getMessage(_filename, _mode))
    {
    }

    FileOpenException(const std::filesystem::path & _filename, Mode _mode) :
        Exception(getMessage(_filename.string(), _mode))
    {
    }

private:
    template<typename Path>
    static QString getMessage(const Path & _filename, Mode _mode)
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
    explicit InvalidXmlException(const std::filesystem::path & _filename) :
        Exception(QObject::tr("File \"%1\" is not valid XML").arg(_filename.string()))
    {
    }
};

class InvalidFileFormatException : public Exception
{
public:
    explicit InvalidFileFormatException(const std::filesystem::path & _filename) :
        Exception(getMessage(_filename))
    {
    }

    InvalidFileFormatException(const std::filesystem::path & _filename, const QString & _additional_message) :
        Exception(getMessage(_filename, _additional_message))
    {
    }

private:
    static QString getMessage(const std::filesystem::path & _filename, const QString & _additional_message = QString())
    {
        QString message = QObject::tr("File \"%1\" has invalid format").arg(_filename.string());
        return _additional_message.isEmpty() ? message : QString("%1.\n%2").arg(message, _additional_message);
    }
};

class S2TP_EXPORT ImageLoadingException : public Exception
{
public:
    explicit ImageLoadingException(const std::filesystem::path & _filename) :
        Exception(getMessage(_filename.string()))
    {
    }

    explicit ImageLoadingException(const QString & _filename) :
        Exception(getMessage(_filename))
    {
    }

private:
    template<typename TString>
    static QString getMessage(const TString & _filename)
    {
        return QObject::tr("Unable to load image from file \"%1\"").arg(_filename);
    }
};
