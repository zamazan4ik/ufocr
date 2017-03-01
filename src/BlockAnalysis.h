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

#ifndef BLOCKANALYSIS_H
#define BLOCKANALYSIS_H

#include <QtCore>
#include <QColor>
class QPixmap;
class QImage;
class QRect;

class BlockAnalysis
{
public:
    BlockAnalysis(QPixmap *pixmap);
    ~BlockAnalysis();
    int getSkew();
    int getSkew1();
    int getSkew2();
    QRect getCoords();
    QPixmap getPixmap();
private:
    QImage *m_image;
    char **m_blockMap;
    int longestLine[3];
    int longestCount[3];
    QRect *m_coords;
    qint64 mtreshold;
    quint16 *linesInfo;
    QRgb **lines;

    void newBlock();
    void deleteBlock();
    void countLinesInImg(int factor, int d);
    void countLinesInImg(QImage *_image);
    void preScan1();
    void deleteLines();
    bool badImage;
    void createLinesInfo();
};

#endif // BLOCKANALYSIS_H
