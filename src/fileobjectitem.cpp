/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "fileobjectitem.h"
#include <QStringList>
#include <QIcon>
#include <QPixmap>
#include <QListWidget>

FileObjectItem::FileObjectItem(FileObjectItemType itemType, const QString &fullName, QListWidget *parent) :
    QListWidgetItem(parent),
    fName(fullName),
    _type(itemType)
{
    extractShortName();
    setText(lName);
    setToolTip(lName);
}

FileObjectItem::FileObjectItemType FileObjectItem::getItemType()
{
    return _type;
}

QString FileObjectItem::getShortName()
{
    return lName;
}

QString FileObjectItem::getPath()
{
    return fName;
}

void FileObjectItem::extractShortName()
{
    QStringList sl = fName.split("/", QString::SkipEmptyParts);
    if (sl.count() > 0) {
        lName = sl.at(sl.count()-1);
    }
    else
        lName = "/";
    QString path;
    switch(_type) {
    case EmptyDirectory:
        path = ":/images/folder.png";
        break;
    case Directory:
        path = ":/images/folder_nempty.png";
        break;
    case Project:
        path = ":/images/project.png";
        break;
    default:
        path = "";
    }
    setIcon(QIcon(path));
}
