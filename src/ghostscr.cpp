/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
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

#include "ghostscr.h"
#include <QStringList>
#include <QString>

GhostScr::GhostScr() : PDFExtractor()
{
}

QStringList GhostScr::makeCommandString()
{
    QStringList args;
    args.append("gs");
    args << "-SDEVICE=jpeg" << "-r1200x1200" << "-sPAPERSIZE=letter" << "-dNOPAUSE" << "-dBATCH";
    if (getStopPage() > 0) {
        if (getStartPage() == 0)
            this->setStartPage("1");
        args << QString("-dFirstPage=").append(QString::number(getStartPage())) << QString("-dLastPage=").append(QString::number(getStopPage()));
    }
    setOutputPrefix(getOutputDir().append("page"));
    args << QString("-sOutputFile=").append(getOutputPrefix()).append("_%04d.jpg");
    args << "--" << this->getSourcePDF();
    setOutputExtension("jpg");
    return args;
}
