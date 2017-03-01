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

#include "qsnippet.h"

QSnippet::QSnippet(QListWidget *parent) :
    QListWidgetItem(parent)
{
}

bool QSnippet::setPage(int id, const QString &name, const QImage &image)
{
    if (image.isNull()) {
        QImage img(name);
        if (img.isNull())
            return false;
        setIcon(QPixmap::fromImage(img));
    } else
        setIcon(QPixmap::fromImage(image));
    this->name = name;
    setToolTip(name);
    pid = id;
    ih = image.height();
    iw = image.width();
    return true;
}

QString QSnippet::getName()
{
    return name;
}

int QSnippet::pageID()
{
    return pid;
}

int QSnippet::imageHeight()
{
    return ih;
}

int QSnippet::imageWidth()
{
    return iw;
}


