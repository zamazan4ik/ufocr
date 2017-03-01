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

#include "popplerdialog.h"
#include "ui_popplerdialog.h"
#include <QFileDialog>

PopplerDialog::PopplerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopplerDialog)
{
    ui->setupUi(this);
}

PopplerDialog::~PopplerDialog()
{
    delete ui;
}

void PopplerDialog::setPDFFile(const QString fileName)
{
    ui->lineEdit->setText(fileName);
}

void PopplerDialog::on_checkBox_toggled(bool checked)
{
    ui->spinBox->setEnabled(!checked);
    ui->spinBox_2->setEnabled(!checked);
}

void PopplerDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                       trUtf8("Open PDF File"), "", trUtf8("PDF Files (*.pdf)"));
    if (!fileName.isEmpty())
        ui->lineEdit->setText(fileName);
}

QString PopplerDialog::getPDFFile()
{
    return ui->lineEdit->text();
}

QString PopplerDialog::getStartPage()
{
    return QString::number(ui->spinBox->value());
}

QString PopplerDialog::getStopPage()
{
    if (ui->checkBox->isChecked())
        return "-1";
    return QString::number(ui->spinBox_2->value());
}

bool PopplerDialog::getDeskew()
{
    return !(ui->checkBox_2->isChecked());
}
