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

#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>
#include <QProcess>
#include <QStringList>


class ScannerBase : public QObject
{
    Q_OBJECT
public:
    explicit ScannerBase(QObject *parent = 0);
    ~ScannerBase();
    void addParameter(const QString &s);
    void addEnvironmentVar(const QString &s);
    void setOutputFile(const QString &s);
    QString programName();
    virtual void exec() = 0;
signals:
    void processFinished();
public slots:
private slots:
    void finished( int, QProcess::ExitStatus );
protected:
    void waitFor();
    void execInternal(const QString &s);
    void setProgramName(const QString &s);
    void setPreloadLibrary(const QString &s);

private:
    QProcess scanProcess;
    QStringList parameters;
    QStringList environment;
    QString outputFile;
    QString pName;
    QString preloadLibrary;
};

class ScannerFactory
{
public:
    ScannerFactory();
    QStringList frontEnds();
    ScannerBase *createScannerFE(const QString &name);
private:
    QString findPreloadLibrary();
    void findFEs();
private:
    QString preloadPath;
    QStringList fes;
};


#endif // SCANNERBASE_H
