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

#include "pageanalysis.h"
#include "rotationcropper.h"
#include "ccbuilder.h"
#include "analysis.h"
#include "utils.h"
#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QColor>
#include <QList>


void BlockSplitter::setImage(const QImage &image, qreal rotation, qreal scale)
{
    img = image;
    m_rotate = rotation;
    m_scale = scale;
}

QRect BlockSplitter::getRootBlock(const QImage &image)
{
    if (image.isNull())
        return QRect(0,0,0,0);
    QImage img1 = image;
    QRect result = blockAllText();
    RotationCropper rc(&img1, QColor("white").rgb());
    //  CCBuilder cb(img1);
    //img1.save("/home/anre/pictures/ttt.png");
    //QRect r = cb.crop();//rc.crop();
    QRect r1(result.x(), result.y(), result.width(), result.height());
    result = r1;
    /*foreach (Rect rc, bars) {
        bars.removeOne(rc);
        rc.x1 += r.x();
        rc.x2 += r.x();
        rc.y1 += r.y();
        rc.y2 += r.y();
        bars.append(rc);
    }*/

    return result;
}

Bars BlockSplitter::getBars()
{
    return bars;
}

QRect BlockSplitter::blockAllText()
{
    //img = img.transformed(QTransform().translate(-x, -y).rotate(m_rotate).translate(x, y), Qt::SmoothTransformation);
    if (!img.isNull()) {
        CCBuilder *cb = new CCBuilder(img);
        cb->labelCCs();
        CCAnalysis *an = new CCAnalysis(cb);
        an->analize();
        lines = an->getLines();
        foreach(TextLine l, lines)
        if (l.count() < 3)
            lines.removeOne(l);
        int minX = 100000;
        int minY = 100000;
        int maxX = 0;
        int maxY = 0;
        for (int i =0; i < lines.count(); i++) {
            int x1 = lines.at(i).at(0).x;
            int y1 = lines.at(i).at(0).y;
            int x2 = lines.at(i).at(lines.at(i).count()-1).x;
            int y2 = lines.at(i).at(lines.at(i).count()-1).y;
            //graphicsInput->drawLine(x1,y1,x2,y2);
            if (x1 > x2) {
                int t = x2;
                x2 = x1;
                x1 = t;
            }
            minX = minX < x1 ? minX : x1;
            maxX = maxX > x2 ? maxX : x2;
            if (y1 > y2) {
                int t = y2;
                y2 = y1;
                y1 = t;
            }
            minY = minY < y1 ? minY : y1;
            maxY = maxY > y2 ? maxY : y2;
        }
        minX = minX  - 2*an->getMediumGlyphWidth();
        maxX =maxX + 2*an->getMediumGlyphWidth();
        minY = minY - 2*an->getMediumGlyphHeight();
        maxY = maxY + 2*an->getMediumGlyphHeight();
        //graphicsInput->clearBlocks();
        //graphicsInput->addBlock(QRectF(ox + minX*2*sideBar->getScale(), oy + minY*2*sideBar->getScale(), (maxX-minX)*2*sideBar->getScale(), (maxY-minY)*2*sideBar->getScale()));

        bars = an->addBars();

        delete an;
        delete cb;
        minX = minX <= 0 ? 1 :minX;
        minY = minY <= 0 ? 1 :minY;
        return QRect(minX*m_scale, minY*m_scale, (maxX-minX)*m_scale, (maxY-minY)*m_scale);
    }
    return QRect(0, 0, 0, 0);
}

void BlockSplitter::splitVertical()
{
    bool didSplit = true;
    while (didSplit) {
        didSplit = false;
        for (int i = blocks.count() - 1; i >=0; i--) {
            Rect block = blocks.at(i);
            foreach(Rect bar, bars) {
                if (abs(bar.x2 - bar.x1) > (bar.y2-bar.y1))
                    continue;
                int xmid = (bar.x1 + bar.x2)/2;
                if ((block.x1 < (xmid - 5)) &&(block.x2 > (xmid + 5))) {
                    Rect block1 = block;
                    block1.x2 = xmid -2;
                    Rect block2 = block;
                    block2.x1 = xmid + 2;
                    blocks.removeAll(block);
                    blocks.append(block1);
                    blocks.append(block2);
                    didSplit = true;
                    break;
                }
            }
        }
    }
}



void BlockSplitter::splitBlocks()
{
    QRect r = getRootBlock(img);
    Rect b;
    b.x1 = r.x();
    b.y1 = r.y();
    b.x2 = b.x1 + r.width();
    b.y2 = b.y1 + r.height();
    blocks.clear();
    blocks.append(b);
    splitVertical();
    splitHorisontal();


    for (int i = blocks.count() -1; i >=0; i--) {
        Rect r  = blocks.at(i);
        if (!isBlockRecogniseable(r))
            blocks.removeAll(r);
    }

    /*CCBuilder ccb(img);
    ccb.labelCCs();
    CCAnalysis cca(&ccb);
    cca.tableAnalise();
    Rect r1  = cca.getTableRect();
    cca.markTableBlocks(b);
    QList<Rect> bl = cca.extractTableBlocks(b);
    foreach (Rect r1, bl) {
        blocks.append(r1);
    }*/




    //int i = bl.count();
}

QList<Rect> BlockSplitter::getBlocks()
{
    return blocks;
}

QRect BlockSplitter::getRotationCropRect(const QImage &image)
{
    const QImage *img = &image;
    QImage *img2 = const_cast<QImage *>(img);
    RotationCropper rc(img2, QColor("white").rgb());
    return rc.crop();
}

void BlockSplitter::splitHorisontal()
{
    bool didSplit = true;
    while (didSplit) {
        didSplit = false;
        for (int i = blocks.count() - 1; i >=0; i--) {
            Rect block = blocks.at(i);
            foreach(Rect bar, bars) {
                if (abs(bar.y2 - bar.y1) > (bar.x2-bar.x1))
                    continue;
                int ymid = (bar.y1 + bar.y2)/2;
                if ((block.y1 < (ymid - 5)) &&(block.y2 > (ymid + 5)))
                    if (_contains(bar.x1, bar.x2, block.x1)||_contains(bar.x1, bar.x2, block.x2)||_contains(block.x1, block.x2, bar.x1)||_contains(block.x1, block.x2, bar.x2)) {
                        Rect block1 = block;
                        block1.y2 = ymid -2;
                        Rect block2 = block;
                        block2.y1 = ymid + 2;
                        blocks.removeAll(block);
                        blocks.append(block1);
                        blocks.append(block2);
                        didSplit = true;
                        break;
                    }
            }
        }
    }
}

inline bool between(int a, int b, int c)
{
    return (c > a) && (c < b);

}

bool BlockSplitter::isBlockRecogniseable(const Rect &block)
{
    int contains = 0;
    int maxl = 0;
    foreach(TextLine l, lines) {
        if (!between(block.x1, block.x2, l.first().x))
            continue;
        if (!between(block.x1, block.x2, l.last().x))
            continue;
        if (!between(block.y1, block.y2, l.first().y))
            continue;
        if (!between(block.y1, block.y2, l.last().y))
            continue;
        contains++;
        if (l.count() > maxl) maxl = l.count();
    }
    if ((maxl > 3)) return true;
    return false;
}

