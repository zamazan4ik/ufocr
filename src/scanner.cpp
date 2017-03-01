/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2012 Andrei Borovsky <anb@symmetrica.net>

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

#include "scanner.h"
#include "utils.h"
#include <QFileInfo>


class XSaneScannerFE : public ScannerBase
{
public:
    XSaneScannerFE(const QString &PLL, QObject *parent = 0) : ScannerBase(parent) {

        addParameter("-s");
        addParameter("-n");
        addParameter("-N");
        setProgramName("XSane");
        setPreloadLibrary(PLL);
        addEnvironmentVar("LD_PRELOAD=" + PLL);

    }

    void exec() {
        waitFor();
        execInternal("xsane");
    }
};


ScannerBase::ScannerBase(QObject *parent) :
    QObject(parent), scanProcess(this)
{
    environment.append(QProcess::systemEnvironment());
}

ScannerBase::~ScannerBase()
{
    waitFor();
}

void ScannerBase::addParameter(const QString &s)
{
    parameters.append(s);
}

void ScannerBase::addEnvironmentVar(const QString &s)
{
    environment.append(s);
}

void ScannerBase::setOutputFile(const QString &s)
{
    outputFile = s;
}

QString ScannerBase::programName()
{
    return pName;
}

void ScannerBase::waitFor()
{
    scanProcess.terminate();
    scanProcess.waitForFinished(10000);
}

void ScannerBase::execInternal(const QString &s)
{
    scanProcess.setEnvironment(environment);
    QStringList sl;
    sl.append(parameters);
    sl.append(outputFile);
    connect(&scanProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    scanProcess.start(s, sl);
}

void ScannerBase::setProgramName(const QString &s)
{
    pName = s;
}

void ScannerBase::setPreloadLibrary(const QString &s)
{
    preloadLibrary = s;
}

QString ScannerFactory::findPreloadLibrary()
{
    QString path = QString(PRILIBRARY_PATH);
    return  path + QString("libxspreload.so");
}

void ScannerFactory::findFEs()
{
    if (findProgram("xsane"))
        fes << "xsane";
}




ScannerFactory::ScannerFactory()
{
    preloadPath = findPreloadLibrary();
    findFEs();
}


ScannerBase *ScannerFactory::createScannerFE(const QString &name)
{
    if (fes.contains(name))
        return new XSaneScannerFE(preloadPath);
    return NULL;
}


void ScannerBase::finished(int, QProcess::ExitStatus)
{
    emit processFinished();
}
