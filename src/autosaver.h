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
    explicit AutoSaver(QObject *parent = 0);
public slots:
    void save(const QString &path);
signals:
    void reportError(const QString &path);
    void saved();
public slots:
    
};

class AutoSaveManager : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit AutoSaveManager(QObject *parent = 0);
    ~AutoSaveManager();
    void startAutoSave();
signals:
    void startedAutoSave(); // queued
    void startJob(const QString &path);
    void finishedAutoSave();
    void saved();
    void reportError(const QString &path);
public slots:
    void work(const QString &path);
};

#endif // AUTOSAVER_H
