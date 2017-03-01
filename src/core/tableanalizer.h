/*
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

#ifndef TABLEANALIZER_H
#define TABLEANALIZER_H

#include "../utils.h"
#include <QList>
#include <QRect>


class QIPBlackAndWhiteImage;


class TableAnalizer
{
public:
    TableAnalizer(QIPBlackAndWhiteImage *image);
    QList<Rect> splitTable(const QRect &bounds);
    QList<Rect> splitTableForce(const QRect &bounds);
    Rect getSkew(const QRect &bounds);
private:
    QIPBlackAndWhiteImage * img;
    Rect maxRect;
    QList<Rect> blocks;
private:
    bool findRect(const QRect &bounds);
    void updateComponents(int x, int y, int label);
    bool findMax(const QRect &bounds);
    void clearImage();
    void clearGarbage();
    void drawHorzLine(int y, int minx, int maxx);
    void drawVertLine(int x, int miny, int maxy);
    void addBars(const QRect &bounds);
    QList<Rect> sortBlocks(const QList<Rect> &blocks);
    Rect getSkew2(const QRect &bounds);
};

#endif // TABLEANALIZER_H
