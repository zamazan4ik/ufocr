/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009 Andrei Borovsky <anb@symmetrica.net>

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

#include "settings.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QByteArray>
#include <QDir>
#include <QMessageBox>
#include <QPixmap>
#include <QThread>
#include <stdlib.h>
#include "utils.h"

QString extractFileName(const QString &path)
{
    QFileInfo fi(path);
    return fi.fileName();
}

QString extractFilePath(const QString &path)
{
    QFileInfo fi(path);
    QString s = fi.dir().path();
    if (!s.endsWith("/"))
        s += '/';
    return s;
}

QString extractDigits(const QString &fn)
{
    bool extracting = FALSE;
    QString result = "";
    for (int i = 0; i < fn.size(); i++)
        if ((fn.at(i) >= '0') && (fn.at(i) <= '9')) {
            extracting = TRUE;
            result += fn.at(i);
        } else {
            if (extracting) break;
        }
    return result;
}

bool findProgram(const QString &name)
{
    QStringList sl = QString(getenv("PATH")).split(":");
    QFileInfo fi;
    for (int i = 0; i < sl.count(); i++) {
        fi.setFile(sl.at(i), name);
        if (fi.exists())
            return true;
    }
    return false;
}

static bool mbShown = false;

void styledWarningMessage(QWidget *parent, const QString &text)
{
    if (mbShown) return;
    QMessageBox mb(parent);
    mb.setIconPixmap(QPixmap(":warning.png"));
    mb.setWindowTitle(QObject::trUtf8("Warning"));
    mb.setText(text);
    mb.setButtonText(0, QObject::trUtf8("OK"));
    mbShown = true;
    mb.exec();
    mbShown = false;
}

void styledInfoMessage(QWidget *parent, const QString &text)
{
    QMessageBox mb(parent);
    mb.setIconPixmap(QPixmap(":/images/info.png"));
    mb.setWindowTitle(QObject::trUtf8("Information"));
    mb.setText(text);
    mb.setButtonText(0, QObject::trUtf8("OK"));
    mb.exec();
}

void styledCriticalMessage(QWidget *parent, const QString &text)
{
    QMessageBox mb(parent);
    mb.setIconPixmap(QPixmap(":/critical.png"));
    mb.setWindowTitle(QObject::trUtf8("Error"));
    mb.setText(text);
    mb.setButtonText(0, QObject::trUtf8("OK"));
    mb.exec();
}


struct SleepThread : public QThread { using QThread::msleep;};

void qSleep(int msecs)
{
    SleepThread::msleep(msecs);
}

void clearTmpFiles()
{
    Settings * settings = Settings::instance();
    QFile::remove(settings->workingDir() + "tmp*.bmp");
    QFile::remove(settings->workingDir() + "tmp*.ygf");
    QFile f(settings->workingDir()+settings->getRecognizeInputFile());
    f.remove();
    f.setFileName(settings->workingDir()+settings->getRecognizeOutputFile());
    f.remove();
}
