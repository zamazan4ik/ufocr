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

#include "SkewAnalysis.h"
#include <math.h>
#include <stdio.h>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QColor>

SkewAnalysis::SkewAnalysis(QPointList *pointList, int width, int height)
{
    /* for (int i = 0; i < 360; i++)
         for (int j = 0; j < 2000; j++)
             bin[i][j] = 0;
     for (int i = 0; i < 360; i++) {
         for (int j = 0; j < pointList->count(); j+=2) {
             long int r = pointList->at(j).x()*cos(i/360*M_2_PI) + pointList->at(j).y()*cos(i/360*M_2_PI);
             r = r/4;
             bin[i][r]++;
         }
     }
     long int sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
      int phi1 = 0,  phi2 =  0, phi3 = 0, phi4 = 0;
     long int tmpsum = 0;
     //for (int i = 0; i < 360; i++)
     //    for (int j = 0; j < 2000; j++)
     //            if (bin[i][j] < 9) bin[i][j] = 0;
     for (int i = 0; i < 360; i++) {
         for (int j = 0; j < 2000; j++)
            // tmpsum += bin[i][j];
             if (tmpsum < bin[i][j]) tmpsum = bin[i][j];
         if ((tmpsum > sum4) || (tmpsum > sum3) || (tmpsum > sum2 || (tmpsum > sum1))) {
             if (sum1 < sum2) {sum1 = sum2; phi1 = phi2;}
             if (sum2 < sum3) {sum2 = sum3; phi2 = phi3;}
             if (sum3 < sum4) {sum3 = sum4; phi3 = phi4;}
             sum4 = tmpsum;
             phi4 = i;
             tmpsum =0;
         }
     }
     FILE * f = fopen("/home/andrei/hough.txt", "w");
     for (int i = 0; i < 360; i++) {
         for (int j = 0; j < 1000; j++)
         fprintf(f, "%i:, %i, ", i, bin[i][j]);
         fprintf(f, "\n");
     }
     fclose(f);*/
    this->pointList = pointList;
    m_width = width;
    m_height = height;
    p1 = new QPoint();
    p2 = new QPoint();

}

SkewAnalysis::~SkewAnalysis()
{
    delete p1;
    delete p2;

}

signed int SkewAnalysis::getSkew()
{
    int minLeftDist = 10000;
    int minRightDist = 0;
    for (int i = 0; i < pointList->count(); i++) {
        if (!(i%2)) {
            if (minLeftDist > pointList->at(i).x())
                minLeftDist = pointList->at(i).x();
        } else {
            if (minRightDist < pointList->at(i).x())
                minRightDist = pointList->at(i).x();
        }
    }
    minRightDist = m_width - minRightDist;

    signed int res;
    if (minRightDist > minLeftDist) {
        phi = getRightPhi();
    } else {
        phi = getLeftPhi();
    }
    res = (phi/(2*M_PI))*360+1;
    if (res > 45) return 90 - res;
    if (res < 45) return -(90 + res);
    return -res;
}

double SkewAnalysis::getPhi()
{
    return getSkew()*2*M_PI/(double)360;
    //if (phi > 0.7854) return M_PI_2 - phi;
    //if (phi < 0.7854) return -(M_PI_2 + phi);
}

double SkewAnalysis::getRightPhi()
{
    double phi = 0;
    int maxx = 0, maxy = 0, minx = 10000, miny  = 10000;
    for (int i = 1; i < pointList->count(); i +=2) {
        if (maxx< pointList->at(i).x()) {
            maxx = pointList->at(i).x();
            maxy = pointList->at(i).y();
        }
        if (miny > pointList->at(i).y()) {
            minx = pointList->at(i).x();
            miny = pointList->at(i).y();
        }
    }
    phi = 0;
    if (maxy-miny < m_height - maxy) {
        miny = 0;
        for (int i = 1; i < pointList->count(); i +=2)
            if (miny < pointList->at(i).y()) {
                minx = pointList->at(i).x();
                miny = pointList->at(i).y();
            }
    } else  {
        miny = 10000;
        for (int i = 1; i < pointList->count(); i +=2)
            if (miny > pointList->at(i).y()) {
                minx = pointList->at(i).x();
                miny = pointList->at(i).y();
            }
    }

    if (maxx-minx == 0)
        return 0;
    if (m_height/abs(maxx-minx) >=100 )
        return 0;
    // if (maxy > miny)
    p1->setX(maxx);
    p1->setY(maxy);
    p2->setX(minx);
    p2->setY(miny);

    phi = atan((double)(maxy-miny)/(double)(maxx-minx));
    //else
    //phi = atan((double)(miny - maxy)/(double)(maxx-minx));
    // phi = - (M_PI_2 -phi);
    return phi;
}


double SkewAnalysis::getLeftPhi()
{
    double phi = 0;
    int maxx = 0, maxy = 0, minx = 10000, miny  = 10000;
    for (int i = 0; i < pointList->count(); i +=2) {
        if (minx > pointList->at(i).x()) {
            minx = pointList->at(i).x();
            miny = pointList->at(i).y();
        }
        if (maxy > pointList->at(i).y()) {
            maxx = pointList->at(i).x();
            maxy = pointList->at(i).y();
        }
    }
    phi = 0;
    if (miny < m_height - miny) {
        maxy = 0;
        for (int i = 0; i < pointList->count(); i +=2)
            if ((maxy < pointList->at(i).y()) && (pointList->at(i).y() < 0.9*m_height)) {
                maxx = pointList->at(i).x();
                maxy = pointList->at(i).y();
            }
    } else  {
        maxy = 10000;
        for (int i = 0; i < pointList->count(); i +=2)
            if ((maxy > pointList->at(i).y()) && (pointList->at(i).y() < 0.9*m_height)) {
                maxx = pointList->at(i).x();
                maxy = pointList->at(i).y();
            }
    }

    p1->setX(maxx);
    p1->setY(maxy);
    p2->setX(minx);
    p2->setY(miny);

    if (maxx-minx == 0)
        return 0;
    if (m_height/abs(maxx-minx) >=100 )
        return 0;
    phi = atan((double)(maxy-miny)/(double)(maxx-minx));
    return phi;
}

QPixmap SkewAnalysis::drawTriangle(QPixmap &pm)
{
    QImage img = pm.toImage();
    QPainter p(&img);
    QPen pen;
    pen.setColor(QColor(255, 0, 0));
    p.setPen(pen);
    p.drawLine(*p1, *p2);
    // p.
    return QPixmap::fromImage(img);
}
