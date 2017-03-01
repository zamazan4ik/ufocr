/*
    YAGF - cuneiform and tesseract OCR graphical front-end
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

#ifndef CCBUILDER_H
#define CCBUILDER_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QList>
#include <QRect>

//#define DEBUG_CC

typedef QList<quint32> IntList;

/*

This class builds a 2-dimensional array for an image in which the connected pixels are labeled with the same numbers.
The elements of the resulting array may be accesed using the label() method.

*/

class CCBuilder : public QObject
{
    Q_OBJECT
public:

    /* pixmap is the image to be analysed */
    explicit CCBuilder(const QImage &img,  QObject *parent = 0);

    ~CCBuilder();

    /* This method resets CCs labels so that they occupy a continuos range of numbers */
    void compactLabels();

    /* This method returns the label value for original image's x and y. */
    quint32 label(int x, int y);

    /* This method performs the actual connected component's labeling */
    int labelCCs();

    /* Returns image width */
    int width();

    /* Returns image height */
    int height();

    QRect crop();

    quint32 * scanLine(int y);

    quint8 getMediumBWBG(quint32 *hist);

signals:

public slots:
private:
    void initialScan();
    void backwardScan();
    void forwardScan();
    int labelChecked(int x, int y);
    void setLabel(int x, int y, int newValue);
    void relabelLineLR(int y);
    void relabelLineRL(int y);
    inline bool isForeground(int x, QRgb value);
    void scanFirstLineLR();
    void scanLineLR(int y);
    void labelLeftmostPoint(int y);
    void labelRightmostPoint(int y);
private:
    quint32 thr1, thr2;
    QImage image;
    quint32 * labels;
    bool * flags;
    bool skipNext;
    IntList recolor;
    quint32 maxlabel;
    bool didRecolor;
    int w, h;
    QRect cropRect;
#ifdef DEBUG_CC
    int dcounter;
    int gcounter;
#endif
    friend class Cropper;
};

#endif // CCBUILDER_H
