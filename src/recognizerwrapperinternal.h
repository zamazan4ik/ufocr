/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#ifndef RECOGNIZERWRAPPERINTERNAL_H
#define RECOGNIZERWRAPPERINTERNAL_H

#include "settings.h"
#include "tpagecollection.h"
#include "utils.h"
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QDir>
#include "globallock.h"
#include <QTextCodec>

class RWHelper
{
public:
    RWHelper()
    {
    }

    ~RWHelper()
    {
        GlobalLock::instance()->unlock();
    }
};

class RecognizerWrapperInternal : public QObject
{
    Q_OBJECT
    friend class RecognizerWrapper;
public:
    RecognizerWrapperInternal(QObject * parent) : QObject(parent)
    {
        connect(&proc, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
        connect(&proc, SIGNAL(error(QProcess::ProcessError)), this, SIGNAL(error(QProcess::ProcessError)));
        supressErros = false;
    }
    ~RecognizerWrapperInternal()
    {
        proc.kill();
        proc.waitForFinished(100);
    }

    void init() {
        settings = Settings::instance();
        pc = PageCollection::instance();
    }
    void recognize()
    {
        QFile::remove(settings->workingDir() + "input*.bmp");
        if (!pc->pageValid()) {
            emit error(trUtf8("No image loaded"));
            return;
        }
        if (!findEngine()) return;
        if (pc->blockCount() > 0) {
            blockIndex = 0;
            prepareBlockForRecognition(0);
            emit blockRecognized(0);
            recognizeInternal();
        } else {
            preparePageForRecognition();
            recognizeInternal();
        }
    }

signals:
    void finished(int);
    void error(QProcess::ProcessError);
    void error(const QString &text);
    void blockRecognized(int n);
    void readOutput(QString text, QChar separator = QChar::fromAscii('\n'));
public slots:
    void onFinished(int i)
    {
        sendOutput();
        blockIndex++;
        if (blockIndex < pc->blockCount()) {
            prepareBlockForRecognition(blockIndex);
            emit blockRecognized(blockIndex+1);
            recognizeInternal();
        } else emit finished(i);
    }

private: // variables
    Settings * settings;
    PageCollection * pc;
    QProcess proc;
    bool supressErros;
    int blockIndex;

private: // functions
    void sendOutput()
    {
        bool res = GlobalLock::instance()->lock();
        if (!res)
            return;
        RWHelper h;
        QFile textFile(settings->workingDir() + settings->getRecognizeOutputFile());
        textFile.open(QIODevice::ReadOnly);
        QString textData;
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        if (settings->getKeepLines()) {
            while (!textFile.atEnd()) {
                QByteArray text = textFile.readLine();
                textData += codec->toUnicode(text);
            }
        } else {
            QByteArray text = textFile.readAll();
            textData = codec->toUnicode(text);
            textData =textData.replace(QString::fromUtf8("—")+'\n', QString::fromUtf8("— "));
            textData = textData.replace(QString::fromUtf8("-")+'\n', QString::fromUtf8("- "));
            textData = textData.replace(QString::fromUtf8("\n"), QString::fromUtf8(" "));
        }
        textFile.close();
        textData = textData.replace(" ,", ",");
        QChar sep = QChar::fromAscii(']');
        if (pc->blockCount() > blockIndex) {
            if (pc->getBlock(blockIndex).isTableCell())
                sep = QChar::fromAscii('|');
        }
        emit readOutput(textData, sep);
    }

    void recognizeInternal() {
        if (settings->getSelectedEngine() == UseCuneiform) {
            if (useCuneiform(settings->getRecognizeInputFile(), settings->getRecognizeOutputFile()))
                return;
        }
        if (settings->getSelectedEngine() == UseTesseract) {
            if (useTesseract(settings->getRecognizeInputFile()))
                return;
        }
    }
    bool useTesseract(const QString &inputFile)
    {
        if (!findProgram("tesseract"))
            return false;
        proc.setWorkingDirectory(settings->workingDir());
        QStringList sl;
        sl.append(inputFile);
        sl.append(settings->getRecognizeOutputBase());
        if (settings->getLanguage() != "digits")
            sl.append("-l");
        sl.append(settings->getLanguage());
        //sl.append("-psm");
        //sl.append("0");
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("TESSDATA_PREFIX", settings->getTessdataPath());
        QDir dir(settings->getTessdataPath()+"tessdata/");
        QStringList sl1;
        sl1 << QString::fromUtf8("*%1.*").arg(settings->getLanguage());
        if ((!settings->getLanguage().contains("+"))&&(settings->getLanguage() != "digits"))
            if (dir.entryList(sl1, QDir::Files).count() == 0) {
                emit error(trUtf8("You have selected recognising %1 language using tesseract OCR. Currently the data for this language is not installed in your system. Please install the tesseract data files for \"%2\" from your system repository.").arg(settings->getFullLanguageName(settings->getLanguage())).arg(settings->getLanguage()));
                return false;
            }
        proc.setProcessEnvironment(env);
        proc.start("tesseract", sl);

        /*if (proc.exitCode()) {
            QByteArray stdoutBytes = proc.readAllStandardOutput();
            QByteArray stderrBytes = proc.readAllStandardError();
            QString output = QString(stdoutBytes) + QString(stderrBytes);
            styledCriticalMessage(this, trUtf8("Starting tesseract failed") + (output != "" ? "" : trUtf8("Program not found.")));
            return false;
        }*/
        return true;
    }
    bool useCuneiform(const QString &inputFile, const QString &outputFile)
    {
        if (!findProgram("cuneiform"))
            return false;
        proc.setWorkingDirectory(settings->workingDir());
        QStringList sl;
        sl.append("-l");
        sl.append(settings->getLanguage());
        sl.append("-f");
        if (settings->getOutputFormat() == "text")
            sl.append("text");
        else
            sl.append("html");
        sl.append("-o");
        sl.append(settings->workingDir() + outputFile);
        sl.append(settings->workingDir() + inputFile);
        proc.start("cuneiform", sl);

        /*if (proc.exitCode()) {
            QByteArray stdoutBytes = proc.readAllStandardOutput();
            QByteArray stderrBytes = proc.readAllStandardError();
            QString output = QString(stdoutBytes) + QString(stderrBytes);
            styledCriticalMessage(this, trUtf8("Starting cuneiform failed") + (output != "" ? "" : trUtf8("Program not found.")));
            return false;
        }*/
        return true;
    }
    static bool findEngine(bool findSelected = false)
    {
        Settings * settings = Settings::instance();
        if (settings->getSelectedEngine() == UseCuneiform) {
            if (!findProgram("cuneiform")) {
                if (findSelected)
                    return false;
                if (findProgram("tesseract")) {
                    //emit error(trUtf8("Cuneiform not found, switching to Tesseract OCR"));
                    settings->setSelectedEngine(UseTesseract);
                } else {
                    //emit error(trUtf8("No recognition engine found.\nPlease install either Cuneiform or Tesseract OCR"));
                    return false;
                }
            }
        }
        if (settings->getSelectedEngine() == UseTesseract) {
            if (!findProgram("tesseract")) {
                if (findSelected)
                    return false;
                if (findProgram("cuneiform")) {
                    //emit error(trUtf8("Tesseract not found, switching to Cuneiform"));
                    settings->setSelectedEngine(UseCuneiform);
                } else {
                    //emit error(trUtf8("No recognition engine found.\nPlease install either Cuneiform or Tesseract OCR"));
                    return false;
                }
            }
        }
        return true;
    }
    void preparePageForRecognition()
    {
        clearTmpFiles();
        pc->savePageForRecognition(settings->workingDir() + settings->getRecognizeInputFile());
    }

    void prepareBlockForRecognition(const QRect &r)
    {
        clearTmpFiles();
        pc->saveBlockForRecognition(r, settings->workingDir() + settings->getRecognizeInputFile());
    }

    void prepareBlockForRecognition(int index)
    {
        clearTmpFiles();
        pc->saveBlockForRecognition(index, settings->workingDir() + settings->getRecognizeInputFile());
    }

};

#endif // RECOGNIZERWRAPPERINTERNAL_H
