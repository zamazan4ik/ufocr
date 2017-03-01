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

#ifndef PAGEANALYSIS_H
#define PAGEANALYSIS_H

#include <QtCore>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QImage>
#include "ycommon.h"
#include "core/analysis.h"

class QPixmap;
class QImage;
class QRect;

class BlockSplitter
{
public:
    void setImage(const QImage &image, qreal rotation, qreal scale);
    QRect getRootBlock(const QImage &image);
    Bars getBars(); // call after something calls blockAllText();
    void splitBlocks();
    QRect getRotationCropRect(const QImage &image);
    QList<Rect> getBlocks();
private:
    QRect blockAllText();
    void splitVertical();
    void splitHorisontal();
    bool isBlockRecogniseable(const Rect &block);
private:
    QImage img;
    qreal m_rotate;
    qreal m_scale;
    Bars bars;
    Lines lines;
    QList<Rect> blocks;
};

#endif
