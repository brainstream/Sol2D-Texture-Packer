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

#include <Sol2dTexturePackerCli/NoopApplication.h>
#include <Sol2dTexturePackerCli/PackApplication.h>
#include <Sol2dTexturePackerCli/UnpackApplication.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QCommandLineParser>
#include <QIODevice>
#include <QTextStream>
#include <memory>

namespace {

struct IO
{
    QTextStream out;
    QTextStream err;
};

struct ExitCodes
{
    enum Codes : int
    {
        Success = 0,
        Error = 1,
        RequiredArgumentNotSpecified = 10,
        InvalidArgumentValue = 11
    };
};

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
        m_args(_args),
        m_help_options({{ "h", "help" }, QObject::tr("Displays help")})
    {
    }

    std::unique_ptr<Application> run() const;
    std::unique_ptr<Application> noop(int _exit_code) const;
    std::unique_ptr<Application> noRequiredArgument(const QCommandLineOption & _option) const;
    std::unique_ptr<Application> invalidArgument(const QCommandLineOption & _option) const;
    std::unique_ptr<Application> parseGlobal() const;
    std::unique_ptr<Application> parsePack() const;
    std::unique_ptr<Application> parseUnpack() const;
    std::unique_ptr<Application> parseUnpackGrid() const;
    std::unique_ptr<Application> parseUnpackAtlas() const;

private:
    IO & m_io;
    const QStringList & m_args;
    const QCommandLineOption m_help_options;
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
    const QCommandLineOption version_option = {
        { "v", "version" },
        QObject::tr("Displays version")
    };
    const QList options
    {
        m_help_options,
        version_option
    };
    parser.addOptions(options);
    parser.process(m_args);

    int exit_code = ExitCodes::Success;

    if(parser.isSet(version_option.names().first()))
    {
        m_io.out << __S2TP_VERSION << Qt::endl;
    }
    else
    {
        if(!parser.isSet(m_help_options.names().first()))
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
            " pack    " << QObject::tr("Packs sprites into an atlas") << Qt::endl <<
            " unpack  " << QObject::tr("Unpacks a texture") << Qt::endl;
    }

    return noop(exit_code);
}

std::unique_ptr<Application> AppRunner::noop(int _exit_code) const
{
    return std::unique_ptr<Application>(new NoopApplication(_exit_code));
}

std::unique_ptr<Application> AppRunner::noRequiredArgument(const QCommandLineOption & _option) const
{
    m_io.err <<
        QObject::tr("Required argument not specified or has invalid value: ") <<
        _option.description() <<
        Qt::endl;
    return noop(ExitCodes::RequiredArgumentNotSpecified);
}

std::unique_ptr<Application> AppRunner::invalidArgument(const QCommandLineOption & _option) const
{
    m_io.err <<
        QObject::tr("Argument has invalid value: ") <<
        _option.description() <<
        Qt::endl;
    return noop(ExitCodes::InvalidArgumentValue);
}

std::unique_ptr<Application> AppRunner::parsePack() const
{
    return std::unique_ptr<Application>(new PackApplication);
}

std::unique_ptr<Application> AppRunner::parseUnpack() const
{
    // 1st argument is app name
    // 2nd argument is "unpack"
    if(m_args.length() > 2)
    {
        if(m_args[2] == "grid")
            return parseUnpackGrid();
        if(m_args[2] == "atlas")
            return parseUnpackAtlas();
    }

    QStringList args = m_args;
    args.removeAt(1); // remove the "unpack" argument to hide it from the parser

    QCommandLineParser parser;
    const QList options { m_help_options };
    parser.addOptions(options);
    parser.process(args);

    int exit_code = ExitCodes::Success;

    if(!parser.isSet("help"))
    {
        m_io.err << QObject::tr("Method not specified") << Qt::endl;
        exit_code = ExitCodes::RequiredArgumentNotSpecified;
    }

    m_io.out <<
        QObject::tr("Usage:") << " " << m_args[0] << " " << m_args[1] << " " <<
            QObject::tr("[options] [method] [method options]") << Qt::endl <<
        Qt::endl <<
        QObject::tr("Options:") << Qt::endl <<
        options << Qt::endl <<
        Qt::endl <<
        QObject::tr("Methods:") << Qt::endl <<
        " grid   " << QObject::tr("Unpacks a grid-aligned texture") << Qt::endl <<
        " atlas  " << QObject::tr("Unpacks a texture using an atlas") << Qt::endl;

    return noop(exit_code);
}

