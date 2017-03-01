/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009-2011 Andrei Borovsky <anb@symmetrica.net>

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

#include "ccbuilder.h"
#include "settings.h"
#include <QRgb>
#include <QRect>
#define XDEBUG
#ifdef XDEBUG
#include <QtDebug>
#endif

class Cropper
{
public:
    Cropper(CCBuilder * builder)
    {
        this->builder = builder;
        darksCount = 0;
        lightsCount = 0;
        minval = 0;
        maxval = 0;
        //clAltCount = 0;
        whitetr = 650;
        clBrighttoWidthtr = 0.1;
    }
    ~Cropper() {}
    QRect crop()
    {
        y1 = 0;
        int tolerance = 3;
        for (int y = 0; y < builder->h; y ++) {
            if (checkHorzLine(y)) {
                tolerance--;
                if (tolerance == 0) {
                    y1 = y;
                    break;
                }

            } else
                tolerance = 3;
        }
        y2 = builder->h-1;
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
        for (int x = x1; x < builder->w; x++) {
            if (checkVertLine(x)) {
                tolerance--;
                if (tolerance == 0) {
                    x1 = x;
                    break;
                }

            } else
                tolerance = 3;
        }
        x2 = builder->w-1;
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
    bool checkHorzLine(int y)
    {
        quint32 clWhiteCount = 0;
        darksCount = 0;
        lightsCount = 0;
        minval = 800;
        maxval = 0;
        whitesCount = 0;
        //clAltCount = 0;
        whiteAlt = 0;
        //int maxlstripe = 0;
        //int currentlstripe = 0;
        QRgb * line = (QRgb *) builder->image.scanLine(y);
        //int curline =0;
        for (int i = 0; i < builder->w; i++) {
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
        clBrighttoWidth = (qreal)lightsCount/builder->w;
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
    bool checkVertLine(int x)
    {
        quint32 clWhiteCount = 0;
        darksCount = 0;
        lightsCount = 0;
        minval = 800;
        maxval = 0;
        whitesCount = 0;
        //clAltCount = 0;
        whiteAlt = 0;
        //int curline =0;
        for (int y = 0; y < builder->h; y++) {
            QRgb * line = (QRgb *) builder->image.scanLine(y);
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
        clBrighttoWidth = (qreal)lightsCount/builder->h;
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
private:
    CCBuilder * builder;
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
    int y1, y2, x1, x2;
};

CCBuilder::CCBuilder(const QImage &img, QObject *parent) :
    QObject(parent)
{
    labels = NULL; //new quint32 [image.height()*image.width()];
    flags =  NULL; //new bool[image.height()];
    if (img.isNull()) {
        w = h = 0;
        return;
    }
    image = img;
    if (image.format() != QImage::Format_ARGB32)
        image.convertToFormat(QImage::Format_ARGB32);
    labels = NULL; //new quint32 [image.height()*image.width()];
    flags =  NULL; //new bool[image.height()];
    //memset(flags, 0, sizeof(bool)*image.height());
    skipNext = false;
    maxlabel = 1;
    w = image.width();
    h = image.height();
    thr1 = 0;
    thr2 = 0;
    for (int y = 64; y < img.height(); y++) {

        QRgb * line = (QRgb *) img.scanLine(y);
        for(int x = 32; x < img.width()/2; x++)
            thr1 = thr1 + (line[x]&0x000000FF);
        for(int x= img.width()/2; x < img.width()-32; x++)
            thr2 = thr2 + (line[x]&0x000000FF);
    }
    thr1 /= (w*h/2);
    thr2 /= (w*h/2);
}

CCBuilder::~CCBuilder()
{
    delete [] labels;
    delete [] flags;
}


int CCBuilder::width()
{
    return w;
}

int CCBuilder::height()
{
    return h;
}


void CCBuilder::compactLabels()
{
    QList<quint32> l;
    l.append(0);
    for( int i = 0; i < w; i++)
        for( int j = 0; j < h; j++)
            if (!l.contains(label(i,j)))
                l.append(label(i,j));
    for( int i = 0; i < w; i++)
        for( int j = 0; j < h; j++)
            setLabel(i, j, l.indexOf(label(i,j)));
}

quint32 CCBuilder::label(int x, int y)
{
    int index = y*w+x;
    if (index < w*h)
        return labels[index];
    return 0;
}

int CCBuilder::labelChecked(int x, int y)
{
    if ((x<0)||(y<0))
        return 0;
    if ((x>=w)||(y>=h))
        return 0;
    return labels[y*w+x];
}

void CCBuilder::setLabel(int x, int y, int newValue)
{
    recolor.replace(label(x, y), newValue);
    labels[y*w+x] = newValue;
    didRecolor = true;
}

void CCBuilder::relabelLineLR(int y)
{
    for (int x = 0; x < w; x++) {
        if (labelChecked(x, y)) {
            int oc = labelChecked(x, y);
            if (recolor.at(labelChecked(x-1, y))) {
                int nc = recolor.at(labelChecked(x-1, y));
                if (nc < oc) {
                    setLabel(x, y, nc);
                    oc = nc;
                }
                if (recolor.at(labelChecked(x+1, y-1))) {
                    int nc = recolor.at(labelChecked(x+1, y-1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
            } else {
                if (recolor.at(labelChecked(x, y-1))) {
                    int nc = recolor.at(labelChecked(x, y-1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
                if (recolor.at(labelChecked(x-1, y-1))) {
                    int nc = recolor.at(labelChecked(x-1, y-1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
                if (recolor.at(labelChecked(x+1, y-1))) {
                    int nc = recolor.at(labelChecked(x+1, y-1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
            }
        }
    }
}

void CCBuilder::relabelLineRL(int y)
{
    for (int x = w-1; x >= 0; x--) {
        if (labelChecked(x, y)) {
            int oc = labelChecked(x, y);
            if (recolor.at(labelChecked(x+1, y))) {
                int nc = recolor.at(labelChecked(x+1, y));
                if (nc < oc) {
                    setLabel(x, y, nc);
                    oc = nc;
                }
                if (recolor.at(labelChecked(x-1, y+1))) {
                    int nc = recolor.at(labelChecked(x-1, y+1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
            } else {
                if (recolor.at(labelChecked(x, y+1))) {
                    int nc = recolor.at(labelChecked(x, y+1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
                if (recolor.at(labelChecked(x+1, y+1))) {
                    int nc = recolor.at(labelChecked(x+1, y+1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
                if (recolor.at(labelChecked(x-1, y+1))) {
                    int nc = recolor.at(labelChecked(x-1, y+1));
                    if (nc < oc) {
                        setLabel(x, y, nc);
                        oc = nc;
                    }
                }
            }
        }
    }
}

bool CCBuilder::isForeground(int x, QRgb value)
{
    quint32 thr = x < image.width()/2 ? thr1 : thr2;
    return (value&0x000000FF) < thr;
}

void CCBuilder::initialScan()
{
    recolor.clear();
    recolor.append(0);
    scanFirstLineLR();
    for (int j = 1; j < h; j++) {
        labelLeftmostPoint(j);
        scanLineLR(j);
        labelRightmostPoint(j);
    }
}

void CCBuilder::backwardScan()
{
    bool rdid = false;
    didRecolor = false;
    relabelLineLR(h-1);
    rdid = rdid|didRecolor;
    skipNext = flags[h-1];
    flags[h-1] = !didRecolor;
    skipNext = skipNext&flags[h-1];
    for (int j = h-2; j >= 0; j--) {
        didRecolor = false;
        skipNext = skipNext&flags[j];
        if (!skipNext) {
            relabelLineRL(j);
            rdid = rdid|didRecolor;
        }
#ifdef DEBUG_CC
        else
            dcounter++;
        gcounter++;
#endif
        skipNext = flags[j];
        flags[j] = !didRecolor;
        skipNext = skipNext&flags[j];

    }
    didRecolor = rdid;
}

void CCBuilder::forwardScan()
{
    bool rdid = false;
    didRecolor = false;
    relabelLineLR(0);
    rdid = rdid|didRecolor;
    skipNext = flags[0];
    flags[0] = !didRecolor;
    skipNext = skipNext&flags[0];
    for (int j = 1; j < h; j++) {
        didRecolor = false;
        skipNext = skipNext&flags[j];
        if (!skipNext) {
            relabelLineLR(j);
            rdid = rdid|didRecolor;
        }
#ifdef DEBUG_CC
        else
            dcounter++;
        gcounter++;
#endif
        skipNext = flags[j];
        flags[j] = !didRecolor;
        skipNext = skipNext&flags[j];

    }
    didRecolor = rdid;
}

int CCBuilder::labelCCs()
{
    delete labels;
    delete flags;
    labels = 0;
    flags = 0;
    if (w*h == 0)
        return 0;
    labels = new quint32 [image.height()*image.width()];
    flags = new bool[image.height()];
    memset(flags, 0, sizeof(bool)*image.height());

#ifdef DEBUG_CC
    dcounter = 0;
    gcounter = 0;
#endif

    int count = 0;
    initialScan();
    didRecolor = true;
    int turn = 1;
    while (didRecolor) {
        didRecolor = false;
        if (turn)
            backwardScan();
        else
            forwardScan();
        turn = 1 - turn;
        count++;
    }

 #ifdef DEBUG_CC
    qDebug() << dcounter << gcounter;
#endif

    return count;
}

void CCBuilder::scanFirstLineLR()
{
    if (!(h*w)) return;
    QRgb * line = (QRgb *) image.scanLine(0);
    if (isForeground(0, line[0])) {
            labels[0] = maxlabel;
            recolor.append(maxlabel);
            maxlabel++;
    } else
        labels[0] = 0;
    for (int i = 1; i < w; i++) {
        if (isForeground(i, line[i])) {
            if (labels[i-1])
                labels[i] = labels[i-1];
            else {
                labels[i] = maxlabel;
                recolor.append(maxlabel);
                maxlabel++;
            }
        } else
            labels[i] = 0;
    }
}

void CCBuilder::labelLeftmostPoint(int y)
{
    QRgb * line = (QRgb *) image.scanLine(y);
    if (isForeground(0, line[0])) {
        int prlabel = labels[w*(y-1)];
        if (prlabel)
            labels[w*y] = prlabel;
        else {
            if ((w > 1) && (labels[w*(y-1) + 1] > 0)) {
                labels[w*y] = labels[w*(y-1) + 1];
            } else {
                labels[w*y] = maxlabel;
                recolor.append(maxlabel);
                maxlabel++;
            }
        }
    } else
        labels[w*y] = 0;
}

void CCBuilder::labelRightmostPoint(int y)
{
    if (w < 2) return;
    QRgb * line = (QRgb *) image.scanLine(y);
    if (isForeground(w-1, line[w-1])) {
        if (labels[w*y-1])
            labels[w*(y+1) - 1] = labels[w*y-1];
        else {
            if (labels[w*y-2]) {
                labels[w*(y+1) - 1] = labels[w*y-2];
            } else
                if (labels[w*(y+1)-2]) {
                    labels[w*(y+1) - 1] = labels[w*(y+1)-2];
                } else {
                 labels[w*(y+1) - 1] = maxlabel;
                recolor.append(maxlabel);
                maxlabel++;
            }
        }
    } else
        labels[w*(y+1) - 1] = 0;
}

quint8 CCBuilder::getMediumBWBG(quint32 * hist)
{
    quint32 acc = 0;
    quint32 count = 0;
    for (int y = 0; y < h; y ++) {
        quint8 * line = (quint8 *) image.scanLine(y);
        for(int x = 0; x < w; x++) {
            if (labels[w*y + x] == 0) {
                acc +=line[x*4];
                hist[line[x*4]]++;
                count++;
            }
        }
    }
    if (count != 0)
        return acc/count;
    return 255;
}



void CCBuilder::scanLineLR(int y)
{
    QRgb * line = (QRgb *) image.scanLine(y);
    for (int i = 1; i < w-1; i++) {
        labels[w*y + i] = 0;
        int nc;
        int oc;
        oc  = maxlabel;
        if (isForeground(i, line[i])) {
            nc = labels[w*(y - 1) + i];
            if (nc) {
                labels[w*y + i] = nc;
                oc = nc;
            }
            nc = labels[w*(y - 1) + i - 1];
            if ((nc) && (nc <= oc)) {
                labels[w*y + i] = nc;
                oc = nc;
            }
            nc = labels[w*(y - 1)  + i + 1];
            if ((nc) && (nc <= oc)) {
                labels[w*y + i] = nc;
                oc = nc;
            }
            nc = labels[w*y + i - 1];
            if ((nc) && (nc <= oc)) {
                labels[w*y + i] = nc;
                oc = nc;
            }
            if (!labels[w*y + i]) {
                labels[w*y + i] = maxlabel;
                recolor.append(maxlabel);
                maxlabel++;

            }
        }
    }
}

QRect CCBuilder::crop()
{
    Cropper cropper(this);
    return cropper.crop();
}

quint32 *CCBuilder::scanLine(int y)
{
    return &labels[w*y];
}

