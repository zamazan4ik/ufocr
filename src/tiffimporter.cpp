/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
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

#include "tiffimporter.h"
#include "settings.h"
#include <QProcess>
#include <QDir>
#include <QImageReader>

TiffImporter::TiffImporter(const QString &fileName, QObject *parent) :
    QObject(parent), tiffName(fileName)
{
}

void TiffImporter::exec()
{
    QString wd = Settings::instance()->workingDir();
    QDir dir;
    dir.setPath(wd);
    QStringList sl, pf, nf;
    sl << "t*_out*.jpg";
    pf = dir.entryList(sl);
    QProcess proc;
    proc.setEnvironment(QProcess::systemEnvironment());
    proc.setWorkingDirectory(wd);
    QString size = Settings::instance()->tiffPageSize();
    QString density = Settings::instance()->tiffDensity();
    QString cmd = QString("convert %1  -units PixelsPerInch -resize %2x%3 -units PixelsPerInch -density %4  t%5_out.jpg").arg(tiffName).arg(size).arg(size).arg(density).arg(Settings::instance()->uniqueSeed());
    proc.start(cmd);
    proc.waitForFinished();
    if (proc.exitStatus() != QProcess::NormalExit) {
        emit error();
        return;
    }
    nf = dir.entryList(sl);

    foreach(QString s, nf) {
        if (!pf.contains(s))
            files.append(wd+s);
    }
    if (files.count())
        emit finished(files);
    else {
        QImageReader ir(tiffName);
        if (!ir.read().isNull())
            files.append(tiffName);
        else {
            const QString ppmFile = QString::fromAscii("input.ppm");
            if (dir.exists(ppmFile)) {
                dir.remove(ppmFile);
                QProcess proc;
                proc.setEnvironment(QProcess::systemEnvironment());
                proc.setWorkingDirectory(wd);
                proc.start("tifftopnm > " + ppmFile);
                proc.waitForFinished();
                QImageReader ir(wd+ppmFile);
                if (!ir.read().isNull())
                    files.append(wd + ppmFile);
            }
        }
    }
    if (files.count())
        emit finished(files);
    else
        emit error();
}

QStringList TiffImporter::extractedFiles()
{
    return files;
}
