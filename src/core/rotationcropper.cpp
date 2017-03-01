/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2010 Andrei Borovsky <anb@symmetrica.net>

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

#include "rotationcropper.h"
#include <QImage>
#include <QColor>

RotationCropper::RotationCropper(QImage * image, QRgb color)
{
    this->image = image;
    darksCount = 0;
    lightsCount = 0;
    minval = 0;
    maxval = 0;
    //clAltCount =
    whitetr = 650;
    clBrighttoWidthtr = 0.1;
    //QRgb * line = (QRgb *) image->scanLine(0);
    whitePixel = color;
    //replaceColor = color;
}

RotationCropper::~RotationCropper() {}

QRect RotationCropper::crop()
{
    y1 = 0;
    int tolerance = 3;
    recolor();
    for (int y = 0; y < image->height(); y ++) {
        if (checkHorzLine(y)) {
            tolerance--;
            if (tolerance == 0) {
                y1 = y;
                break;
            }

        } else
            tolerance = 3;
    }
    y2 = image->height()-1;
    tolerance = 3;
    for (int y = y2; y >= 0; y--) {
        if (checkHorzLine(y)) {
            tolerance--;
            if (tolerance == 0) {
                y2 = y;
                break;
            }

        } else
            tolerance = 3;
    }
    x1 = 0;
    tolerance = 3;
    for (int x = x1; x < image->width(); x++) {
        if (checkVertLine(x)) {
            tolerance--;
            if (tolerance == 0) {
                x1 = x;
                break;
            }

        } else
            tolerance = 3;
    }
    x2 = image->width()-1;
    tolerance = 3;
    for (int x = x2; x >= 0; x--) {
        if (checkVertLine(x)) {
            tolerance--;
            if (tolerance == 0) {
                x2 = x;
                break;
            }

        } else
            tolerance = 3;
    }
    return QRect(x1, y1, x2-x1, y2-y1);
}

void RotationCropper::recolor()
{
    QColor repColor(1,2,3);
    for (int y = 0; y < image->height(); y++) {
        QRgb * line = (QRgb *) image->scanLine(y);
        for (int x = 0; x < image->width(); x++) {
            if (line[x] == whitePixel )
                line[x] = repColor.rgb();
            else break;

        }
        for (int x = image->width() - 1; x >= 0; x--) {
            if (line[x] == whitePixel )
                line[x] = repColor.rgb();
            else break;

        }
    }
}

bool RotationCropper::checkHorzLine(int y)
{
    int skipCount = 0;
    darksCount = 0;
    lightsCount = 0;
    minval = 800;
    maxval = 0;
    whitesCount = 0;
    //clAltCount = 0;
    whiteAlt = 0;
    //int maxlstripe = 0;
    //int currentlstripe = 0;
    QRgb * line = (QRgb *) image->scanLine(y);
    for (int i = 0; i < image->width(); i++) {
        if ((qRed(line[i]) == 1) && (qGreen(line[i]) == 2) && (qBlue(line[i]) == 3)) {
            skipCount++;
            continue;
        }
        int pixel = qRed(line[i]) + qGreen(line[i]) + qBlue(line[i]);
        if (pixel <= 382) {
            darksCount++;
//             if (currentlstripe > maxlstripe) maxlstripe = currentlstripe;
//             currentlstripe = 0;
       } else {
            lightsCount++;
//               currentlstripe++;
            if (pixel >= whitetr) {
                whitesCount++;
                if (whiteAlt == 0) {
                    whiteAlt = 1;
                    clWhiteCount++;
                }
            } else
                whiteAlt = 0;
        }
        minval = pixel < minval ? pixel : minval;
        maxval = pixel > maxval ? pixel : maxval;
    }
    qreal d = image->width() - skipCount;
    if (d == 0)
        return false;
    clBrighttoWidth = (qreal)lightsCount/d;
    if (clBrighttoWidth <= clBrighttoWidthtr)
        return false;
    if (maxval - minval < 40)
        return false;
    //if (clAltCount < 10)
      //  return false;
    //if (maxlstripe < 10)
    //    return false;
   // if ((clWhiteCount > 0)&&(clWhiteCount < 3))
     //       return false;
    return true;
}

bool RotationCropper::checkVertLine(int x)
{
    int skipCount = 0;
    darksCount = 0;
    lightsCount = 0;
    minval = 800;
    maxval = 0;
    whitesCount = 0;
    //clAltCount = 0;
    whiteAlt = 0;
    //int curline =0;
    for (int y = 0; y < image->height(); y++) {
        QRgb * line = (QRgb *) image->scanLine(y);
        if ((qRed(line[x]) == 1) && (qGreen(line[x]) == 2) && (qBlue(line[x]) == 3)) {
            skipCount++;
            continue;
        }
        int pixel = qRed(line[x]) + qGreen(line[x]) + qBlue(line[x]);
        if (pixel <= 382) {
            darksCount++;
//                if (curline) {
//                    clAltCount++;
//                    curline = 0;
//                }
        } else {
            lightsCount++;
//                if (!curline) {
//                    clAltCount++;
//                    curline = 1;
//                }
            if (pixel >= whitetr) {
                whitesCount++;
                if (whiteAlt == 0) {
                    whiteAlt = 1;
                    clWhiteCount++;
                }
            } else
                whiteAlt = 0;
        }
        minval = pixel < minval ? pixel : minval;
        maxval = pixel > maxval ? pixel : maxval;
    }
    qreal d = image->height() - skipCount;
    if (d == 0)
        return false;
    clBrighttoWidth = (qreal)lightsCount/d;
    if (clBrighttoWidth <= clBrighttoWidthtr)
        return false;
    if (maxval - minval < 40)
        return false;
//        if (clAltCount < 10)
//            return false;
    if ((clWhiteCount > 0)&&(clWhiteCount < 3))
            return false;
    return true;
}
