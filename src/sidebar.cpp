/*
   UFOCR - User-Friendly OCR
   Copyright (C) 2017 Alexander Zaitsev <zamazan4ik@tut.by>

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
#include <QMimeData>
#include <QMenu>


SideBar::SideBar(QWidget* parent) :
        QListWidget(parent)
{
    //setDragDropOverwriteMode(true);
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QObject * )),
            this, SLOT(deleteFile(QObject * )));
    current = 0;
    setMaximumWidth(120);
    setMinimumWidth(120);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(
                                 const QPoint &)),
            SLOT(showContextMenuForWidget(
                         const QPoint &)));
    connect(this, SIGNAL(currentItemChanged(QListWidgetItem * , QListWidgetItem * )), this,
            SLOT(itemActive(QListWidgetItem * , QListWidgetItem * )));
    setToolTip(trUtf8("Drop files here"));
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    lock = false;
    dragging = false;
}

void SideBar::addItem(Snippet* item)
{
    setAlternatingRowColors(false);
    QSize size = item->sizeHint();
    size.setWidth(width());
    if (item->imageWidth() == 0)
    {
        return;
    }
    int h = item->imageHeight() * 100 / item->imageWidth();
    if (h > width() + 2)
    { h = width() + 2; }

    size.setHeight(item->imageHeight() < width() ? item->imageHeight() + 2 : h + 2);
    item->setSizeHint(size);
    QListWidget::addItem(item);
    setCurrentItem(item);
}

void SideBar::deleteItem(Snippet* item)
{
    /*item->
    QListWidget::(item);
    setCurrentItem(item);*/
}

void SideBar::itemActive(QListWidgetItem* item, QListWidgetItem* item2)
{
    if (lock)
    {
        return;
    }
    lock = true;
    if (item)
    {
        emit pageSelected(((Snippet*) item)->pageID());
        current = ((Snippet*) item);
    }
    else
    {
        current = 0;
    }
    lock = false;
}

void SideBar::showContextMenuForWidget(const QPoint& pos)
{
    QMenu contextMenu(trUtf8("Context menu"), this);
    QListWidgetItem* item = this->itemAt(pos);
    if (!item)
    {
        return;
    }
    QAction* act = new QAction(QString("Close"), this);
    connect(act, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(act, (QObject*) item);
    contextMenu.addAction(act);
    contextMenu.exec(mapToGlobal(pos));
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
    {
        return 0;
    }
    return Qt::CopyAction | Qt::MoveAction;
}

bool SideBar::dropMimeData(int index, const QMimeData* data, Qt::DropAction action)
{
//   if (action == Qt::MoveAction)
//       return false;
    QList<QUrl> urlList;
    urlList = data->urls(); // retrieve list of urls
    QStringList files;
    for (const QUrl& url : urlList)
    { // iterate over list
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
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData();
    QList<QUrl> urlList;
    QStringList sl;
    for (const QListWidgetItem* lwi : selectedItems())
    {
        sl << QString::number(((Snippet*) lwi)->pageID());
    }
    mimeData->setUrls(urlList);
    drag->setMimeData(mimeData);
    if (drag->exec(supportedActions, Qt::CopyAction) == Qt::CopyAction)
    {
        for (QListWidgetItem* lwi : selectedItems())
        {
            emit fileRemoved(((Snippet*) lwi)->pageID());
            model()->removeRow(row(lwi));
        }
    }
    current = nullptr;
    dragging = false;
}

Snippet* SideBar::getItemByName(const QString& name)
{
    for (int i = 0; i < count(); i++)
    {
        if (((Snippet*) item(i))->getName() == name)
        {
            return ((Snippet*) item(i));
        }
    }
    return nullptr;
}

void SideBar::select(const QString& name)
{
    current = getItemByName(name);
    if (current)
    {
        current->setSelected(true);
    }
}

void SideBar::selectFirstFile()
{
    if (count() == 0)
    {
        current = nullptr;
        return;
    }
    current = (Snippet*) item(0);
}

void SideBar::deleteFile(QObject* item)
{
    //TODO: What can i do with these ugly casts?
    int id = ((Snippet*) item)->pageID();
    QListWidget::removeItemWidget((QListWidgetItem*) item);
    ((QListWidgetItem*) item)->~QListWidgetItem();
    emit fileRemoved(id);
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