std::unique_ptr<Application> AppRunner::parseUnpackGrid() const
{
    QStringList args = m_args;
    args.removeAt(1); // remove the "unpack" argument to hide it from the parser
    args.removeAt(1); // remove the "grid" argument to hide it from the parser

    const QCommandLineOption texture_option {
        QStringList { "tx", "texture" },
        QObject::tr("Texture filename"),
        QObject::tr("filename")
    };
    const QCommandLineOption output_option {
        QStringList { "out" },
        QObject::tr("Output directory"),
        QObject::tr("directory")
    };
    const QCommandLineOption rows_option {
        QStringList { "rows" },
        QObject::tr("Row count"),
        QObject::tr("value")
    };
    const QCommandLineOption columns_option {
        QStringList { "cols", "columns" },
        QObject::tr("Column count"),
        QObject::tr("value")
    };
    const QCommandLineOption sprite_width_option {
        QStringList { "wd", "sprite-width" },
        QObject::tr("Sprite width"),
        QObject::tr("value")
    };
    const QCommandLineOption sprite_height_option {
        QStringList { "ht", "sprite-height" },
        QObject::tr("Sprite height"),
        QObject::tr("value")
    };
    const QCommandLineOption horizontal_spacing_option {
        QStringList { "hs", "horizontal-spacing" },
        QObject::tr("Horizontal spacing"),
        QObject::tr("value")
    };
    const QCommandLineOption vertical_spacing_option {
        QStringList { "vs", "vertical-spacing" },
        QObject::tr("Vertical spacing"),
        QObject::tr("value")
    };
    const QCommandLineOption margin_top_option {
        QStringList { "mt", "margin-top" },
        QObject::tr("Margin top"),
        QObject::tr("value")
    };
    const QCommandLineOption margin_left_option {
        QStringList { "ml", "margin-left" },
        QObject::tr("Margin left"),
        QObject::tr("value")
    };

    const QList options = {
        m_help_options,
        texture_option,
        output_option,
        rows_option,
        columns_option,
        sprite_width_option,
        sprite_height_option,
        horizontal_spacing_option,
        vertical_spacing_option,
        margin_top_option,
        margin_left_option
    };

    QCommandLineParser parser;
    parser.addOptions(options);
    parser.process(args);

    if(parser.isSet(m_help_options.names().first()))
    {
        m_io.out << QObject::tr("Usage:") << " " << m_args[0] << " " << m_args[1] << " " << m_args[2] <<
                " " << "[options]" << Qt::endl <<
            Qt::endl <<
            QObject::tr("Options:") << Qt::endl <<
            options << Qt::endl;
        return noop(ExitCodes::Success);
    }

    GridOptions grid_options = {};
    QString texture;
    QString out_directory;

    if(parser.isSet(texture_option.names().first()))
        texture = parser.value(texture_option.names().first());
    if(parser.isSet(output_option.names().first()))
        out_directory = parser.value(output_option.names().first());
    if(parser.isSet(rows_option.names().first()))
        grid_options.row_count = parser.value(rows_option.names().first()).toInt();
    if(parser.isSet(columns_option.names().first()))
        grid_options.column_count = parser.value(columns_option.names().first()).toInt();
    if(parser.isSet(sprite_height_option.names().first()))
        grid_options.sprite_height = parser.value(sprite_height_option.names().first()).toInt();
    if(parser.isSet(sprite_width_option.names().first()))
        grid_options.sprite_width = parser.value(sprite_width_option.names().first()).toInt();
    if(parser.isSet(vertical_spacing_option.names().first()))
        grid_options.vertical_spacing = parser.value(vertical_spacing_option.names().first()).toInt();
    if(parser.isSet(horizontal_spacing_option.names().first()))
        grid_options.horizontal_spacing = parser.value(horizontal_spacing_option.names().first()).toInt();
    if(parser.isSet(margin_top_option.names().first()))
        grid_options.margin_top = parser.value(margin_top_option.names().first()).toInt();
    if(parser.isSet(margin_left_option.names().first()))
        grid_options.margin_left = parser.value(margin_left_option.names().first()).toInt();

    if(texture.isEmpty())
        return noRequiredArgument(texture_option);
    if(out_directory.isEmpty())
        return noRequiredArgument(output_option);
    if(grid_options.row_count <= 0)
        return noRequiredArgument(rows_option);
    if(grid_options.column_count <= 0)
        return noRequiredArgument(columns_option);
    if(grid_options.sprite_width <= 0)
        return noRequiredArgument(sprite_width_option);
    if(grid_options.sprite_height <= 0)
        return noRequiredArgument(sprite_height_option);
    if(grid_options.vertical_spacing < 0)
        return invalidArgument(vertical_spacing_option);
    if(grid_options.horizontal_spacing < 0)
        return invalidArgument(horizontal_spacing_option);
    if(grid_options.margin_top < 0)
        return invalidArgument(margin_top_option);
    if(grid_options.margin_left < 0)
        return invalidArgument(margin_left_option);

    return std::unique_ptr<Application>(new UnpackApplication(texture, grid_options, out_directory));
}

