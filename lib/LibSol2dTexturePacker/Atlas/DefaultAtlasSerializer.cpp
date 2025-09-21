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

#include <LibSol2dTexturePacker/Atlas/DefaultAtlasSerializer.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QFile>
#include <QXmlStreamWriter>
#include <QDomDocument>

namespace {

const char * g_xml_tag_atlas = "atlas";
const char * g_xml_tag_frame = "frame";
const char * g_xml_attr_texture = "texture";
const char * g_xml_attr_name = "name";
const char * g_xml_attr_x = "x";
const char * g_xml_attr_y = "y";
const char * g_xml_attr_width = "width";
const char * g_xml_attr_height = "height";

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

void DefaultAtlasSerializer::serialize(const Atlas & _atlas, const QString & _file)
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
    xml.writeAttribute(g_xml_attr_texture, makeTextureRelativePath(_atlas, _file));
    for(int i = 0; i < _atlas.frames.count(); ++i)
    {
        const Frame & frame = _atlas.frames[i];
        xml.writeStartElement(g_xml_tag_frame);
        if(frame.name.isEmpty())
            xml.writeAttribute(g_xml_attr_name, makeDefaultFrameName(_atlas, i + 1));
        else
            xml.writeAttribute(g_xml_attr_name, frame.name);
        xml.writeAttribute(g_xml_attr_x, QString::number(frame.x));
        xml.writeAttribute(g_xml_attr_y, QString::number(frame.y));
        xml.writeAttribute(g_xml_attr_width, QString::number(frame.width));
        xml.writeAttribute(g_xml_attr_height, QString::number(frame.height));
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeEndDocument();
}

void DefaultAtlasSerializer::deserialize(const QString & _file, Atlas & _atlas)
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
    Atlas tmp_atlas;
    tmp_atlas.texture = xatlas.attribute(g_xml_attr_texture);
    quint32 frame_position = 1;
    for(
        QDomElement xframe = xatlas.firstChildElement(g_xml_tag_frame);
        !xframe.isNull();
        xframe = xframe.nextSiblingElement(g_xml_tag_frame))
    {
        tmp_atlas.frames.append({
            .name = xframe.attribute(g_xml_attr_name),
            .x = getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_x, frame_position),
            .y = getXmlFrameIntAttribute<qint32>(_file, xframe, g_xml_attr_y, frame_position),
            .width = getXmlFrameIntAttribute<quint32>(_file, xframe, g_xml_attr_width, frame_position),
            .height = getXmlFrameIntAttribute<quint32>(_file, xframe, g_xml_attr_height, frame_position)
        });
        ++frame_position;
    }
    _atlas = tmp_atlas;
}
