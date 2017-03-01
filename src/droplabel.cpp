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

#include "droplabel.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QUrl>
#include <QStandardItemModel>
#include <QtDebug>
#include <QListWidget>
#include <QList>

DropLabel::DropLabel(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{
    setAcceptDrops(true);
}

void DropLabel::dragEnterEvent(QDragEnterEvent *event)
{
    //event->setDropAction(Qt::IgnoreAction);
    const QMimeData *md = event->mimeData();
    QStringList sl = md->formats();
    if (!md->formats().contains("text/uri-list")) {
        setCursor(Qt::ForbiddenCursor);
    } else {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        //QCursor cur(lw->selectedItems().at(0)->icon().pixmap(96, 128));
        //setCursor(cur);
    }

}

void DropLabel::dragLeaveEvent(QDragLeaveEvent *event)
{
    setCursor(Qt::ArrowCursor);
    event->accept();
}

void DropLabel::dropEvent(QDropEvent *event)
{
    if (event->dropAction() == Qt::MoveAction)
        event->ignore();
    else
        event->accept();
}

void DropLabel::setListWidget(QListWidget *w)
{
    lw = 0;
}
