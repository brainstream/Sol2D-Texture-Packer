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
#include <LibSol2dTexturePacker/Packers/MaxRectsBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/SkylineBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Packers/GuillotineBinAtlaskPacker.h>
#include <LibSol2dTexturePacker/Packers/ShelfBinAtlasPacker.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QImageWriter>
#include <QCommandLineParser>
#include <QIODevice>
#include <QTextStream>
#include <QMap>
#include <QtGlobal>
#include <memory>
#include <functional>

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

QString joinOptionNames(const QStringList & _flags)
{
    QString result;
    foreach(const QString & name, _flags)
    {
        QString n = name.length() == 1 ? QString("-") + name : QString("--") + name;
        if(result.isEmpty())
            result = n;
        else
            result += QString(", ") + n;
    }
    return result;
}

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
        opt.names = joinOptionNames(o.names());
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
        m_help_options({{ "h", "help" }, QObject::tr("Display help")})
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
        QObject::tr("Display version")
    };
    const QList options
    {
        m_help_options,
        version_option
    };
    parser.addOptions(options);
    parser.process(m_args);

    int exit_code = ExitCodes::Success;

    if(parser.isSet(version_option.names().constFirst()))
    {
        m_io.out << __S2TP_APP << ": " << __S2TP_VERSION << Qt::endl <<
            "Qt: " << qVersion() << Qt::endl;
    }
    else
    {
        if(!parser.isSet(m_help_options.names().constFirst()))
        {
            m_io.err << QObject::tr("Command not specified") << Qt::endl;
            exit_code = ExitCodes::RequiredArgumentNotSpecified;
        }
        m_io.out <<
            QObject::tr("Usage") << ": " << m_args[0] << " " <<
                QObject::tr("[global options] [command] [command args]") << Qt::endl <<
            Qt::endl <<
            QObject::tr("Global options") << ":" << Qt::endl <<
            options << Qt::endl <<
            Qt::endl <<
            QObject::tr("Commands") << ":" << Qt::endl <<
            " pack    " << QObject::tr("Pack sprites into an atlas") << Qt::endl <<
            " unpack  " << QObject::tr("Unpack a texture") << Qt::endl;
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
        QObject::tr("Required argument not specified or has invalid value") << ": " <<
        _option.description() <<
        Qt::endl;
    return noop(ExitCodes::RequiredArgumentNotSpecified);
}

std::unique_ptr<Application> AppRunner::invalidArgument(const QCommandLineOption & _option) const
{
    m_io.err <<
        QObject::tr("Argument has invalid value") << ": " <<
        _option.description() <<
        Qt::endl;
    return noop(ExitCodes::InvalidArgumentValue);
}

