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

#ifndef TABLEANALIZER_H
#define TABLEANALIZER_H

#include "../utils.h"
#include <QList>
#include <QRect>

class QIPBlackAndWhiteImage;

class TableAnalizer
{
public:
    TableAnalizer(QIPBlackAndWhiteImage* image);

    QList <Rect> splitTable(const QRect& bounds);

    QList <Rect> splitTableForce(const QRect& bounds);

private:
    QIPBlackAndWhiteImage* img;
    Rect maxRect;
    QList <Rect> blocks;
private:
    bool findRect(const QRect& bounds);

    void updateComponents(int x, int y, int label);

    bool findMax(const QRect& bounds);

    void clearImage();

    void clearGarbage();

    void drawHorzLine(int y, int minx, int maxx);

    void drawVertLine(int x, int miny, int maxy);

    void addBars(const QRect& bounds);

    bool findHorzLine(qint32 y, qint32 xstart, qint32 xstop, int count);
};

#endif // TABLEANALIZER_H
