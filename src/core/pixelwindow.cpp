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

#include "pixelwindow.h"
PixelWindow::PixelWindow(QIPBlackAndWhiteImage * image, int x, int y, int width, int height)
{
        img = image;
        gsimg = 0;
        xStart = x;
        yStart = y;
        w = width;
        h = height;
        iw = image->width();
        lines = new  quint8 * [h];
        int yt = yStart;
        while ((yt < 0)&&(yt - yStart < h)) {
            lines[yt - yStart] = 0;
            yt++;
        }
        while (yt + h < (int) img->height()) {
            lines[yt] = img->scanLine(yt - yStart);
        }
        while (yt - yStart < h) {
            lines[yt - yStart] = 0;
            yt++;
        }
}

PixelWindow::PixelWindow(QIPGrayscaleImage *image, int x, int y, int width, int height)
{
    img = 0;
    gsimg = image;
    xStart = x;
    yStart = y;
    w = width;
    h = height;
    iw = image->width();
    lines = new quint8 * [h];
    int yt = yStart;
    while ((yt < 0)&&(yt - yStart < h)) {
        lines[yt - yStart] = 0;
        yt++;
    }
    while (yt + h < gsimg->height()) {
        lines[yt] = gsimg->scanLine(yt - yStart);
    }
    while (yt - yStart < h) {
        lines[yt - yStart] = 0;
        yt++;
    }
}

PixelWindow::~PixelWindow()
{
    delete[] lines;
}

void PixelWindow::move(int newX, int newY)
{
    xStart = newX;
    yStart = newY;
    delete[] lines;
    lines = new quint8 * [h];
    int yt = yStart;
    while ((yt < 0)&&(yt - yStart < h)) {
        lines[yt - yStart] = 0;
        yt++;
    }
    if (gsimg == 0) {
        while (yt + h < (int) img->height()) {
            lines[yt] = img->scanLine(yt - yStart);
        }
    } else {
        while (yt + h < gsimg->height()) {
            lines[yt] = gsimg->scanLine(yt - yStart);
        }
   }
    while (yt - yStart < h) {
        lines[yt - yStart] = 0;
        yt++;
    }
}

quint8 PixelWindow::pixel(int x, int y)
{
    int yt = yStart - y;
    if (lines[yt] == 0)
        return 255;
    if (x < 0) return 255;
    if ((x < xStart)||(x+xStart >= iw))
            return 255;
    return lines[yt][xStart+x];
}


