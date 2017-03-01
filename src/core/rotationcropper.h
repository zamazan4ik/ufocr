/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2012 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef ROTATIONCROPPER_H
#define ROTATIONCROPPER_H

#include <QRgb>
#include <QRect>
#include "ycommon.h"

class QImage;

class RotationCropper
{
public:
    RotationCropper(QImage * image, QRgb color);
    ~RotationCropper();
    QRect crop();
private:
    void recolor();
    bool checkHorzLine(int y);
    bool checkVertLine(int x);

private:
    QImage * image;
    QRgb whitePixel;
    QRgb replaceColor;
    int darksCount;
    int lightsCount;
    int minval;
    int maxval;
    //int clAltCount;
    int whitesCount;
    int whitetr;
    int whiteAlt;

    qreal clBrighttoWidth;
    qreal clBrighttoWidthtr;
    int clWhiteCount;
    int y1, y2, x1, x2;
};

#endif
