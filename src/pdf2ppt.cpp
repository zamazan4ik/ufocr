/*
    UFOCR - User-Friendly OCRs
    Copyright (C) 2009-2010 Alexander Zaitsev <zamazan4ik@tut.by>

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

#include "pdf2ppt.h"
#include <QStringList>
#include <QString>

PDF2PPT::PDF2PPT() : PDFExtractor()
{
}

QStringList PDF2PPT::makeCommandString()
{
    QStringList args;
    args.append("pdftoppm");
    args << "-jpeg";
    if (getStopPage() > 0)
    {
        if (getStartPage() == 0)
        {
            this->setStartPage("1");
        }
        args << "-f" << QString::number(getStartPage()) << "-l" << QString::number(getStopPage());
    }
    args << "-rx" << "600" << "-ry" << "600" << this->getSourcePDF();
    setOutputPrefix(getOutputDir().append("page"));
    args << getOutputPrefix();
    setOutputExtension("jpg");
    return args;
}
