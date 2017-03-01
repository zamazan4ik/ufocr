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

#include "BlockAnalysis.h"
#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QColor>
#include <math.h>

//#define DEBUG

BlockAnalysis::BlockAnalysis(QPixmap *pixmap)
{
    m_image = new QImage((pixmap->toImage().convertToFormat(QImage::Format_RGB32)));
    badImage = m_image->isNull();

    this->m_coords = new QRect(m_image->rect());
    m_blockMap = NULL;
}

BlockAnalysis::~BlockAnalysis()
{
    deleteBlock();
    delete m_image;
}

void BlockAnalysis::newBlock()
{
    /*deleteBlock();
    m_blockMap = new char * [m_coords->height() - m_coords->top()];
    for (int i = 0; i < m_coords->height() - m_coords->top(); i++)
        m_blockMap[i] = new char[m_coords->width() - m_coords->left()];
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        QRgb * line = (QRgb *) m_image->scanLine(i);
        for (int j = m_coords->left(); j < m_coords->width(); j++) {
            if ((qRed(line[j]) + qGreen(line[j]) + qBlue(line[j])) < treshold)
                m_blockMap[i-m_coords->top()][j-m_coords->left()] = 0;
            else
                m_blockMap[i-m_coords->top()][j-m_coords->left()] = 128;
        }

    }*/
}

void BlockAnalysis::deleteBlock()
{
    if (m_blockMap != NULL) {
        for (int i = 0; i < m_coords->height() - m_coords->top(); i++)
            delete m_blockMap[i];
        delete m_blockMap;
        m_blockMap = NULL;
    }
}

void BlockAnalysis::countLinesInImg(int factor, int d)
{

    int ftmp = factor;
    if (m_image->isNull())
        return;
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        int l = i;
        linesInfo[i] = 0;
        ftmp = factor;
        for (int j = m_coords->left(); j < m_coords->width(); j++) {
            int k = qRed(lines[l][j]) + qGreen(lines[l][j]) + qBlue(lines[l][j]);
            if (k >= mtreshold - 1) {
                linesInfo[i]++;
#ifdef DEBUG
                if (d != 0)
                    m_image->setPixel(j, l, (1 << 32) + 1);
#endif

            } else
                break;
            ftmp--;
            if (ftmp == 0) {
                ftmp = factor;
                l -= d;
                if ((l < m_coords->top()) || (l == m_coords->height()))
                    break;
            }
        }
    }
    longestLine[2] = longestLine[1];
    longestLine[1] = longestLine[0];
    longestLine[0] = linesInfo[0];
    for (int i = m_coords->top(); i < m_coords->height() ; i++)
        if (linesInfo[i] > longestLine[0])
            longestLine[0] = linesInfo[i];
    longestCount[2] = longestCount[1];
    longestCount[1] = longestCount[0];
    longestCount[0] = 0;
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        if (longestLine[0] - linesInfo[i] < 5)
            longestCount[0]++;
    }

#ifdef DEBUG
    QImage im = m_image->copy(*m_coords);
    im.save("/home/andrei/ttt.jpg", "JPEG");
#endif

}

void BlockAnalysis::countLinesInImg(QImage *_image)
{
    mtreshold = 0;
    linesInfo = new quint16[_image->rect().height() + _image->rect().top()];
    for (int i = 0; i < _image->rect().height() + _image->rect().top(); i++)
        linesInfo[i] = 0;
    lines = new QRgb * [_image->rect().height() + _image->rect().top()];

    for (int i = _image->rect().top() + _image->rect().height() / 8; i < _image->rect().height() - _image->rect().height() / 8; i++) {
        QRgb *line = (QRgb *)(m_image->scanLine(i));
        lines[i] = line;
        for (int j = _image->rect().left() + _image->rect().width() / 8; j < _image->rect().width() - _image->rect().width() / 8; j++) {
            mtreshold = mtreshold + qRed(line[j]) + qGreen(line[j]) + qBlue(line[j]);
        }
    }
    int div = (_image->rect().height() - _image->rect().top()) * (_image->rect().width() - _image->rect().left());
    mtreshold = mtreshold / div;


    if (_image->isNull())
        return;
    for (int i = _image->rect().top() + _image->rect().height() / 8; i < _image->rect().height() - _image->rect().height() / 8; i++) {
        for (int j = _image->rect().left() + _image->rect().width() / 8; j < _image->rect().width() - _image->rect().width() / 8; j++) {
            int k = qRed(lines[i][j]) + qGreen(lines[i][j]) + qBlue(lines[i][j]);
            if (k >= mtreshold - 1) {
                linesInfo[i]++;
#ifdef DEBUG
                if (d != 0)
                    m_image->setPixel(j, l, (1 << 32) + 1);
#endif

            } else
                break;
        }
    }
    longestLine[2] = longestLine[1];
    longestLine[1] = longestLine[0];
    longestLine[0] = linesInfo[0];
    for (int i = _image->rect().top(); i < _image->rect().height() ; i++)
        if (linesInfo[i] > longestLine[0])
            longestLine[0] = linesInfo[i];
    longestCount[2] = longestCount[1];
    longestCount[1] = longestCount[0];
    longestCount[0] = 0;
    for (int i = _image->rect().top(); i < _image->rect().height(); i++) {
        if (longestLine[0] - linesInfo[i] < 5)
            longestCount[0]++;
    }

#ifdef DEBUG
    QImage im = m_image->copy(*m_coords);
    im.save("/home/andrei/ttt.jpg", "JPEG");
#endif
    delete lines;
    delete linesInfo;
}


