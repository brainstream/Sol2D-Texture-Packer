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

#include <Sol2dTexturePackerCli/NopApplication.h>
#include <Sol2dTexturePackerCli/PackApplication.h>
#include <Sol2dTexturePackerCli/UnpackApplication.h>
#include <Sol2dTexturePackerCli/ExitCodes.h>
#include <LibSol2dTexturePacker/Splitters/GridSplitter.h>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QIODevice>
#include <memory>
#include <cstdio>

namespace {

QTextStream & operator << (QTextStream & _stream, const QList<QCommandLineOption> & _options)
{
    struct Option
    {
        QString names;
        QString description;
    };
    QList<Option> opts;
    opts.reserve(_options.length());
    qsizetype max_name_len = 0;
    foreach(const QCommandLineOption & o, _options)
    {
        Option opt;
        opt.description = o.description();
        foreach(const QString & name, o.names())
        {
            QString n = name.length() == 1 ? QString("-") + name : QString("--") + name;
            if(opt.names.isEmpty())
                opt.names = n;
            else
                opt.names += QString(", ") + n;
        }
        if(!o.valueName().isEmpty())
            opt.names += QString(" <%1>").arg(o.valueName());
        if(opt.names.length() > max_name_len)
            max_name_len = opt.names.length();
        opts.append(opt);
    }
    for(qsizetype i = 0; i < opts.length(); ++i)
    {
        qsizetype spaces = max_name_len - opts[i].names.length() + 2;
        _stream << " " << opts[i].names << QString(" ").repeated(spaces) << opts[i].description;
        if((opts.length() - i) > 1)
            _stream << Qt::endl;
    }
    return _stream;
}

class AppRunner final
{
public:
    static std::unique_ptr<Application> run(IO & _io, const QStringList & _args)
    {
        AppRunner runner(_io, _args);
        return runner.run();
    }

private:
    AppRunner(IO & _io, const QStringList & _args) :
        m_io(_io),
        m_args(_args)
    {
    }

