/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef UTILS_H
#define UTILS_H

#include <qglobal.h>

typedef struct _Rect
{
    qint32 x1, x2, y1, y2;
    int dotCount;
    int label;
} Rect;


class QString;
class QWidget;

QString extractFileName(const QString &path);
QString extractFilePath(const QString &path);
QString extractDigits(const QString &fn);
bool findProgram(const QString &name);
void styledWarningMessage(QWidget *parent, const QString &text);
void styledInfoMessage(QWidget *parent, const QString &text);
void styledCriticalMessage(QWidget *parent, const QString &text);
void qSleep(int msecs);
void clearTmpFiles();
inline bool _contains(qreal x1, qreal x2, qreal xc)
{
    return ((xc >= x1) && (xc <= x2)) || ((xc <= x1) && (xc >= x2));
}


#endif
