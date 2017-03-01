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

#ifndef YCOMMMON_H
#define YCOMMMON_H
#include <QList>
#include <QPoint>
#include <QRect>
#include <QRectF>

typedef QList<QPoint> QPointList;

inline QRect QRectF2Rect(const QRectF &rf)
{
    QRect r;
    r.setX(rf.x());
    r.setY(rf.y());
    r.setWidth(rf.width());
    r.setHeight(rf.height());
    return r;
}




#endif
