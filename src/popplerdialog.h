/*
    YAGF - cuneiform and tesseract OCR graphical front-end
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

#ifndef POPPLERDIALOG_H
#define POPPLERDIALOG_H

#include <QDialog>

namespace Ui
{
class PopplerDialog;
}

class PopplerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PopplerDialog(QWidget *parent = 0);
    ~PopplerDialog();
    void setPDFFile(const QString fileName);
    QString getPDFFile();
    QString getStartPage();
    QString getStopPage();
    bool getDeskew();
private slots:
    void on_checkBox_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::PopplerDialog *ui;
};

#endif // POPPLERDIALOG_H
