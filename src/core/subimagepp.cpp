/*
   YAGF - cuneiform and tesseract OCR graphical front-end
   Copyright (C) 2009-2014 Andrei Borovsky <anb@symmetrica.net>

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

#include "subimagepp.h"
#include "settings.h"
#include <QSet>

SubimagePP::SubimagePP(QImage &image) :
img(image), ccbuilder(image)
{
    ccbuilder.labelCCs();
    quint32 hist[256] = {0};
    quint8 c = ccbuilder.getMediumBWBG(hist);
    color = 0xFF << 24;
    color = color + (c << 16) + (c << 8) + c;

    for (int i = 0; i < 256; i++) {
            if (hist[i]) {
                darkestBG = hist[i];
                break;
            }
    }

    /*for (int i = 0; i < 256; i++) {
        if (hist[i]) {
            for (int j = i + 16; j < 256; j++)
                hist[j] = c;
            break;
        }
    }
    for (int y = 0; y < img.height(); y ++) {
        quint32 * line = (quint32 *) img.scanLine(y);
        quint32 * lineCC = (quint32 *) ccbuilder.scanLine(y);

        for(int x = 0; x < img.width(); x++) {
            if (lineCC[x] == 0)  {
                if (hist[line[x]&0x000000FF] == c)
                    line[x] = color;
            }
        }
    }*/
}

SubimagePP::~SubimagePP()
{
    components.clear();
}

void SubimagePP::fillComponents()
{
    CCAnalysis an(&ccbuilder);
    an.analize(true);
    mgw = an.getMediumGlyphWidth();
    mgh = an.getMediumGlyphHeight();
    bars = an.getBars();
    components = an.getAllComponents(false);
}

void SubimagePP::removeBars()
{
    QSet<quint32> barLabels;
    foreach (Rect r, bars) {
        barLabels.insert(r.label);
    }
    labelsToBackground(barLabels);
}

void SubimagePP::removeNoise()
{
    int sf = Settings::instance()->getSkipWidth();
    sf *= sf;
    QSet<quint32> noiseLabels;
    foreach (Rect r, components) {
        if (r.dotCount < 3)
            noiseLabels.insert(r.label);
        if ((r.x1 == r.x2) || (r.y1 == r.y2) )
            noiseLabels.insert(r.label);
        if (abs((r.x2-r.x1)*(r.y2-r.y1)) >= mgw*mgw*sf)
            noiseLabels.insert(r.label);
        if ((r.y1 == 0)||(r.y2 == img.height()-1))
            if (r.y2 - r.y1 < mgh/2)
                noiseLabels.insert(r.label);
        if ((r.x1 == 0)||(r.x2 == img.width()-1)) {
                    if (r.y2 - r.y1 < mgh/2)
                        noiseLabels.insert(r.label);
                    if (r.y2 - r.y1 > 2*mgh)
                        noiseLabels.insert(r.label);
        }
    }
    labelsToBackground(noiseLabels);
}

void SubimagePP::stregthen()
{
    quint8 c2 = (darkestBG&0x000000FF)/2;
    quint32 darker = 0xFF + (c2 << 16) +(c2 << 8) +c2;
    for (int y = 2; y < img.height()-2; y ++) {
        quint32 * linel2 = (quint32 *) img.scanLine(y-2);
        quint32 * lineg2 = (quint32 *) img.scanLine(y+2);
        quint32 * linel = (quint32 *) img.scanLine(y-1);
        quint32 * line = (quint32 *) img.scanLine(y);
        quint32 * lineg = (quint32 *) img.scanLine(y+1);
        quint32 * lineCC = (quint32 *) ccbuilder.scanLine(y);
        for(int x = 1; x < img.width()-1; x++) {
            if ((lineCC[x] != 0)&&(line[x] >= darkestBG)) {
                if ((linel[x] >= darkestBG)&&(lineg[x] >= darkestBG))
                    if ((linel2[x] >= darkestBG)&&(lineg2[x] >= darkestBG))
                   // if ((line[x-1] >= darkestBG)&&(line[x+1] >= darkestBG))
                        line[x] = darker;
            }
        }
    }
}

void SubimagePP::labelsToBackground(QSet<quint32> &labels)
{
    for (int y = 0; y < img.height(); y ++) {
        quint32 * line = (quint32 *) img.scanLine(y);
        quint32 * lineCC = (quint32 *) ccbuilder.scanLine(y);

        for(int x = 0; x < img.width(); x++) {
            if (labels.contains(lineCC[x])) {
                line[x] = color;
                //lineCC[x] = 0;
            }
        }
    }
}
