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

#include "djvu2pdf.h"
#include "utils.h"
#include "settings.h"
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>

Djvu2PDF::Djvu2PDF(QObject *parent) :
    QObject(parent)
{
    pdfout = Settings::instance()->workingDir()+ QString("fromdjvu/");
    QDir dir(pdfout);
    if (!dir.exists())
        dir.mkdir(pdfout);
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError()));
    connect(&process, SIGNAL(started()), this, SIGNAL(started()), Qt::QueuedConnection);
    connect(&process, SIGNAL(finished(int)), this, SLOT(onFinished()), Qt::QueuedConnection);
}

Djvu2PDF::~Djvu2PDF()
{
    cancel();
}

void Djvu2PDF::convert(const QString &fileName)
{
    if (!findProgram("ddjvu")) {
        emit error(trUtf8("The ddjvu utility from the DjVuLibre package has not been found.\nPlease make sure the package is installed."));
        return;
    }
    QString output = pdfout + "out.pdf";
    QStringList sl;
    sl << "--format=pdf" << fileName << output;
    process.start("ddjvu", sl);
}

QString Djvu2PDF::pdfName() const
{
    return pdfout + "out.pdf";
}

void Djvu2PDF::onError()
{
    emit error(trUtf8("Failed to convert djvu to pdf"));
}

void Djvu2PDF::cancel()
{
    process.kill();
    clearFiles();
}

void Djvu2PDF::onFinished()
{
    QFileInfo fi(pdfout, "out.pdf");
    if (!fi.exists())
        onError();
    else
        emit finished();
}

void Djvu2PDF::clearFiles()
{
    QDir dir;
    dir.setPath(pdfout);
    QFileInfoList fil;
    fil = dir.entryInfoList();
    foreach (QFileInfo fi, fil) {
        dir.remove(fi.absoluteFilePath());
    }
}
