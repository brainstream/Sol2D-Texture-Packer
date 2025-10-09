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

#include <Sol2dTexturePackerGui/Animator/SpriteAnimationWidget.h>

SpriteAnimationWidget::SpriteAnimationWidget(QWidget * _parent) :
    QWidget(_parent),
    m_timer_id(-1),
    m_next_frame_index(0)
{
    setupUi(this);
    m_default_preview_size = m_label_animation->size();
    connect(m_spin_box_frame_rate, &QSpinBox::valueChanged, this, &SpriteAnimationWidget::updateTimer);
    connect(m_widget_sprite_list, &SpriteListWidget::spriteListChanged, this, &SpriteAnimationWidget::updateFrames);
    updateFrames();
}

void SpriteAnimationWidget::updateTimer()
{
    stopTimer();
    if(!m_frames.isEmpty())
    {
        int fps = m_spin_box_frame_rate->value();
        if(fps <= 0) fps = 1;
        m_timer_id = startTimer(1000.0 / fps);
    }
}

void SpriteAnimationWidget::stopTimer()
{
    if(m_timer_id >= 0)
    {
        killTimer(m_timer_id);
        m_timer_id = -1;
    }
}

void SpriteAnimationWidget::updateFrames()
{
    stopTimer();
    m_frames.clear();
    m_label_animation->clear();
    const QList<Sprite> & sprites = m_widget_sprite_list->sprites();
    m_frames.reserve(sprites.count());
    int max_width = 0;
    int max_height = 0;
    foreach(const Sprite & sprite, sprites)
    {
        m_frames.append(QPixmap::fromImage(sprite.image));
        if(sprite.image.width() > max_width)
            max_width = sprite.image.width();
        if(sprite.image.height() > max_height)
            max_height = sprite.image.height();
    }
    if(max_width && max_height)
        m_label_animation->setMinimumSize(max_width, max_height);
    else
        m_label_animation->setMinimumSize(m_default_preview_size);
    updateTimer();
}

void SpriteAnimationWidget::timerEvent(QTimerEvent *)
{
    if(m_frames.isEmpty())
        return;
    if(m_next_frame_index >= m_frames.count())
        m_next_frame_index = 0;
    m_label_animation->setPixmap(m_frames[m_next_frame_index]);
    ++m_next_frame_index;
}

void SpriteAnimationWidget::setSprites(const QList<Sprite> & _sprites)
{
    m_widget_sprite_list->setSprites(_sprites);
}
