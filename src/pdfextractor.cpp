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

#include "pdfextractor.h"
#include "settings.h"
#include "utils.h"
#include "busyform.h"
#include <QThread>
#include <QProcess>
#include  <QStringList>
#include <QApplication>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>


PDFExtractor::PDFExtractor(QObject* parent) :
        QObject(parent), proc(), convertEntire(false)
{
    connect(&proc, SIGNAL(started()), this, SIGNAL(processStarted()), Qt::QueuedConnection);
    connect(&proc, SIGNAL(finished(int)), this, SLOT(procFinished()), Qt::QueuedConnection);
    //connect(&proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(procFinishedError()));
    connect(&proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procFinishedError()));
}

PDFExtractor::~PDFExtractor()
{
    proc.kill();
    clearFiles();
}

void PDFExtractor::setCommandStringPaged(const QString& cmdStr)
{
    commandStringPaged = cmdStr;
}

void PDFExtractor::setConvertEntire(const QString& cmdStr)
{
    commandStringEntire = cmdStr;
}

void PDFExtractor::setOutputDir()
{
    QString pdfout = Settings::instance()->workingDir() + QString("pdfout/");
    outputDir = pdfout;
    QDir dir(pdfout);
    if (!dir.exists())
    {
        dir.mkdir(pdfout);
    }
    else
    {
        dir.setFilter(QDir::Files);
        QStringList sl = dir.entryList();
        for(const QString& s : sl)
        {
            dir.remove(pdfout + s);
        }
    }
}

QString PDFExtractor::getOutputDir()
{
    return outputDir;
}

void PDFExtractor::setOutputExtension(const QString& value)
{
    outputExtension = value;
}

QString PDFExtractor::getOutputExtension()
{
    return outputExtension;
}

void PDFExtractor::run()
{
    proc.setEnvironment(QProcess::systemEnvironment());
    QStringList sl = makeCommandString();
    QString cmd = sl.first();
    sl.removeAt(0);
    proc.start(cmd, sl);
}

void PDFExtractor::setOutputPrefix(const QString& value)
{
    outputPrefix = value;
}

QString PDFExtractor::getOutputPrefix()
{
    return outputPrefix;
}

void PDFExtractor::setResolution(const QString& value)
{
    resolution = value;
}

QString PDFExtractor::getResolution()
{
    return resolution;
}

void PDFExtractor::setSourcePDF(const QString& value)
{
    sourcePDF = value;
}

QString PDFExtractor::getSourcePDF()
{
    return sourcePDF;
}

void PDFExtractor::setStartPage(const QString& value)
{
    startPage = value.toInt();
}

int PDFExtractor::getStartPage()
{
    return startPage;
}

void PDFExtractor::setStopPage(const QString& value)
{
    stopPage = value.toInt();
}

int PDFExtractor::getStopPage()
{
    return stopPage;
}

void PDFExtractor::cancelProcess()
{
    proc.kill();
    clearFiles();
}


void PDFExtractor::sortDir(QDir& dir)
{
    //pageCount();
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    dir.setSorting(QDir::Name);
    dir.setPath(outputDir);
}


#include "utils.h"


void PDFExtractor::procFinished()
{
    emit processFinished(false);
    processFiles();
}

void PDFExtractor::procFinishedError()
{
    emit error(trUtf8("There was an error while extracting PDF pages."));
    emit processFinished(true);
    processFiles();
}

void PDFExtractor::processFiles()
{
    QApplication::processEvents();
    QDir dir;
    sortDir(dir);
    QFileInfoList fil;
    fil = dir.entryInfoList(filters, QDir::Files, QDir::Name);
    int counter = fil.count();
    while (counter > 0)
    {
        int current = fil.count() - counter;
        QApplication::processEvents();
        emit addPage(fil[current].absoluteFilePath(), current + 1, fil.count());
        counter--;
    }
    clearFiles();
    emit extractingFinished();
}

void PDFExtractor::clearFiles()
{
    QDir dir;
    dir.setPath(outputDir);
    QFileInfoList fil;
    fil = dir.entryInfoList(filters, QDir::Files, QDir::Name);
            foreach (QFileInfo fi, fil)
        {
            dir.remove(fi.absoluteFilePath());
        }
}

void PDFExtractor::removeRemaining()
{
    if (lastFile != "")
    {
        QDir dir;
        sortDir(dir);
        QStringList sl = dir.entryList();
        sl.sort();
        bool doDelete = false;
        for (int i = 0; i < sl.count(); i++)
        {
            if (doDelete)
            {
                QFile f(outputDir + sl.at(i));
                f.remove();
            }
            if (lastFile.endsWith(sl.at(i)))
            {
                doDelete = true;
            }
        }
    }
    lastFile = "";
}
