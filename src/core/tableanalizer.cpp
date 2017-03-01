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

#include "tableanalizer.h"
#include "qipblackandwhiteimage.h"
#include "ccbuilder.h"
#include "settings.h"
#include "utils.h"
#include "pixelwindow.h"
#include <math.h>

enum State {
    Start,
    InLine,
    AfterLine
};
const quint8 clBWWhite = 1;
const quint8 clBWBlack = 0;

TableAnalizer::TableAnalizer(QIPBlackAndWhiteImage *image) : img(image), blocks()
{
}

QList<Rect> TableAnalizer::splitTable(const QRect &bounds)
{

    if (!findRect(bounds))
        return blocks;
   // clearImage();
    //img->toImage().save("0000A.png");
    int cLabel = maxRect.label + 1;
    for (int y = maxRect.y1+1; y < maxRect.y2; y++) {
        quint8 * line = img->scanLine(y);
        quint8 * prevLine = img->scanLine(y - 1);
        for (int x = maxRect.x1+1; x < maxRect.x2-1; x++) {
            if (line[x] == clBWWhite) {
                if (line[x-1] > maxRect.label) {
                    line[x] = line[x-1];
                } else {
                    if (prevLine[x] > maxRect.label) {
                        line[x] = prevLine[x];
                    } else {
                        if (prevLine[x+1] > maxRect.label) {
                            line[x] = prevLine[x+1];
                        } else {
                            if (prevLine[x-1] > maxRect.label) {
                                line[x] = prevLine[x-1];
                            } else {
                                cLabel++;
                                line[x] = cLabel;
                            }
                        }
                    }
                }
                updateComponents(x, y, line[x]);
            } else {

            }
        }
    }
    for (int i = blocks.count() - 1; i > -1; i--)
        if (blocks.at(i).label == maxRect.label) {
            blocks.removeAt(i);
            break;
        }
    clearGarbage();
    img = 0;
    return sortBlocks(blocks);
}

QList<Rect> TableAnalizer::splitTableForce(const QRect &bounds)
{
    if (!findRect(bounds))
        return blocks;
    blocks.clear();
    addBars(bounds);
    clearGarbage();
    return sortBlocks(blocks);
}

Rect TableAnalizer::getSkew(const QRect &bounds)
{
    findRect(bounds);
    Rect result = {0,0,0,0};
    quint8 * line = img->scanLine(maxRect.y1);
    for (int x = maxRect.x1; x <= maxRect.x2; x++) {
        if (line[x] == clBWBlack) {
            result.x1 = x;
            result.y1 = maxRect.y1;
            break;
        }
    }
    if (maxRect.x2 - result.x1 > 200) {
        for (int y = maxRect.y1; y <= maxRect.y2; y++) {
            quint8 * line = img->scanLine(y);
            if (line[maxRect.x2] == clBWBlack) {
                result.x2 = maxRect.x2;
                result.y2 = y;
                break;
            }
        }
    } else {
       result.x2 = result.x1;
       result.y2 = result.y1;
       for (int y = maxRect.y1; y <= maxRect.y2; y++) {
           quint8 * line = img->scanLine(y);
           if (line[maxRect.x1] == clBWBlack) {
               result.x1 = maxRect.x1;
               result.y1 = y;
               break;
           }
       }
    }
    float dx = result.x2 - result.x1;
    if (dx == 0) {
        result.y1 = result.y2;
        return result;
    }
    float dy = result.y2 - result.y1;
    float angle = abs(-atan(dy/dx)*360/6.283*100);
    if ((angle < 10)||(angle > 200)) {
        result.y1 = result.y2;
        return result;
    }
    return result;
}

