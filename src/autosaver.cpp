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

#include "autosaver.h"
#include "projectmanager.h"
#include "settings.h"
#include "globallock.h"

AutoSaver::AutoSaver(QObject* parent) :
        QObject(parent)
{
}

void AutoSaver::save(const QString& path)
{
    ProjectSaver ps;
    if (!ps.save(path))
            emit { reportError(trUtf8("failed to save the project")); }
    else
            emit { saved(); }
}

AutoSaveManager::AutoSaveManager(QObject* parent) :
        QObject(parent)
{
}

AutoSaveManager::~AutoSaveManager()
{
    workerThread.quit();
    workerThread.wait();
}

void AutoSaveManager::startAutoSave()
{
    emit startedAutoSave();
}


void AutoSaveManager::work(const QString& path)
{
    AutoSaver* autoSaver = new AutoSaver;
    autoSaver->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), autoSaver, SLOT(deleteLater()));
    connect(this, SIGNAL(startJob(QString)), autoSaver, SLOT(save(QString)));
    connect(autoSaver, SIGNAL(saved()), this, SIGNAL(finishedAutoSave()));
    connect(autoSaver, SIGNAL(reportError(QString)), this, SIGNAL(reportError(QString)));
    workerThread.start();
    // workerThread.setPriority(QThread::LowPriority);
    if (path != "")
            emit { startJob(path); }
    else
    {
        QString npath = Settings::instance()->workingDir() + "autosave";
        emit startJob(npath);
    }
}
