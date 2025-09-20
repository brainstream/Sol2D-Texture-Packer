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

#include <Sol2dTexturePackerGui/ZoomWidget.h>

namespace {

class ComboBoxValidator : public QValidator
{
public:
    explicit ComboBoxValidator(QObject * _parent);
    State validate(QString & _input, int & _position) const override;
};

} // namespace name

inline ComboBoxValidator::ComboBoxValidator(QObject * _parent) :
    QValidator(_parent)
{
}

QValidator::State ComboBoxValidator::validate(QString & _input, int & _position) const
{
    if(_input.length())
    {
        if(_input.endsWith('%'))
        {
            if(_position == _input.length())
                _position = _input.length() - 1;
            return Acceptable;
        }
        quint32 value = 0;
        foreach(QChar symbol, _input)
        {
            if(symbol == '%') continue;
            if(symbol.isDigit()) value = value * 10 + symbol.digitValue();
            else return Acceptable;
        }
        if(value) _input = QString("%1%").arg(value);
    }
    return Acceptable;
}

ZoomWidget::ZoomWidget(QWidget * _parent) :
    QWidget(_parent),
    m_model(new ZoomModel(100, this))
{
    setupUi(this);
    m_slider_zoom->setRange(1, 1600);
    m_slider_zoom->setSingleStep(5);
    m_slider_zoom->setPageStep(100);
    m_combo_zoom->addItem("100%");
    m_combo_zoom->addItem("200%");
    m_combo_zoom->addItem("400%");
    m_combo_zoom->addItem("800%");;
    m_combo_zoom->addItem("1600%");
    m_slider_zoom->setValue(100);
    m_combo_zoom->setCurrentIndex(0);
    m_combo_zoom->setValidator(new ComboBoxValidator(this));
    connect(m_combo_zoom, &QComboBox::editTextChanged, this, &ZoomWidget::onComboboxTextChanged);
    connect(m_slider_zoom, &QSlider::valueChanged, m_model, &ZoomModel::setZoom);
    connect(m_model, &ZoomModel::zoomChanged, this, &ZoomWidget::applyZoomValue);
}

void ZoomWidget::onComboboxTextChanged(const QString & _text)
{
    if(_text.endsWith('%'))
    {
        QString value_txt = _text.left(_text.length() - 1);
        bool is_ok;
        int value = value_txt.toInt(&is_ok);
        if(is_ok && value > 0)
        {
            m_slider_zoom->setValue(value);
            m_model->setZoom(value);
        }
    }
}

void ZoomWidget::onSliderValueCahnged(int _value)
{
    m_combo_zoom->setCurrentText(QString("%1%").arg(_value));
    m_model->setZoom(_value);
}

void ZoomWidget::applyZoomValue(quint32 _zoom)
{
    m_combo_zoom->setCurrentText(QString("%1%").arg(_zoom));
    m_slider_zoom->setValue(_zoom);
}