int BlockAnalysis::getSkew2()
{
    int ll[7], lc[7];
    for (int i = 0; i < 7; i++) {
        QMatrix m;
        m.rotate((i - 3) * 2);
        QImage img = m_image->transformed(m, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32);
        img = img.copy(img.rect());
        countLinesInImg(&img);
        ll[i] = longestLine[0];
        lc[i] = longestCount[0];
    }
    int imax = 0;
    for (int i = 0; i < 7; i++) {
        if ((ll[i] > ll[imax]) && (lc[i] > lc[imax])) {
            imax = i;
        }
    }
    int result = (imax - 3) * 2;
    return result;
}

int BlockAnalysis::getSkew1()
{
    createLinesInfo();
    preScan1();
    int ll, lc;
    int ll1, lc1, div1, dir1;
    int ll2, lc2, div2, dir2;

    if (m_coords->width() > 1024)
        m_coords->setWidth(1024);
    if (m_coords->height() > m_coords->width() / 2)
        m_coords->setHeight(m_coords->width() / 2);
    countLinesInImg(m_coords->width(), 0);
    ll = longestLine[0];
    lc = longestCount[0];
    int div = 2;
    int direction = 1;
    countLinesInImg(m_coords->width() / div, direction);
    int safe = 0;
    while ((longestLine[0] >= longestLine[1]) && (longestCount[0] >= longestCount[1]) && (safe < 512)) {
        div++;
        countLinesInImg(m_coords->width() / div, direction);
        safe++;
    }
    ll1 = longestLine[0];
    lc1 = longestCount[0];
    div1 = div;
    dir1 = direction;
    longestLine[0] = ll;
    longestCount[0] = lc;
    div = 2;
    direction = - direction;
    countLinesInImg(m_coords->width() / div, direction);
    safe = 0;
    while ((longestLine[0] >= longestLine[1]) && (longestCount[0] >= longestCount[1]) && (safe < 512)) {
        div++;
        countLinesInImg(m_coords->width() / div, direction);
        safe++;
    }
    ll2 = longestLine[0];
    lc2 = longestCount[0];
    div2 = div;
    dir2 = direction;
    int rdir;
    int rdiv;
    if ((ll1 > ll2) && (lc1 > lc2)) {
        rdir = dir1;
        rdiv = div1;
    } else {
        rdir = dir2;
        rdiv = div2;
    }
    float result = atan(((float)(rdiv - 1)) / ((float)m_coords->width()) * rdir) / M_2_PI * 360;
    QMatrix m;
    m.rotate(result);
    QImage *tmp = m_image;
    m_image = new QImage(tmp->transformed(m, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32));
    delete tmp;
    tmp = m_image;
    m_image = new QImage(tmp->scaled(QSize(tmp->width(), tmp->height()) * 1.1).convertToFormat(QImage::Format_RGB32));
    delete tmp;
    *m_coords = m_image->rect();
    delete linesInfo;
    delete lines;
    createLinesInfo();
    preScan1();
    countLinesInImg(m_coords->width(), 0);
    if ((ll > longestLine[0]) && (lc > longestCount[0])) {
        result = 0;
    }
//   if (((float)longestLine[0])/((float)m_coords->width()) < 0.8)
//      result = 0;

    delete linesInfo;
    delete lines;
    return result;
}

