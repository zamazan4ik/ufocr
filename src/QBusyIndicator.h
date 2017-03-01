/*  This widget is based on QBusyIndicator by James Aleksans.

   YAGF - cuneiform and tesseract OCR graphical front-end
   Copyright (C) 2009-2015 Andrei Borovsky <borovsky.andrei@gmail.com>

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


#ifndef QBUSYINDICATOR_H
#define QBUSYINDICATOR_H

#include <QWidget>
#include <QColor>

class QBusyIndicator : public QWidget
{
Q_OBJECT
    Q_PROPERTY(bool showBackground
                       READ
                       getShowBackground
                       WRITE
                       setShowBackground)
    Q_PROPERTY(double speed
                       READ
                       getSpeed
                       WRITE
                       setSpeed)
    Q_PROPERTY(bool displayedWhenStopped
                       READ
                       isDisplayedWhenStopped
                       WRITE
                       setDisplayedWhenStopped)
    Q_PROPERTY(QColor color
                       READ
                       color
                       WRITE
                       setColor)
    Q_PROPERTY(QColor backgroundColor
                       READ
                       getBackgroundColor
                       WRITE
                       setBackgroundColor)
public:
    QBusyIndicator(QWidget* parent = 0);

    bool getShowBackground() const
    {
        return m_showBackground;
    }

    double getSpeed() const
    {
        return m_speed;
    }

    bool isAnimated() const;

    bool isDisplayedWhenStopped() const;

    const QColor& color() const
    {
        return m_color;
    }

    const QColor& getBackgroundColor() const
    {
        return m_backgroundColor;
    }

    virtual QSize sizeHint() const;

    int heightForWidth(int w) const;

public slots:

    void startAnimation();

    void stopAnimation();

    void setSpeed(double speed);

    void setDisplayedWhenStopped(bool state);

    void setColor(const QColor& color);

    void setShowBackground(bool state);

    void setBackgroundColor(const QColor& color);

protected:
    virtual void timerEvent(QTimerEvent* event);

    virtual void paintEvent(QPaintEvent* event);

private:
    bool m_showBackground;
    int m_angle;
    int m_timerId;
    double m_speed;
    bool m_displayedWhenStopped;
    QColor m_color;
    QColor m_backgroundColor;
};

#endif // QBUSYINDICATOR_H