bool TableAnalizer::findRect(const QRect &bounds)
{
  //  connectLine(bounds);
    CCBuilder ccbuilder(img->toImage());
    ccbuilder.labelCCs();
    for (int y = bounds.y(); y < bounds.y() + bounds.height(); y++) {
        quint32 * line = ccbuilder.scanLine(y);
        for (int x = bounds.x(); x < bounds.x()+ bounds.width(); x++) {
            if (line[x] > 0)
                updateComponents(x, y, line[x]);
        }
    }
    bool result = findMax(bounds);
    for (int y = bounds.y(); y < bounds.y() + bounds.height(); y++) {
        quint32 * line = ccbuilder.scanLine(y);
        quint8 * out = img->scanLine(y);
        for (int x = bounds.x(); x < bounds.x() + bounds.width(); x++) {
            if (line[x] == (quint32) maxRect.label)
                out[x] = clBWBlack;
            else
                out[x] = clBWWhite;
        }

    }
  //  img->toImage().save("/home/parallels/A0.png");
    return result;
}

void TableAnalizer::updateComponents(int x, int y, int label)
{
    Rect r;
    for (int i = 0; i < blocks.count(); i++) {
        if (blocks.at(i).label == label) {
            r = blocks.at(i);
            if (x < r.x1)
                r.x1 = x;
            if (y < r.y1)
                r.y1 = y;
            if (x > r.x2)
                r.x2 = x;
            if (y > r.y2)
                r.y2 = y;
            r.dotCount++;
            blocks.removeAt(i);
            blocks.prepend(r);
            return;
        }
    }
    r.x1 = x;
    r.y1 = y;
    r.x2 =x;
    r.y2 =y;
    r.dotCount = 1;
    r.label = label;
    blocks.prepend(r);
}

bool TableAnalizer::findMax(const QRect &bounds)
{
    /*quint8 colorMax = clBWWhite > clBWBlack ? clBWWhite : clBWBlack;
    for (int y = 0; y < img->height(); y++) {
        quint8 * line = img->scanLine(y);
        for (int x = 0; x < img->width(); x++) {
            if (line[x] > colorMax )
                line[x] = clBWBlack;
        }
    }*/
    if (blocks.count() == 0)
        return false;
    maxRect = blocks.at(0);
    for (int i = 1; i < blocks.count(); i++) {
        Rect r = blocks.at(i);
        if ((r.x2 - r.x1)*(r.y2 -r.y1) > (maxRect.x2- maxRect.x1)*(maxRect.y2 - maxRect.y1))
            if (r.dotCount > 200)
                maxRect = r;
    }
    blocks.clear();
    if ((maxRect.x2- maxRect.x1)*(maxRect.y2 - maxRect.y1) > bounds.width()*bounds.height()/4)
        return true;
    return false;
}

void TableAnalizer::clearImage()
{
    for (quint32 y = 0; y < img->height(); y++) {
        quint8 * line = img->scanLine(y);
        for (quint32 x = 0; x < img->width(); x++) {
            quint8 color = line[x];
            if (color != maxRect.label)
                line[x] = clBWWhite;
            else
                line[x] = clBWBlack;
        }
    }
  /*  for (int y = 0; y < 200; y++) {
        quint8 * line = img->scanLine(y);
        for (int x = 0; x < 200; x++) {
            line[x] = clBWBlack;
        }
    }*/
    img->toImage().save("/home/parallels/A0.png");
}

bool __contains(Rect &r1, Rect &r2)
{
    if ((r2.x1 >= r1.x1)&&(r2.y1 >= r1.y1))
        if ((r2.x2 <= r1.x2)&&(r2.y2 <= r1.y2))
            return true;
    return false;
}
bool __intersects(Rect &r1, Rect &r2)
{
    if ((r2.x1 >= r1.x1)&&(r2.y1 >= r1.y1))
        if ((r2.x1 <= r1.x2)&&(r2.y1 <= r1.y2))
            return true;
    return false;

}

bool __intersects2(Rect &r1, Rect &r2)
{
    if((r2.x1 < r1.x1-1)&&(r2.x2 > r1.x1+1))
        if ((r2.y1 > r1.y1)&&(r2.y2 < r1.y2))
                return true;
    if((r2.x1 < r1.x2-1)&&(r2.x2 > r1.x2+1))
        if ((r2.y1 > r1.y1)&&(r2.y2 < r1.y2))
                return true;

   /* if((r2.y1 < r1.y2-1)&&(r2.y2 > r1.y2+1))
        if ((r2.x1 > r1.x1)&&(r2.x2 < r1.x2))
                return true;
    if((r2.y1 < r1.y1-1)&&(r2.y2 > r1.y1+1))
        if ((r2.x1 > r1.x1)&&(r2.x2 < r1.x2))
                return true;*/
    return false;
}


