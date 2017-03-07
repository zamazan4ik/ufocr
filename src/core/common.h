/*
   UFOCR - User-Friendly OCR
   Copyright (C) 2017 Alexander Zaitsev <zamazan4ik@tut.by>

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

#ifndef COMMON_H
#define COMMON_H


#include <QtGlobal>

template<typename T_>
void deallocator(T_ buf[])
{
    delete[] buf;
}

typedef struct IntRect
{
    int x1, y1, x2, y2;
} _IntRect;


//#define IPRIT_MULTITHREADING

#ifdef IPRIT_MULTITHREADING
#include <QtConcurrentRun>
#include <QFuture>
#endif


#endif // COMMON_H
