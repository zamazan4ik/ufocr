#include "autosaver.h"
#include "projectmanager.h"
#include "settings.h"
#include "globallock.h"

AutoSaver::AutoSaver(QObject *parent) :
    QObject(parent)
{
}

void AutoSaver::save(const QString &path)
{
    ProjectSaver ps;
    if (!ps.save(path))
        emit reportError(trUtf8("failed to save the project"));
    else
        emit saved();
}

AutoSaveManager::AutoSaveManager(QObject *parent):
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


void AutoSaveManager::work(const QString &path)
{
    AutoSaver * autoSaver= new AutoSaver;
    autoSaver->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), autoSaver, SLOT(deleteLater()));
    connect(this, SIGNAL(startJob(QString)), autoSaver, SLOT(save(QString)));
    connect(autoSaver, SIGNAL(saved()), this, SIGNAL(finishedAutoSave()));
    connect(autoSaver, SIGNAL(reportError(QString)), this, SIGNAL(reportError(QString)));
    workerThread.start();
   // workerThread.setPriority(QThread::LowPriority);
    if (path != "")
        emit startJob(path);
    else {
        QString npath = Settings::instance()->workingDir() + "autosave";
        emit startJob(npath);
    }
}