void TableAnalizer::clearGarbage()
{
    for (int i = blocks.count()-1; i > -1; i--) {
        Rect r = blocks.at(i);
        if ((r.x2-r.x1)*(r.y2-r.y1)<150)
            blocks.removeAt(i);
        else {
            if ((r.x2-r.x1) < 15)
                blocks.removeAt(i);
            else {
                if ((r.y2-r.y1) < 15)
                    blocks.removeAt(i);
            }
        }
    }
    int c = blocks.count();
    int i = 0;
    while(i < c) {
        Rect r = blocks.at(i);
        for (int j = i+1; j < c; j++) {
            Rect r1 = blocks.at(j);
            if (__contains(r, r1)) {
                blocks.removeAt(j);
                c--;
                i--;
                break;
            }
            if (__contains(r1, r)) {
                blocks.removeAt(i);
                c--;
                i--;
                break;
            }
            if (__intersects(r, r1)){
                blocks.removeAt(i);
                c--;
                i--;
                break;
            }
            if (__intersects(r1, r)){
                blocks.removeAt(i);
                c--;
                i--;
                break;
            }
           /* if (__intersects2(r1, r)) {
                blocks.removeAt(i);
                c--;
                i--;
                break;
            }
            if (__intersects2(r, r1)) {
                blocks.removeAt(i);
                c--;
                i--;
                break;
            }*/

        }
        i++;
    }
}

void TableAnalizer::drawHorzLine(int y, int minx, int maxx)
{
    quint8 * line = img->scanLine(y);
    for (int x = minx; x < maxx; x++)
        line[x] = clBWBlack;

}

void TableAnalizer::drawVertLine(int x, int miny, int maxy)
{
    for (int y = miny; y < maxy; y++) {
        quint8 * line = img->scanLine(y);
        line[x] = clBWBlack;
    }

}


const int stride = 100;
const int thr = 90;



void TableAnalizer::addBars(const QRect &bounds)
{
    for (int y = maxRect.y1; y < maxRect.y2; y++) {
        quint8 * line = img->scanLine(y);
        for (int x = maxRect.x1 + 1; x < maxRect.x2 -1; x++)
            if ((line[x-1] == clBWBlack)&&(line[x+1] == clBWBlack))
                    line[x] = clBWBlack;
    }
    for (int y = maxRect.y1+1; y < maxRect.y1-1; y++) {
        quint8 * l1 = img->scanLine(y-1);
        quint8 * l2 = img->scanLine(y);
        quint8 * l3 = img->scanLine(y+1);
        for(int x = maxRect.x1; x < maxRect.x2; x++) {
            if ((l1[x]==clBWBlack)&&l3[x]==clBWBlack)
                l2[x] = clBWBlack;
        }
    }
    drawHorzLine(maxRect.y1+1, bounds.x(), bounds.x() + bounds.width());
    drawHorzLine(maxRect.y2-1, bounds.x(), bounds.x() + bounds.width());
    drawVertLine(maxRect.x1+1, bounds.y(), bounds.y() + bounds.height());
    drawVertLine(maxRect.x2-1, bounds.y(), bounds.y() + bounds.height());
    int prevHLine = maxRect.y1 + 1;
    int currentHLine = maxRect.y2-1;
    for (int y = maxRect.y1+5; y < maxRect.y2-5; y++) {
        quint8 * pprevLine = img->scanLine(y-2);
        quint8 * prevLine = img->scanLine(y-1);
        quint8 * nextLine = img->scanLine(y+1);
        quint8 * nnextLine = img->scanLine(y+2);
        quint8 * line = img->scanLine(y);

        int count = 0;
        for (int x = bounds.x(); x < bounds.x() + bounds.width(); x++) {
            if (count >= 150) {
                drawHorzLine(y, bounds.x(), bounds.x() + bounds.width());
                Rect r;
                r.x1 = maxRect.x1;
                r.x2 = maxRect.x2;
                r.y1 = prevHLine;
                r.y2 = y;
                blocks.append(r);
                prevHLine = y+2;
                y+=8;
                break;
            }
            if ((line[x] == clBWBlack)||(prevLine[x] == clBWBlack)||(pprevLine[x] == clBWBlack)||(nextLine[x] == clBWBlack)||(nnextLine[x] == clBWBlack))
                count++;
            else {
                    count = 0;
            }
        }

    }

    Rect r;
    r.x1 = maxRect.x1;
    r.x2 = maxRect.x2;
    r.y1 = prevHLine;
    r.y2 = currentHLine;
    blocks.append(r);
    clearGarbage();

    for (int x = maxRect.x1+3; x < maxRect.x2-3; x++) {
        int count = 0;
        for (int y = bounds.y(); y < bounds.y() + bounds.height(); y++) {
            quint8 * line = img->scanLine(y);
            if (count >= 150) {
                drawVertLine(x, bounds.y(), bounds.y() + bounds.height());
                int bc = blocks.count();
                for (int i = 0; i < bc;) {
                    if ((x > blocks.at(i).x1)&&(x < blocks.at(i).x2)) {
                        Rect r = blocks.at(i);
                        Rect r1 = r;
                        Rect r2 = r;
                        r1.x2 = x-1;
                        r2.x1 = x+1;
                        blocks.removeAt(i);
                        blocks.append(r1);
                        blocks.append(r2);
                        bc--;
                    } else
                        i++;
                }
                x +=8;
                break;
            }
            if (line[x] == clBWBlack)
                count++;
            else {
                if ((line[x-1] != clBWBlack)&&(line[x+1] != clBWBlack))
                    count = 0;
                else
                    count++;

            }
        }
    }
}



