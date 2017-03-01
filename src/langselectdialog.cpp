/*
    YAGF - cuneiform and tesseract OCR graphical front-end
    Copyright (C) 2009-2014 Andrei Borovsky <anb@symmetrica.net>

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

#include "langselectdialog.h"
#include "ui_langselectdialog.h"
#include "settings.h"
#include <QCheckBox>
#include <QIcon>
#include <QPixmap>

const int max_lang = 48;

LangSelectDialog::LangSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LangSelectDialog)
{
    ui->setupUi(this);
    fillLangs();
    connect (ui->listWidgetCuneiform, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onItemClicked(QListWidgetItem *)));
    connect (ui->listWidgetTesseract, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onItemClicked(QListWidgetItem *)));
}

LangSelectDialog::~LangSelectDialog()
{
    delete ui;
}

QStringList LangSelectDialog::getRecognitionLanguages() const
{
    QStringList res;
    foreach(QListWidgetItem * item, items) {
        if (item->checkState() == Qt::Checked)
            res.append(item->text());
    }
    res.removeDuplicates();
    return res;
}


void LangSelectDialog::accept()
{
    QStringList sl = getRecognitionLanguages();
    if (sl.count() == 0)
        return;
    Settings::instance()->setSelectedLanguages(sl);
    QDialog::accept();
}

void LangSelectDialog::onItemClicked(QListWidgetItem *item)
{
    foreach(QListWidgetItem *it, items) {
        if (it->text() == item->text())
            it->setCheckState(item->checkState());
    }
}

void LangSelectDialog::fillLangs()
{
    QStringList sl = Settings::instance()->languagesAvailableTo("cuneiform");
    sl.sort();
    foreach (QString s, sl) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetCuneiform);
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        item->setText(s);
        items.append(item);
        item->setCheckState(Qt::Unchecked);
        if (Settings::instance()->selectedLanguagesAvailableTo("cuneiform").contains(s))
            item->setCheckState(Qt::Checked);
    }
    sl.clear();
    ui->listWidgetTesseract->setIconSize(QSize(18, 18));
    sl = Settings::instance()->languagesAvailableTo("tesseract");
    foreach (QString s, sl) {
        if (s.contains("+"))
            continue;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetTesseract);
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        item->setText(s);
        items.append(item);
        item->setCheckState(Qt::Unchecked);
        if (Settings::instance()->selectedLanguagesAvailableTo("tesseract").contains(s))
            item->setCheckState(Qt::Checked);
        if (Settings::instance()->installedTesseractLanguages().contains(s)) {
            item->setIcon(QIcon(QPixmap(":/images/box.png")));
            item->setToolTip(trUtf8("Installed"));
        } else {
            item->setIcon(QIcon(QPixmap(":/images/notinst.png")));
            item->setToolTip(trUtf8("Not installed"));
        }
        if (s == trUtf8("Digits Only")) {
            item->setCheckState(Qt::Checked);
            item->setIcon(QIcon(QPixmap(":/images/box.png")));
            item->setToolTip(trUtf8("Installed"));

        }

    }
}

