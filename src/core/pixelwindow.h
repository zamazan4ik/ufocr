/*
   YAGF - cuneiform and tesseract OCR graphical front-end
   Copyright (C) 2009-2014 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef PIXELWINDOW_H
#define PIXELWINDOW_H

#include "qipblackandwhiteimage.h"
#include "qipgrayscaleimage.h"

class PixelWindow
{
public:
    PixelWindow(QIPBlackAndWhiteImage *image, int x, int y, int width, int height);
    PixelWindow(QIPGrayscaleImage * image, int x, int y, int width, int height);
    ~PixelWindow();
    void move(int newX, int newY);
    quint8 pixel(int x, int y);
private:
    QIPBlackAndWhiteImage * img;
    QIPGrayscaleImage * gsimg;
    int xStart;
    int yStart;
    int w;
    int h;
    int iw;
    quint8 ** lines;
    quint8 * blankLine;
};

#endif // PIXELWINDOW_H
