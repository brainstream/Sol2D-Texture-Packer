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
#include <Sol2dTexturePackerGui/Animator/SpriteAnimationWidget.h>
#include <Sol2dTexturePackerGui/MainWindow.h>
#include <Sol2dTexturePackerGui/AboutDialog.h>
#include <Sol2dTexturePackerGui/Settings.h>
#include <QFileInfo>
#include <QShortcut>

MainWindow::MainWindow(QWidget *_parent) :
    QMainWindow(_parent),
    m_split_icon(new QIcon(":/icons/edit-cut")),
    m_pack_icon(new QIcon(":/icons/package-x-generic")),
    m_animation_icon(new QIcon(":/icons/video-x-generic"))
{
    setupUi(this);
    QSettings settings;
    restoreGeometry(settings.value(Settings::MainWindow::geometry).toByteArray());
    restoreState(settings.value(Settings::MainWindow::state).toByteArray());

    m_btn_pack_sprites->setDefaultAction(m_action_pack_sprites);
    m_btn_split_sheet->setDefaultAction(m_action_split_sheet);
    m_btn_animation->setDefaultAction(m_action_animation);
    m_btn_about->setDefaultAction(m_action_about);

    QShortcut * shortcut_close_tab = new QShortcut(QKeySequence("CTRL+W"), this);

    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(shortcut_close_tab, &QShortcut::activated, this, &MainWindow::closeActiveTab);
    connect(m_action_split_sheet, &QAction::triggered, this, &MainWindow::showSheetSplitter);
    connect(m_action_pack_sprites, &QAction::triggered, this, &MainWindow::showSpritePacker);
    connect(m_action_animation, &QAction::triggered, this, &MainWindow::showSpriteAnimator);
    connect(m_action_about, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

MainWindow::~MainWindow()
{
    delete m_split_icon;
    delete m_pack_icon;
}

void MainWindow::closeEvent(QCloseEvent * _event)
{
    Q_UNUSED(_event)
    QSettings settings;
    settings.setValue(Settings::MainWindow::geometry, saveGeometry());
    settings.setValue(Settings::MainWindow::state, saveState());
}

void MainWindow::showSheetSplitter()
{
    SpriteSheetSplitterWidget * widget = new SpriteSheetSplitterWidget(this);
    widget->setSpriteAnimationPipe(this);
    m_tabs->setCurrentIndex(m_tabs->addTab(widget, *m_split_icon, tr("Split Sprite Sheet")));
    connect(widget, &SpriteSheetSplitterWidget::sheetLoaded, this, [this, widget](const QString & __filename) {
        int index = m_tabs->indexOf(widget);
        if(index >= 0)
        {
            QFileInfo fi(__filename);
            m_tabs->setTabText(index, tr("Splitting: %1").arg(fi.fileName()));
        }
    });
}

void MainWindow::showSpritePacker()
{
    SpritePackerWidget * widget = new SpritePackerWidget(this);
    m_tabs->setCurrentIndex(m_tabs->addTab(widget, *m_pack_icon, tr("Pack Sprites")));
    connect(widget, &SpritePackerWidget::packNameChanged, this, [this, widget](const QString & __name) {
        int index = m_tabs->indexOf(widget);
        if(index >= 0)
            m_tabs->setTabText(index, tr("Packing: %1").arg(__name));
    });
}

void MainWindow::showSpriteAnimator()
{
    produceAnimation({});
}

void MainWindow::produceAnimation(const QList<Sprite> & _sprites)
{
    SpriteAnimationWidget * widget = new SpriteAnimationWidget(this);
    m_tabs->setCurrentIndex(m_tabs->addTab(widget, *m_animation_icon, tr("Sprite Animation")));
    if(!_sprites.isEmpty())
        widget->setSprites(_sprites);
}

void MainWindow::closeActiveTab()
{
    int index = m_tabs->currentIndex();
    if(index >= 0)
        closeTab(index);
}

void MainWindow::closeTab(int _index)
{
    m_tabs->widget(_index)->deleteLater();
}

void MainWindow::showAboutDialog()
{
    AboutDialog dlg(this);
    dlg.exec();
}
