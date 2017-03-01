/* YAGF - cuneiform and tesseract OCR graphical front-end
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


#include "recognitiondialog.h"
#include "ui_recognitiondialog.h"

RecognitionDialog::RecognitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecognitionDialog)
{
    ui->setupUi(this);
    ui->widget_2->startAnimation();
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(rejected()));
    connect (this, SIGNAL(rejected()), this, SIGNAL(cancel()), Qt::QueuedConnection);
}

RecognitionDialog::~RecognitionDialog()
{
    delete ui;
}

void RecognitionDialog::blockRecognized(int n)
{
    ui->label_2->setText(trUtf8("block # %1").arg(n));
}
