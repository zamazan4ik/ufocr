/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
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

#ifndef DROPLABEL_H
#define DROPLABEL_H
#include <QLabel>

class QListWidget;

class DropLabel : public QLabel
{
    Q_OBJECT
public:
    DropLabel(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setListWidget(QListWidget *w);
signals:
    void pageRemoved(int id);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
private:
    QListWidget *lw;
};

#endif // DROPLABEL_H
