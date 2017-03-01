/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2010 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef PDFEXTRACTOR_H
#define PDFEXTRACTOR_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QStringList>

class QDir;
class PDFExtractor : public QObject
{
    Q_OBJECT
public:
    explicit PDFExtractor(QObject *parent = 0);
    ~PDFExtractor();
    void setCommandStringPaged(const QString &cmdStr);
    void setConvertEntire(const QString &cmdStr);
    bool isConvertEntire();
    void setSourcePDF(const QString &value);
    QString getSourcePDF();
    void setOutputDir();
    QString getOutputDir();
    void setStartPage(const QString &value);
    int getStartPage();
    void setStopPage(const QString &value);
    int getStopPage();
    void setResolution(const QString &value);
    QString getResolution();
    void setOutputPrefix(const QString &value);
    QString getOutputPrefix();
    void setOutputExtension(const QString &value);
    QString getOutputExtension();
    void run();
    static bool findCProgram();
    void removeRemaining();
signals:
    void processStarted();
    void processFinished(bool error);
    void addPage(QString pageName, int current, int total);
    void extractingFinished();
    void error(const QString &text);
public slots:
    void cancelProcess();
private slots:
    void procFinished();
    void procFinishedError();
protected:
    void sortDir(QDir &dir);
    virtual QStringList makeCommandString() = 0;
private:
    void processFiles();
    void clearFiles();
private:
    QString commandStringPaged;
    QString commandStringEntire;
    QString sourcePDF;
    QString outputDir;
    int startPage;
    int stopPage;
    QString resolution;
    QString outputPrefix;
    QString outputExtension;
    QStringList filters;
    QString lastFile;
    QProcess proc;
    bool convertEntire;
};

#endif // PDFEXTRACTOR_H
