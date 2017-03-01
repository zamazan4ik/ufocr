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

#include "sidebar.h"
#include "qsnippet.h"
#include <QStringList>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFileInfo>
#include <QDrag>
#include <QSize>


SideBar::SideBar(QWidget *parent) :
    QListWidget(parent)
{
    //setDragDropOverwriteMode(true);
    current = 0;
    setMaximumWidth(120);
    setMinimumWidth(120);
    connect(this, SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)), this, SLOT(itemActive(QListWidgetItem *,QListWidgetItem *)));
    setToolTip(trUtf8("Drop files here"));
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    lock = false;
    dragging = false;
}

void SideBar::addItem(QSnippet *item)
{
    setAlternatingRowColors(false);
    QSize size= item->sizeHint();
    size.setWidth(width());
    if (item->imageWidth() == 0)
        return;
    int h = item->imageHeight()*100/item->imageWidth();
    if (h > width()+2) h = width()+2;

    size.setHeight(item->imageHeight() < width() ? item->imageHeight() + 2 : h + 2);
    item->setSizeHint(size);
    QListWidget::addItem(item);
    setCurrentItem(item);
}


void SideBar::itemActive(QListWidgetItem *item, QListWidgetItem *item2)
{
    if (lock) return;
    lock = true;
    if (item) {
        emit pageSelected(((QSnippet *)item)->pageID());
        current = ((QSnippet *)item);
    } else current = 0;
    lock = false;
}

/*void SideBar::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasUrls()) {
        setCursor(Qt::ForbiddenCursor);
        event->ignore();
    //     >mimeData()->setData("application/x-qabstractitemmodeldatalist", QByteArray());
    }
    else {
        setCursor(Qt::DragCopyCursor);
        event->accept();
    }
    QListWidget::dragEnterEvent(event);
}*/

QStringList SideBar::mimeTypes() const
{
    QStringList qstrList;
    qstrList.append("text/uri-list");
    return qstrList;
}

Qt::DropActions SideBar::supportedDropActions() const
{
    if (dragging)
        return 0;
    return Qt::CopyAction | Qt::MoveAction;
}

bool SideBar::dropMimeData(int index, const QMimeData *data, Qt::DropAction action)
{
//   if (action == Qt::MoveAction)
//       return false;
    QList <QUrl> urlList;
    urlList = data->urls(); // retrieve list of urls
    QStringList files;
    foreach(QUrl url, urlList) { // iterate over list
        files.append(url.toLocalFile());
        ++index; // increment index to preserve drop order
    }
    emit filesDropped(files);
    return true;
}

void SideBar::startDrag(Qt::DropActions supportedActions)
{
    dragging = true;
    supportedActions |= Qt::MoveAction;
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urlList;
    QStringList sl;
    foreach(QListWidgetItem * lwi, selectedItems()) {
        QString s = QString::number(((QSnippet *)lwi)->pageID());
        sl << s;
    }
    mimeData->setUrls(urlList);
    drag->setMimeData(mimeData);
    if (drag->exec(supportedActions,Qt::CopyAction) == Qt::CopyAction) {
        foreach(QListWidgetItem * lwi, selectedItems()) {
            emit fileRemoved(((QSnippet *) lwi)->pageID());
            model()->removeRow(row(lwi));
        }
    }
    current = 0;
    dragging = false;
}

QSnippet *SideBar::getItemByName(const QString &name)
{
    for (int i = 0; i < count(); i++) {
        if (((QSnippet *)item(i))->getName() == name)
            return ((QSnippet *)item(i));
    }
    return NULL;
}

void SideBar::select(const QString &name)
{
    current = getItemByName(name);
    if (current)
        current->setSelected(true);
}

void SideBar::selectFirstFile()
{
    if (count() == 0) {
        current = NULL;
        return;
    }
    current = (QSnippet *) item(0);
}

/*void SideBar::dragLeaveEvent(QDragLeaveEvent *event)
{
    setCursor(Qt::ArrowCursor);
    event->accept();
    //QListWidget::dragLeaveEvent(event);
}

void SideBar::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> ul = event->mimeData()->urls();
        QList<QUrl>::Iterator i;
        for (i = ul.begin(); i != ul.end(); i++) {
            QUrl url = *i;
            this->addFile(url.toLocalFile());
        }
    }
    setCursor(Qt::ArrowCursor);
    event->accept();
    //QListWidget::dropEvent(event);
}*/