    std::unique_ptr<Application> run() const;
    std::unique_ptr<Application> parseGlobal() const;
    std::unique_ptr<Application> parsePack() const;
    std::unique_ptr<Application> parseUnpack() const;
    static bool tryParseGridOptions(const QString & _str, UnpackApplication::GridOptions & _options);

private:
    IO & m_io;
    const QStringList & m_args;
};

std::unique_ptr<Application> AppRunner::run() const
{
    if(m_args.length() > 1)
    {
        if(m_args[1] == "pack")
            return parsePack();
        else if(m_args[1] == "unpack")
            return parseUnpack();
    }
    return parseGlobal();
}

std::unique_ptr<Application> AppRunner::parseGlobal() const
{
    QCommandLineParser parser;
    QList<QCommandLineOption> options
    {
        {
            { "h", "help" },
            QObject::tr("Displays help")
        },
        {
            { "v", "version" },
            QObject::tr("Displays version")
        }
    };
    parser.addOptions(options);
    parser.process(m_args);

    int exit_code = ExitCodes::Success;

    if(parser.isSet("version"))
    {
        m_io.out << __S2TP_VERSION << Qt::endl;
    }
    else
    {
        if(!parser.isSet("help"))
        {
            m_io.err << QObject::tr("Command not specified") << Qt::endl;
            exit_code = ExitCodes::RequiredArgumentNotSpecified;
        }
        m_io.out <<
            QObject::tr("Usage:") << " " << m_args[0] << " " <<
            QObject::tr("[global options] [command] [command args]") << Qt::endl <<
            Qt::endl <<
            QObject::tr("Global options:") << Qt::endl <<
            options << Qt::endl <<
            Qt::endl <<
            QObject::tr("Commands:") << Qt::endl <<
            " pack    " << QObject::tr("Packs textures into an atlas") << Qt::endl <<
            " unpack  " << QObject::tr("Unpacks textures from the atlas") << Qt::endl;
    }

    return std::unique_ptr<Application>(new NopApplication(m_io, exit_code));
}

std::unique_ptr<Application> AppRunner::parsePack() const
{
    return std::unique_ptr<Application>(new PackApplication(m_io));
}

std::unique_ptr<Application> AppRunner::parseUnpack() const
{
    QCommandLineParser parser;
    const QString opt_name_help = "help";
    const QString opt_name_grid = "grid";
    const QString opt_name_atlas = "atlas";
    const QString opt_name_output = "output";
    QList<QCommandLineOption> options
    {
        {
            { "h", opt_name_help },
            QObject::tr("Displays unpacker help")
        },
        {
            { "g", opt_name_grid },
            QObject::tr("Unpacks using grid"),
            "grid description"
        },
        {
            { "a", opt_name_atlas },
            QObject::tr("Unpacks using atlas"),
            "file"
        },
        {
            { "o", opt_name_output },
            QObject::tr("Output directory"),
            "directory"
        }
    };
    QStringList args = m_args;
    args.removeAt(1);
    parser.addOptions(options);
    parser.process(args);

    if(parser.isSet(opt_name_help))
    {
        m_io.out <<
            QObject::tr("Usage:") << " " << m_args[0] << " " << m_args[1] << " " << QObject::tr("[options]") << Qt::endl <<
            Qt::endl <<
            QObject::tr("Options:") << Qt::endl <<
            options << Qt::endl
                 << Qt::endl <<
                QObject::tr("Grid description format:") << Qt::endl <<
                " t:texture.png;r:5;c:5;w:48;h:48;hs:2;vs:2;mt:12;ml:14" << Qt::endl <<
                QObject::tr(" where:") << Qt::endl <<
                " t              " << QObject::tr("Texture filename") << Qt::endl <<
                " r              " << QObject::tr("Row count") << Qt::endl <<
                " c              " << QObject::tr("Column count") << Qt::endl <<
                " w              " << QObject::tr("Sprite width") << Qt::endl <<
                " h              " << QObject::tr("Sprite height") << Qt::endl <<
                " hs (optional)  " << QObject::tr("Horizontal spacing") << Qt::endl <<
                " vs (optional)  " << QObject::tr("Vertical spacing") << Qt::endl <<
                " mt (optional)  " << QObject::tr("Margin top") << Qt::endl <<
                " ml (optional)  " << QObject::tr("Margin left") << Qt::endl;
        return std::unique_ptr<Application>(new NopApplication(m_io, ExitCodes::Success));
    }

    QString output =  parser.value(opt_name_output);
    if(output.isEmpty())
    {
        m_io.err << QObject::tr("Output directory not specified") << Qt::endl;
        return std::unique_ptr<Application>(new NopApplication(m_io, ExitCodes::RequiredArgumentNotSpecified));
    }
    if(parser.isSet(opt_name_atlas))
    {
        return std::unique_ptr<Application>(new UnpackApplication(m_io, parser.value(opt_name_grid), output));
    }
    if(parser.isSet(opt_name_grid))
    {
        UnpackApplication::GridOptions options = {};
        if(tryParseGridOptions(parser.value(opt_name_grid), options))
        {
            return std::unique_ptr<Application>(new UnpackApplication(m_io, options, output));
        }
        else
        {
            m_io.err << QObject::tr("Invalid grid description") << Qt::endl;
            return std::unique_ptr<Application>(new NopApplication(m_io, ExitCodes::InvalidDataFormat));
        }
    }
    m_io.err << QObject::tr("Neither the atlas nor the grid are specified") << Qt::endl;
    return std::unique_ptr<Application>(new NopApplication(m_io, ExitCodes::RequiredArgumentNotSpecified));
}

bool AppRunner::tryParseGridOptions(const QString & _str, UnpackApplication::GridOptions & _options)
{
    GridSplitterOptions opts;
    QString texture;
    QStringList items = _str.split(';', Qt::SkipEmptyParts);
    foreach(const QString & item, items)
    {
        QStringList kv = item.split(':', Qt::SkipEmptyParts);
        if(kv.length() != 2) return false;
        if(kv[0] == "t") texture = kv[1];
        else if(kv[0] == "r") opts.row_count = kv[1].toInt();
        else if(kv[0] == "c") opts.column_count = kv[1].toInt();
        else if(kv[0] == "w") opts.sprite_width = kv[1].toInt();
        else if(kv[0] == "h") opts.sprite_height = kv[1].toInt();
        else if(kv[0] == "hs") opts.horizontal_spacing = kv[1].toInt();
        else if(kv[0] == "vs") opts.vertical_spacing = kv[1].toInt();
        else if(kv[0] == "mt") opts.margin_top = kv[1].toInt();
        else if(kv[0] == "ml") opts.margin_left = kv[1].toInt();
        else return false;
    }
    if(
        texture.isEmpty() ||
        opts.row_count <= 0 ||
        opts.column_count <= 0 ||
        opts.sprite_height <= 0 ||
        opts.sprite_height <= 0 ||
        opts.horizontal_spacing < 0 ||
        opts.vertical_spacing < 0 ||
        opts.margin_top < 0 ||
        opts.margin_left < 0)
    {
        return false;
    }
    _options.texture = texture;
    _options.splitter_options = opts;
    return true;
}

} // namespace name

int main(int _argc, char * _argv[])
{
    QCoreApplication app(_argc, _argv);
    app.setApplicationName(__S2TP_BIN);
    app.setApplicationVersion(__S2TP_VERSION);
    app.setOrganizationName(__S2TP_ORG);
    IO io
    {
        .out = QTextStream(stdout, QIODevice::WriteOnly),
        .err = QTextStream(stderr, QIODevice::WriteOnly)
    };
    return AppRunner::run(io, app.arguments())->exec();
}
