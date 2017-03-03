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

#ifndef AUTOSAVER_H
#define AUTOSAVER_H

#include <QObject>
#include <QThread>
#include <QString>

class AutoSaver : public QObject
{
Q_OBJECT
    //QThread workerThread;
public:
    explicit AutoSaver(QObject* parent = 0);

public slots:

    void save(const QString& path);

signals:

    void reportError(const QString& path);

    void saved();

public slots:

};

class AutoSaveManager : public QObject
{
Q_OBJECT
    QThread workerThread;
public:
    explicit AutoSaveManager(QObject* parent = 0);

    ~AutoSaveManager();

    void startAutoSave();

signals:

    void startedAutoSave(); // queued
    void startJob(const QString& path);

    void finishedAutoSave();

    void saved();

    void reportError(const QString& path);

public slots:

    void work(const QString& path);
};

#endif // AUTOSAVER_H
