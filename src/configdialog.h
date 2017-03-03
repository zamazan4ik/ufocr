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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
Q_OBJECT

public:
    explicit ConfigDialog(QWidget* parent = 0);

    ~ConfigDialog();

    void accept();

private slots:

    void on_pushButtonTessData_clicked();

    void on_pushButtonLangs_clicked();

    void itemClicked(QListWidgetItem* item);

    void on_checkBoxProcessAfterDeskew_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

private:
    void init();

private:
    Ui::ConfigDialog* ui;
};

#endif // CONFIGDIALOG_H