std::unique_ptr<Application> AppRunner::parsePack() const
{
    enum AlgoritmOptions : int
    {
        OptionFlagNone = 0x0,
        OptionFlagAllowFlip = 0x1,
        OptionFlagAllowMerge = 0x2,
        OptionFlagUseWasteMap = 0x4
    };

    struct
    {
        QString name;
        QMap<QString, QString> choice_heuristics;
        QMap<QString, QString> split_heuristics;
        std::function<AtlasPacker * ()> create;
        AlgoritmOptions algoritm_options;
    }
    algoritms[] =
    {
        {
            .name = "maxrects",
            .choice_heuristics =
            {
                { "BLSF", QObject::tr("Best Long Side Fit (default)") },
                { "BSSF", QObject::tr("Best Short Side Fit") },
                { "BAF", QObject::tr("Best Area Fit") },
                { "BL", QObject::tr("Bottom Left Rule") },
                { "CP", QObject::tr("Contact Point Rule") }
            },
            .split_heuristics = {},
            .create = []() { return new MaxRectsBinAtlasPacker(); },
            .algoritm_options = OptionFlagAllowFlip
        },
        {
            .name = "skyline",
            .choice_heuristics =
            {
                { "BL", QObject::tr("Bottom Left (default)") },
                { "MWF", QObject::tr("Min Waste Fit") }
            },
             .split_heuristics = {},
            .create = []() { return new SkylineBinAtlasPacker(); },
            .algoritm_options = OptionFlagUseWasteMap
        },
        {
            .name = "guillotine",
            .choice_heuristics =
             {
                { "BAF", QObject::tr("Best Area Fit (default)") },
                { "BLSF", QObject::tr("Best Long Side Fit")  },
                { "BSSF", QObject::tr("Best Short Side Fit")  },
                { "WAF", QObject::tr("Worst Area Fit")  },
                { "WSSF", QObject::tr("Worst Short Side Fit")  },
                { "WLSF", QObject::tr("Worst Long Side Fit")  }
             },
            .split_heuristics =
            {
                { "SLA", QObject::tr("Shorter Leftover Axis (default)") },
                { "LLA", QObject::tr("Longer Leftover Axis") },
                { "MIN", QObject::tr("Minimize Area") },
                { "MAX", QObject::tr("Maximize Area") },
                { "SA", QObject::tr("Shorter Axis") },
                { "LA", QObject::tr("Longer Axis") }
            },
            .create = []() { return new GuillotineBinAtlaskPacker(); },
            .algoritm_options = OptionFlagAllowMerge
        },
        {
            .name = "shelf",
             .choice_heuristics =
             {
                { "NF", QObject::tr("Next Fit (default)") },
                { "FF", QObject::tr("First Fit") },
                { "BAF", QObject::tr("Best Area Fit") },
                { "WAF", QObject::tr("Worst Area Fit") },
                { "BWF", QObject::tr("Best Width Fit") },
                { "WWF", QObject::tr("Worst Width Fit") }
             },
             .split_heuristics = {},
            .create = []() { return new ShelfBinAtlasPacker(); },
            .algoritm_options = OptionFlagUseWasteMap
        }
    };


    // 1st argument is app name
    // 2nd argument is "pack"
    QStringList args = m_args;
    args.removeAt(1); // remove the "pack" argument to hide it from the parser

    const QString default_atlas_name = QObject::tr("texture");

    QCommandLineParser parser;
    const QCommandLineOption algorithm_option = {
        { "a", "algorithm" },
        QObject::tr("Packing algoritm"),
        QObject::tr("algorithm name")
    };
    const QCommandLineOption choice_heuristic_option = {
        { "c", "choice-heuristic" },
        QObject::tr("Algorithm-specific choice heuristic"),
        QObject::tr("heuristic")
    };
    const QCommandLineOption split_heuristic_option = {
        { "s", "split-heuristic" },
        QObject::tr("Algorithm-specific split heuristic"),
        QObject::tr("heuristic")
    };
    const QCommandLineOption allow_flip_option = {
        { "f", "allow-flip" },
        QObject::tr("Algorithm-specific option that allows sprites to be flipped")
    };
    const QCommandLineOption use_waste_map_option = {
        { "w", "use-waste-map" },
        QObject::tr("Algorithm-specific option that enables the use of a waste map")
    };
    const QCommandLineOption output_directory_option = {
        { "o", "output" },
        QObject::tr("Output directory (default: .)"),
        QObject::tr("existent directory")
    };
    const QCommandLineOption output_name_option = {
        { "n", "name" },
        QObject::tr("Atlas name (default: %1)").arg(default_atlas_name),
        QObject::tr("name")
    };
    const QCommandLineOption allow_merge_option = {
        { "m", "allow-merge" },
        QObject::tr("Algorithm-specific option that allows merge rectangles")
    };
    const QCommandLineOption max_width_option = {
        { "x", "max-width" },
        QObject::tr("Maximum width of the atlas texture (default: 2048)"),
        QObject::tr("value in pixels")
    };
    const QCommandLineOption max_height_option = {
        { "y", "max-height" },
        QObject::tr("Maximum height of the atlas texture (default: 2048)"),
        QObject::tr("value in pixels")
    };
    const QCommandLineOption crop_option = {
        { "r", "crop" },
        QObject::tr("Remove whitespace around sprites to save space in the atlas")
    };
    const QCommandLineOption detect_duplicates_option = {
        { "d", "detect-duplicates" },
        QObject::tr("Remove duplicates sprites to save space in the atlas")
    };
    const QCommandLineOption remove_file_ext_option = {
        { "e", "remove-ext" },
        QObject::tr("Remove file extensions from frame names")
    };
    const QCommandLineOption format_option = {
        { "t", "format" },
        QObject::tr("Texture file format. Supported formats (default: png)"),
        QObject::tr("format")
    };
    const QList options
    {
        m_help_options,
        algorithm_option,
        choice_heuristic_option,
        split_heuristic_option,
        allow_flip_option,
        use_waste_map_option,
        allow_merge_option,
        output_directory_option,
        output_name_option,
        max_width_option,
        max_height_option,
        crop_option,
        detect_duplicates_option,
        remove_file_ext_option,
        format_option
    };
    parser.addOptions(options);
    parser.process(args);

    int exit_code = ExitCodes::Success;

    if(!parser.isSet("help"))
    {
        exit_code = ExitCodes::RequiredArgumentNotSpecified;
    }

    m_io.out <<
        QObject::tr("Usage") << ": " << m_args[0] << " " << m_args[1] << " " <<
        QObject::tr("[options] <sprites...>") << Qt::endl <<
        Qt::endl <<
        QObject::tr("Options") << ":" << Qt::endl <<
        options << Qt::endl <<
        Qt::endl <<
        QObject::tr("Algorithms") << ":" << Qt::endl;

    for(size_t i = 0; i < sizeof(algoritms) / sizeof(algoritms[0]); ++i)
    {
        const auto & alg = algoritms[i];
        m_io.out << Qt::endl << " - " << alg.name;
        if(i == 0)
            m_io.out << " (" << QObject::tr("default") << ")";
        m_io.out << Qt::endl;
        if(!alg.choice_heuristics.empty())
        {
            m_io.out << "   " << QObject::tr("Choice heuristics") << ":" << Qt::endl;
            for(const auto [name, desc] : alg.choice_heuristics.asKeyValueRange())
                m_io.out << "    - " << name << ": " << desc << Qt::endl;
        }
        if(!alg.split_heuristics.empty())
        {
            m_io.out << "   " << QObject::tr("Split heuristics") << ":" << Qt::endl;
            for(const auto [name, desc] : alg.split_heuristics.asKeyValueRange())
                m_io.out << "    - " << name << ": " << desc << Qt::endl;
        }
        if(alg.algoritm_options)
        {
            m_io.out << "   " << QObject::tr("Supported options") << ":" << Qt::endl;
            if(alg.algoritm_options & OptionFlagAllowFlip)
                m_io.out << "    " << joinOptionNames(allow_flip_option.names()) << Qt::endl;
            if(alg.algoritm_options & OptionFlagUseWasteMap)
                m_io.out << "    " << joinOptionNames(use_waste_map_option.names()) << Qt::endl;
            if(alg.algoritm_options & OptionFlagAllowMerge)
                m_io.out << "    " <<  joinOptionNames(allow_merge_option.names()) << Qt::endl;
        }
    }

    m_io.out << Qt::endl << QObject::tr("Supported texture file formats (depends on Qt)") << ": " <<
        QImageWriter().supportedImageFormats().join(", ") << Qt::endl;

    // return std::unique_ptr<Application>(new PackApplication);
    return noop(exit_code);
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
        QObject::tr("Usage") << ": " << m_args[0] << " " << m_args[1] << " " <<
            QObject::tr("[options] [method] [method options]") << Qt::endl <<
        Qt::endl <<
        QObject::tr("Options") << ":" << Qt::endl <<
        options << Qt::endl <<
        Qt::endl <<
        QObject::tr("Methods") << ":" << Qt::endl <<
        " grid   " << QObject::tr("Unpack a grid-aligned texture") << Qt::endl <<
        " atlas  " << QObject::tr("Unpack a texture using an atlas") << Qt::endl;

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

    if(parser.isSet(m_help_options.names().constFirst()))
    {
        m_io.out << QObject::tr("Usage") << ": " << m_args[0] << " " << m_args[1] << " " << m_args[2] <<
                " " << "[options]" << Qt::endl <<
            Qt::endl <<
            QObject::tr("Options") << ":" << Qt::endl <<
            options << Qt::endl;
        return noop(ExitCodes::Success);
    }

    GridOptions grid_options = {};
    QString texture;
    QString out_directory;

    if(parser.isSet(texture_option.names().constFirst()))
        texture = parser.value(texture_option.names().constFirst());
    if(parser.isSet(output_option.names().constFirst()))
        out_directory = parser.value(output_option.names().constFirst());
    if(parser.isSet(rows_option.names().constFirst()))
        grid_options.row_count = parser.value(rows_option.names().constFirst()).toInt();
    if(parser.isSet(columns_option.names().constFirst()))
        grid_options.column_count = parser.value(columns_option.names().constFirst()).toInt();
    if(parser.isSet(sprite_height_option.names().constFirst()))
        grid_options.sprite_height = parser.value(sprite_height_option.names().constFirst()).toInt();
    if(parser.isSet(sprite_width_option.names().constFirst()))
        grid_options.sprite_width = parser.value(sprite_width_option.names().constFirst()).toInt();
    if(parser.isSet(vertical_spacing_option.names().constFirst()))
        grid_options.vertical_spacing = parser.value(vertical_spacing_option.names().constFirst()).toInt();
    if(parser.isSet(horizontal_spacing_option.names().constFirst()))
        grid_options.horizontal_spacing = parser.value(horizontal_spacing_option.names().constFirst()).toInt();
    if(parser.isSet(margin_top_option.names().constFirst()))
        grid_options.margin_top = parser.value(margin_top_option.names().constFirst()).toInt();
    if(parser.isSet(margin_left_option.names().constFirst()))
        grid_options.margin_left = parser.value(margin_left_option.names().constFirst()).toInt();

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

    if(parser.isSet(m_help_options.names().constFirst()))
    {
        m_io.out << QObject::tr("Usage") << ": " << m_args[0] << " " << m_args[1] << " " << m_args[2] <<
                " " << "[options]" << Qt::endl <<
            Qt::endl <<
            QObject::tr("Options") << ":" << Qt::endl <<
            options << Qt::endl;
        return noop(ExitCodes::Success);
    }

    QString atlas_filename;
    QString out_directory;

    if(parser.isSet(atlas_option.names().constFirst()))
        atlas_filename = parser.value(atlas_option.names().constFirst());
    if(parser.isSet(output_option.names().constFirst()))
        out_directory = parser.value(output_option.names().constFirst());

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