std::unique_ptr<Application> AppRunner::parseUnpackAtlas() const
{
QStringList args = m_args;
    args.removeAt(1); // remove the "unpack" argument to hide it from the parser
    args.removeAt(1); // remove the "atlas" argument to hide it from the parser

    const QCommandLineOption atlas_option {
        QStringList { "at", "atlas" },
        QObject::tr("Atlas filename"),
        QObject::tr("filename")
    };
    const QCommandLineOption output_option {
        QStringList { "out" },
        QObject::tr("Output directory"),
        QObject::tr("directory")
    };

    const QList options = {
        m_help_options,
        atlas_option,
        output_option
    };

    QCommandLineParser parser;
    parser.addOptions(options);
    parser.process(args);

    if(parser.isSet(m_help_options.names().first()))
    {
        m_io.out << QObject::tr("Usage:") << " " << m_args[0] << " " << m_args[1] << " " << m_args[2] <<
                " " << "[options]" << Qt::endl <<
            Qt::endl <<
            QObject::tr("Options:") << Qt::endl <<
            options << Qt::endl;
        return noop(ExitCodes::Success);
    }

    QString atlas_filename;
    QString out_directory;

    if(parser.isSet(atlas_option.names().first()))
        atlas_filename = parser.value(atlas_option.names().first());
    if(parser.isSet(output_option.names().first()))
        out_directory = parser.value(output_option.names().first());

    if(atlas_filename.isEmpty())
        return noRequiredArgument(atlas_option);
    if(out_directory.isEmpty())
        return noRequiredArgument(output_option);

    return std::unique_ptr<Application>(new UnpackApplication(atlas_filename, out_directory));
}

} // namespace

int main(int _argc, char * _argv[])
{
    QCoreApplication app(_argc, _argv);
    QCoreApplication::setApplicationName(__S2TP_BIN);
    QCoreApplication::setApplicationVersion(__S2TP_VERSION);
    QCoreApplication::setOrganizationName(__S2TP_ORG);
    IO io
    {
        .out = QTextStream(stdout, QIODevice::WriteOnly),
        .err = QTextStream(stderr, QIODevice::WriteOnly)
    };
    try
    {
        return AppRunner::run(io, QCoreApplication::arguments())->exec();
    }
    catch(const Exception & e)
    {
        io.err << e.message() << Qt::endl;
        return ExitCodes::Error;
    }
}
