/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Open Sprite Sheet Tools.                                                      *
 *                                                                                                        *
 * Open Sprite Sheet Tools is free software: you can redistribute it and/or modify it under  the terms of *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Open Sprite Sheet Tools is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;   *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#pragma once

#include <LibSol2dTexturePacker/Splitters/Splitter.h>
#include <LibSol2dTexturePacker/Atlas/AtlasSerializer.h>
#include <memory>

class S2TP_EXPORT AtlasSplitter : public Splitter
{
    Q_OBJECT

public:
    AtlasSplitter(QObject * _parent);
    bool forEachFrame(std::function<void(const Frame &)> _cb) const override;
    qsizetype frameCount() const override;
    void reset() override;

public slots:
    void setDataFile(const QString & _file);

signals:
    void error(const QString & _message);

private:
    std::unique_ptr<AtlasSerializer> m_serializer;
    std::unique_ptr<Atlas> m_atlas;
};

