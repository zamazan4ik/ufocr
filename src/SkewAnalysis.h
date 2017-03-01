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

#ifndef SKEWANALYSIS_H
#define SKEWANALYSIS_H

#include "ycommon.h"


class QPixmap;

class SkewAnalysis
{
public:
    SkewAnalysis(QPointList *pointList, int width, int height);
    ~SkewAnalysis();
    signed int getSkew();
    double getPhi();
    QPixmap drawTriangle(QPixmap &pm);
private:
    //int bin[360][2000];
    double getRightPhi();
    double getLeftPhi();
    QPointList *pointList;
    int m_height;
    int m_width;
    double phi;
    QPoint *p1;
    QPoint *p2;
};
#endif