int BlockAnalysis::getSkew()
{
    createLinesInfo();
    preScan1();
    int ll, lc;
    if (m_coords->width() > 1536)
        m_coords->setWidth(1536);
    if (m_coords->height() > m_coords->width() / 2)
        m_coords->setHeight(m_coords->width() / 2);
    countLinesInImg(m_coords->width(), 0);
    ll = longestLine[0];
    lc = longestCount[0];
    countLinesInImg(m_coords->width() / 4, -1);
    countLinesInImg(m_coords->width() / 4, 1);
    int maxRes = 0;
    if ((longestLine[1] >= longestLine[maxRes]) && (longestCount[1] > longestCount[maxRes]))
        maxRes = 1;
    if ((longestLine[2] >= longestLine[maxRes]) && (longestCount[2] > longestCount[maxRes]))
        maxRes = 2;
    int direction = 0;
    if (maxRes == 0) direction = 1;
    else if (maxRes == 1) direction = -1;
    else
        return 0;

    int div = 4;
    countLinesInImg(m_coords->width() / div, direction);
    int safe = 0;
    while ((longestLine[0] >= longestLine[1]) && (longestCount[0] >= longestCount[1]) && (safe < 512)) {
        div++;
        countLinesInImg(m_coords->width() / div, direction);
        direction = - direction;
        safe++;
    }
    float result = atan(((float)(div - 1)) / ((float)m_coords->width()) * (direction)) / M_2_PI * 360;
    QMatrix m;
    m.rotate(result);
    QImage *tmp = m_image;
    m_image = new QImage(tmp->transformed(m, Qt::SmoothTransformation).convertToFormat(QImage::Format_RGB32));
    *m_coords = m_image->rect();
    delete linesInfo;
    delete lines;
    delete tmp;
    createLinesInfo();
    preScan1();
    countLinesInImg(m_coords->width(), 0);
    if ((ll > longestLine[0]) && (lc > longestCount[0]))
        result = 0;
    if (((float)longestLine[0]) / ((float)m_coords->width()) < 0.8)
        result = 0;

    delete linesInfo;
    delete lines;
    return result;
}

void BlockAnalysis::preScan1()
{
    mtreshold = 0;
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        QRgb *line = (QRgb *)(m_image->scanLine(i));
        lines[i] = line;
        for (int j = m_coords->left(); j < m_coords->width(); j++) {
            mtreshold = mtreshold + qRed(line[j]) + qGreen(line[j]) + qBlue(line[j]);
        }
    }
    int div = (m_coords->height() - m_coords->top()) * (m_coords->width() - m_coords->left());
    mtreshold = mtreshold / div;
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        //qint16 pixel1 = qRed(lines[i][m_coords->left()]) + qGreen(lines[i][m_coords->left()]) + qBlue(lines[i][m_coords->left()]);
        //qint16 pixel2 = qRed(lines[i][m_coords->left() + 1]) + qGreen(lines[i][m_coords->left() + 1]) + qBlue(lines[i][m_coords->left() + 1]);
        for (int j = m_coords->left() + 2; j < m_coords->width(); j++) {
            qint16 pixel3 = qRed(lines[i][j]) + qGreen(lines[i][j]) + qBlue(lines[i][j]);
            if (pixel3 >= mtreshold) {
                lines[i][j-1] = lines[i][j];
                lines[i][j-2] = lines[i][j];
            }
            //pixel1 = pixel2;
            //pixel2 = pixel3;
        }
        lines[i][m_coords->width()-1] = lines[i][m_coords->width()-2];
    }

    for (int i = m_coords->height() - 1; i >= 0; i--) {
        if (longestLine[0] - linesInfo[i] > 2) {
            m_coords->setHeight(i + 1);
            break;
        }
    }
    for (int i = m_coords->top(); i < m_coords->height(); i++) {
        if (longestLine[0] - linesInfo[i] > 2) {
            m_coords->setTop(i);
            break;
        }
    }

}

void BlockAnalysis::createLinesInfo()
{
    linesInfo = new quint16[m_coords->height() + m_coords->top()];
    for (int i = 0; i < m_coords->height() + m_coords->top(); i++)
        linesInfo[i] = 0;
    lines = new QRgb * [m_coords->height() + m_coords->top()];
}
