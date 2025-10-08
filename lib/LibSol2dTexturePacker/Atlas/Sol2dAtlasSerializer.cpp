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

#include <LibSol2dTexturePacker/Atlas/Sol2dAtlasSerializer.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamWriter>
#include <QDomDocument>

namespace {

const char * g_xml_tag_atlas = "atlas";
const char * g_xml_tag_frame = "frame";
const char * g_xml_attr_version = "version";
const char * g_xml_attr_texture = "texture";
const char * g_xml_attr_name = "name";
const char * g_xml_attr_rotated = "rotated";
const char * g_xml_attr_texture_x = "tx";
const char * g_xml_attr_texture_y = "ty";
const char * g_xml_attr_texture_width = "tw";
const char * g_xml_attr_texture_height = "th";
const char * g_xml_attr_sprite_x = "sx";
const char * g_xml_attr_sprite_y = "sy";
const char * g_xml_attr_sprite_width = "sw";
const char * g_xml_attr_sprite_height = "sh";

void validateXmlFrameAttribute(
    const QString & _file,
    const QDomElement & _xframe,
    const char * _attribute_name,
    quint32 _frame_position)
{
    if(!_xframe.hasAttribute(_attribute_name))
    {
        throw InvalidFileFormatException(
            _file,
            QObject::tr("XML element \"%1\" at position %2 must contain attribute \"%3\"")
                .arg(g_xml_tag_frame)
                .arg(_frame_position)
                .arg(_attribute_name)
        );
    }
}

template<std::integral Int>
Int getXmlFrameIntAttribute(
    const QString & _file,
    const QDomElement & _xframe,
    const char * _attribute_name,
    quint32 _frame_position)
{
    validateXmlFrameAttribute(_file, _xframe, _attribute_name, _frame_position);
    QString attr_value = _xframe.attribute(_attribute_name);
    bool is_ok;
    qint32 value = attr_value.toLongLong(&is_ok);
    if(!is_ok)
    {
        throw InvalidFileFormatException(
            _file,
            QObject::tr("The XML attribute \"%1\" of element \"%2\" at position %3 must be of type integer.")
                .arg(_attribute_name)
                .arg(g_xml_tag_frame)
                .arg(_frame_position)
        );
    }
    return static_cast<Int>(value);
}

} // namespace


const int Sol2dAtlasSerializer::m_latest_version = 1;

void Sol2dAtlasSerializer::serialize(const Atlas & _atlas, const QString & _file)
{
    QFile file(_file);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        throw FileOpenException(_file, FileOpenException::Write);
    }
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(g_xml_tag_atlas);
    xml.writeAttribute(g_xml_attr_version, QString::number(m_latest_version));
    xml.writeAttribute(g_xml_attr_texture, makeTextureRelativePath(_atlas, _file));
    for(int i = 0; i < _atlas.frames.count(); ++i)
    {
        const Frame & frame = _atlas.frames[i];
        xml.writeStartElement(g_xml_tag_frame);
        if(frame.name.isEmpty())
            xml.writeAttribute(g_xml_attr_name, makeDefaultFrameName(_atlas, i + 1));
        else
            xml.writeAttribute(g_xml_attr_name, frame.name);
        xml.writeAttribute(g_xml_attr_texture_x, QString::number(frame.texture_rect.x()));
        xml.writeAttribute(g_xml_attr_texture_y, QString::number(frame.texture_rect.y()));
        xml.writeAttribute(g_xml_attr_texture_width, QString::number(frame.texture_rect.width()));
        xml.writeAttribute(g_xml_attr_texture_height, QString::number(frame.texture_rect.height()));
        xml.writeAttribute(g_xml_attr_sprite_x, QString::number(frame.sprite_rect.x()));
        xml.writeAttribute(g_xml_attr_sprite_y, QString::number(frame.sprite_rect.y()));
        xml.writeAttribute(g_xml_attr_sprite_width, QString::number(frame.sprite_rect.width()));
        xml.writeAttribute(g_xml_attr_sprite_height, QString::number(frame.sprite_rect.height()));
        if(frame.is_rotated)
            xml.writeAttribute(g_xml_attr_rotated, "true");
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeEndDocument();
}

void Sol2dAtlasSerializer::deserialize(const QString & _file, Atlas & _atlas)
{
    QFile file(_file);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw FileOpenException(_file, FileOpenException::Read);
    }
    QDomDocument xml;
    if(!xml.setContent(&file))
    {
        throw InvalidXmlException(_file);
    }
    QDomElement xatlas = xml.firstChildElement(g_xml_tag_atlas);
    if(xatlas.isNull())
    {
        throw InvalidFileFormatException(
            _file,
            QObject::tr("The XML root element must be \"%1\"").arg(g_xml_tag_atlas));
    }
    {
        bool is_version_parsed;
        int version = xatlas.attribute(g_xml_attr_version).toInt(&is_version_parsed);
        if(!is_version_parsed)
        {
            throw InvalidFileFormatException(
                _file,
                QObject::tr("Tag \"%1\" must contain attribute \"%2\"").arg(g_xml_tag_atlas, g_xml_attr_version));
        }
        if(version != m_latest_version)
        {
            throw InvalidFileFormatException(
                _file,
                QObject::tr("Unsupported version %1, latest supported version is %2").arg(version).arg(m_latest_version));
        }
    }
    Atlas tmp_atlas;
    tmp_atlas.texture = xatlas.attribute(g_xml_attr_texture);
    {
        QFileInfo texture_fi(xatlas.attribute(g_xml_attr_texture));
        if(!texture_fi.isAbsolute())
            tmp_atlas.texture = QFileInfo(_file).dir().absoluteFilePath(tmp_atlas.texture);
    }
    quint32 frame_position = 1;
    for(
        QDomElement xframe = xatlas.firstChildElement(g_xml_tag_frame);
        !xframe.isNull();
        xframe = xframe.nextSiblingElement(g_xml_tag_frame))
    {
        tmp_atlas.frames.append({
            .texture_rect = QRect(
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_texture_x, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_texture_y, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_texture_width, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_texture_height, frame_position)
            ),
            .sprite_rect = QRect(
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_sprite_x, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_sprite_y, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_sprite_width, frame_position),
                getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_sprite_height, frame_position)
            ),
            .name = xframe.attribute(g_xml_attr_name),
            .is_rotated = xframe.attribute(g_xml_attr_rotated).compare("true", Qt::CaseInsensitive) == 0
        });
        ++frame_position;
    }
    _atlas = tmp_atlas;
}
