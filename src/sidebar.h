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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QListWidget>
#include <QSignalMapper>

class Snippet;

class SideBar : public QListWidget
{
Q_OBJECT
public:
    explicit SideBar(QWidget* parent = 0);

    void addItem(Snippet* item);

    void deleteItem(Snippet* item);

    void clearBlocks();

    void select(const QString& name);

    void selectFirstFile();

signals:

    void pageSelected(int id);

    void filesDropped(QStringList);

    void fileRemoved(int id);

private slots:

    void itemActive(QListWidgetItem* item, QListWidgetItem* item2);

    void showContextMenuForWidget(const QPoint &pos);

    void deleteFile(QObject*);
protected:
    QStringList mimeTypes() const;

    Qt::DropActions supportedDropActions() const;

    bool dropMimeData(int index, const QMimeData* data, Qt::DropAction action);

    void startDrag(Qt::DropActions supportedActions);

private:
    QSignalMapper* signalMapper;
    Snippet* getItemByName(const QString& name);

    Snippet* current;
    bool lock;
    bool dragging;
};

#endif // SIDEBAR_H
