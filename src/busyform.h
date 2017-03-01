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

#ifndef BUSYFORM_H
#define BUSYFORM_H

#include <QSplashScreen>

namespace Ui
{
class BusyForm;
}

class BusyForm : public QSplashScreen
{
    Q_OBJECT

public:
    explicit BusyForm(QWidget *parent = 0);
    ~BusyForm();
public slots:
    void textOut(const QString &text);

private:
    Ui::BusyForm *ui;
};

#endif // BUSYFORM_H
