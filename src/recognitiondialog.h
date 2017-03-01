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


#ifndef RECOGNITIONDIALOG_H
#define RECOGNITIONDIALOG_H

#include <QDialog>

namespace Ui {
class RecognitionDialog;
}

class RecognitionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RecognitionDialog(QWidget *parent = 0);
    ~RecognitionDialog();
signals:
    void cancel();
public slots:
    void blockRecognized(int n);
private:
    Ui::RecognitionDialog *ui;
};

#endif // RECOGNITIONDIALOG_H
