/*
   UFOCR - User-Friendly OCR
   Copyright (C) 2017 Alexander Zaitsev <zamazan4ik@tut.by>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "QBusyIndicator.h"

#include <QPainter>

const int defaultInterval = 64;

QBusyIndicator::QBusyIndicator(QWidget* parent)
        : QWidget(parent),
          m_showBackground(false),
          m_angle(0),
          m_timerId(-1),
          m_speed(1),
          m_displayedWhenStopped(false),
          m_color(Qt::black),
          m_backgroundColor(Qt::white)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool QBusyIndicator::isAnimated() const
{
    return (m_timerId != -1);
}

void QBusyIndicator::setDisplayedWhenStopped(bool state)
{
    m_displayedWhenStopped = state;

    update();
}

bool QBusyIndicator::isDisplayedWhenStopped() const
{
    return m_displayedWhenStopped;
}

void QBusyIndicator::startAnimation()
{
    m_angle = 0;

    if (m_timerId == -1)
    {
        m_timerId = startTimer(m_speed * defaultInterval);
    }
}

void QBusyIndicator::stopAnimation()
{
    if (m_timerId != -1)
    {
        killTimer(m_timerId);
    }

    m_timerId = -1;

    update();
}

void QBusyIndicator::setSpeed(double speed)
{
    if (m_timerId != -1)
    {
        killTimer(m_timerId);
    }

    m_speed = speed;

    if (m_timerId != -1)
    {
        m_timerId = startTimer(m_speed * defaultInterval);
    }
}

void QBusyIndicator::setColor(const QColor& color)
{
    m_color = color;

    update();
}

void QBusyIndicator::setShowBackground(bool state)
{
    m_showBackground = state;
    update();
}

void QBusyIndicator::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
    update();
}

QSize QBusyIndicator::sizeHint() const
{
    return QSize(20, 20);
}

int QBusyIndicator::heightForWidth(int w) const
{
    return w;
}

void QBusyIndicator::timerEvent(QTimerEvent* /*event*/)
{
    m_angle = (m_angle + 20) % 360;

    update();
}

void QBusyIndicator::paintEvent(QPaintEvent* /*event*/)
{
    if (!m_displayedWhenStopped && !isAnimated())
    {
        return;
    }

    int width = qMin(this->width(), this->height());

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int outerRadius = m_showBackground ? (width - 8) * 0.5 : (width - 4) * 0.5;
    int innerRadius = m_showBackground ? (width - 8) * 0.5 * 0.38 : (width - 4) * 0.5 * 0.38;
    int boundingRadius = outerRadius + 2;
    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth = (width > 32) ? capsuleHeight * .23 : capsuleHeight * .35;
    int capsuleRadius = capsuleWidth / 2;

    if (m_showBackground)
    {
        p.setBrush(QColor(m_backgroundColor));
        p.setPen(QColor(m_backgroundColor));
        p.drawEllipse(rect().center(), boundingRadius, boundingRadius);
    }

    for (int i = 0; i < 18; i++)
    {
        QColor color = m_color;
        color.setAlphaF(qMax(1.0 - (i / 16.0), 0.));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i * 20.0f);
        p.drawRoundedRect(-capsuleWidth * 0.5, -(innerRadius + capsuleHeight), capsuleWidth, capsuleHeight,
                          capsuleRadius, capsuleRadius);
        p.restore();
    }

}
