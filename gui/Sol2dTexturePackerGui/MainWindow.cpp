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

#include <Sol2dTexturePackerGui/Splitter/SpriteSheetSplitterWidget.h>
#include <Sol2dTexturePackerGui/Packer/SpritePackerWidget.h>
#include <Sol2dTexturePackerGui/MainWindow.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *_parent) :
    QMainWindow(_parent)
{
    setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(gc_settings_key_wnd_geom).toByteArray());
    restoreState(settings.value(gc_settings_key_wnd_state).toByteArray());

    m_btn_pack_sprites->setDefaultAction(m_action_pack_sprites);
    m_btn_split_sheet->setDefaultAction(m_action_split_sheet);

    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_action_split_sheet, &QAction::triggered, this, &MainWindow::showSheetSplitter);
    connect(m_action_pack_sprites, &QAction::triggered, this, &MainWindow::showSpritePacker);
}

void MainWindow::closeEvent(QCloseEvent * _event)
{
    Q_UNUSED(_event)
    QSettings settings;
    settings.setValue(gc_settings_key_wnd_geom, saveGeometry());
    settings.setValue(gc_settings_key_wnd_state, saveState());
}

void MainWindow::showSheetSplitter()
{
    SpriteSheetSplitterWidget * widget = new SpriteSheetSplitterWidget(this);
    m_tabs->setCurrentIndex(m_tabs->addTab(widget, tr("Split Sprite Sheet")));
    connect(widget, &SpriteSheetSplitterWidget::sheetLoaded, this, [this, widget](const QString & _filename) {
        int index = m_tabs->indexOf(widget);
        if(index >= 0)
        {
            QFileInfo fi(_filename);
            m_tabs->setTabText(index, tr("Splitting: %1").arg(fi.fileName()));
        }
    });
}

void MainWindow::showSpritePacker()
{
    SpritePackerWidget * widget = new SpritePackerWidget(this);
    int tab = m_tabs->addTab(widget, tr("Pack Sprites"));
    m_tabs->setCurrentIndex(tab);
}

void MainWindow::closeTab(int _index)
{
    m_tabs->widget(_index)->deleteLater();
}
