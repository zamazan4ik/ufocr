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

#include "loadprojectdialog.h"
#include <QLineEdit>

LoadProjectDialog::LoadProjectDialog(const QString &path, QWidget *parent) :
    SaveProjectDialog(path, parent)
{
    setWindowTitle(trUtf8("Load Project"));
    lineEdit()->setStyleSheet("color: rgb(0, 0, 0)");
    connect(this, SIGNAL(projectClicked(QString,QString)), this, SLOT(onProjectClicked(QString,QString)));
}

QString LoadProjectDialog::projectPath()
{
    return pPath;
}

QString LoadProjectDialog::projectName()
{
    return pName;
}

void LoadProjectDialog::accept()
{
    if (lineEdit()->text() != "") {
        if (isProjectDir(lineEdit()->text())) {
            pPath = lineEdit()->text();
            QDialog::accept();
        }
        else
            cd(lineEdit()->text());
        return;
    }
    if (pPath != "") {
        QDialog::accept();
    }
}

void LoadProjectDialog::reject()
{
    pPath = "";
    pName = "";
    QDialog::reject();
}

void LoadProjectDialog::onProjectClicked(const QString &path, const QString &name)
{
    pPath = path;
    pName = name;
    QDialog::accept();
}

void LoadProjectDialog::onLineEditTextChanged(QLineEdit *le, const QString &arg1)
{
    le->setStyleSheet("");
}
