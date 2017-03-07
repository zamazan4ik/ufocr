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

#ifndef FILEOBJECTITEM_H
#define FILEOBJECTITEM_H

#include <QListWidgetItem>

class FileObjectItem : public QListWidgetItem
{
public:
    enum FileObjectItemType
    {
        Directory,
        EmptyDirectory,
        Project
    };
    Q_DECLARE_FLAGS(FOItemType, FileObjectItemType)

    explicit FileObjectItem(FileObjectItemType itemType, const QString& fullName, QListWidget* parent = 0);

    FileObjectItemType getItemType();

    QString getShortName();

    QString getPath();

signals:

public slots:
private:
    QString fName;
    QString lName;
    FileObjectItemType _type;
private:
    void extractShortName();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FileObjectItem::FOItemType)

#endif // FILEOBJECTITEM_H
