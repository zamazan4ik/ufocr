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

#ifndef QSNIPPET_H
#define QSNIPPET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QRect>
#include <QPixmap>

class QSnippet : public QListWidgetItem
{
public:
    explicit QSnippet(QListWidget *parent = 0);
    bool setPage(int id, const QString &name, const QImage &image = QImage());
    QString getName();
    int pageID();
    int imageHeight();
    int imageWidth();
signals:
public slots:
private:
    QString name;
    int pid;
    int ih;
    int iw
    ;
};

#endif // QSNIPPET_H
