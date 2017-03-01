/*
   YAGF - cuneiform and tesseract OCR graphical front-end
   Copyright (C) 2009-2015 Andrei Borovsky <borovsky.andrei@gmail.com>

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

#ifndef LOADPROJECTDIALOG_H
#define LOADPROJECTDIALOG_H

#include "saveprojectdialog.h"

class LoadProjectDialog : public SaveProjectDialog
{
    Q_OBJECT
public:
    explicit LoadProjectDialog(const QString &path, QWidget *parent = 0);
    QString projectPath();
    QString projectName();
signals:
public slots:
protected slots:
    void accept();
    void reject();
private slots:
    void onProjectClicked(const QString &path, const QString &name);
protected:
    virtual void onLineEditTextChanged(QLineEdit * le, const QString &arg1);
private:
    QString pPath;
    QString pName;
};

#endif // LOADPROJECTDIALOG_H