QList<Rect> TableAnalizer::sortBlocks(const QList<Rect> &blocks)
{
    if (blocks.count() < 3)
        return blocks;
    QList<Rect> tmp = blocks;
    QList<Rect> out;
    while(tmp.count()>1) {
        int ymid;
        ymid = (tmp.at(0).y1 + tmp.at(1).y2)/2;
        out.append(tmp.at(0));
        tmp.removeFirst();
        for (int i = tmp.count()-1; i >= 0; i--) {
            if (_contains(tmp.at(i).y1, tmp.at(i).y2, ymid)) {
                bool appeneded = false;
                for (int j = 0; j < out.count(); j++) {
                    if (tmp.at(i).y2 <= out.at(j).y1) {
                        appeneded = true;
                        out.insert(j, tmp.at(i));
                        tmp.removeAt(i);
                        break;
                    }
                }
                if (!appeneded) {
                    out.append(tmp.at(i));
                    tmp.removeAt(i);
                }
            }
        }
    }
    return out;
}

Rect TableAnalizer::getSkew2(const QRect &bounds)
{
    int xb = -1;
    int yb;
    Rect er = {0,0,0,0};
    quint8 * line = img->scanLine(maxRect.y1);
    for (int x = maxRect.x1; x <= maxRect.x2; x++) {
        if (line[x] == clBWBlack) {
            xb = x;
            yb = maxRect.y1;
            break;
        }
    }
    if (xb == -1)
        return er;
    PixelWindow   * pw = new PixelWindow(img, xb, yb+5, 160, 11);
    int yt = 0;
    while((pw->pixel(xb, yb - yt)&&(yt <= 5)) == clBWBlack) {
        yt++;
    }
    yt--;
    if (pw->pixel(xb, yb + yt) != clBWBlack)
        return er;
    pw->move(xb, yb - yt + 5);
    int vcounter = 0;
    int xe = xb + 1;
    while ((abs(vcounter) < 5)&&(xe-xb<200)) {
        if (pw->pixel(xe, yt) == clBWBlack) {
            if (pw->pixel(xe, yt-1)==clBWBlack) {
                yt--;
                vcounter--;
            }
        } else {
            if (pw->pixel(xe, yt) == clBWWhite) {
                yt++;
                vcounter++;
            } else break;
        }
        xe++;
    }
    delete pw;
    Rect result;
    result.x1 = xb;
    result.x2 = xe;
    result.y1 = 0;
    result.y2 = vcounter;
    return result;
}
