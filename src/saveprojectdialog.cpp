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

#include "saveprojectdialog.h"
#include "ui_saveprojectdialog.h"
#include "projectfiles.h"
#include "fileobjectitem.h"
#include <QShowEvent>
#include <QStringList>
#include <QFileSystemWatcher>


const int itemType = 0;
const int itemTypeFolder = 1;
const int itemTypeEmptyFolder = 2;
const int itemTypeProject = 3;

const int itemDisplayName = 1;
const int itemFullName = 2;


SaveProjectDialog::SaveProjectDialog(const QString &dir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveProjectDialog),
    pf(new ProjectFiles(dir)),
    fsw(new QFileSystemWatcher(QStringList(dir)))
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
    ui->lineEdit->setText(dir);
}

SaveProjectDialog::~SaveProjectDialog()
{
    delete fsw;
    delete pf;
    delete ui;
}

QString SaveProjectDialog::projectPath()
{
    if (ppath != "") {
        if (!ppath.startsWith("/"))
            ppath = pf->currentDir() + ppath;
        if (!ppath.endsWith("/"))
            ppath = ppath + "/";
    }
    return ppath;
}

QString SaveProjectDialog::upDir()
{
    return pf->upDir(ppath);
}

void SaveProjectDialog::onDoubleClicked(const QModelIndex &index)
{
    gbList.append(pf->currentDir());
    FileObjectItem * item = (FileObjectItem *) ui->listWidget->item(index.row());
    if (item) {
        if (item->getItemType() == FileObjectItem::Project) {
            emit projectClicked(item->getPath(), item->getShortName());
            return;
        }
        if ((item->getItemType() == FileObjectItem::EmptyDirectory)|(item->getItemType() == FileObjectItem::Directory))
            delete fsw;
            pf->cd(item->getPath());
            fillList();
            ui->lineEdit->setText("");
            ui->lineEdit->setStyleSheet("color: rgb(0, 0, 0)");
            ui->lineEdit->setToolTip(trUtf8("Add project name"));
            ppath = "";
            fsw = new QFileSystemWatcher(QStringList(pf->currentDir()));
            connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
    }
}

void SaveProjectDialog::onClicked(const QModelIndex &index)
{
    ui->lineEdit->setText("");
    ui->lineEdit->setStyleSheet("");
    ppath ="";
    FileObjectItem * item = (FileObjectItem *) ui->listWidget->item(index.row());
    if (!item)
        return;
    if (item->getItemType() == FileObjectItem::Project) {
        emit projectClicked(item->getPath(), item->getShortName());
        return;
    }
    if (pf->canProjectBeSavedToDir(item->getPath())) {
        ui->lineEdit->setStyleSheet("color: rgb(0, 0, 200)");
        ui->lineEdit->setText(item->getPath());
        ui->lineEdit->setStyleSheet("");
        ui->lineEdit->setToolTip(trUtf8("Save project to this folder?"));
        ppath = item->getPath();
    } else {
        ui->lineEdit->setStyleSheet("");
        if (!item->getPath().endsWith("/"))
            ui->lineEdit->setText(item->getPath() + "/");
        else
            ui->lineEdit->setText(item->getPath());
       //ui->lineEdit->setStyleSheet("color: rgb(255, 255, 255)");
       ui->lineEdit->setToolTip(trUtf8("Add project name"));
    }
}

void SaveProjectDialog::showEvent(QShowEvent *event)
{
    event->accept();
    fillList();
    QDialog::showEvent(event);
}

void SaveProjectDialog::onLineEditTextChanged(QLineEdit *le, const QString &arg1)
{
    if (pf->pathExists(arg1)) {
        if(pf->canProjectBeSavedToDir(arg1)) {
            le->setStyleSheet("");
            ppath = arg1;
        } else {
            le->setStyleSheet("color: rgb(200, 0, 0)");
            ppath = "";
        }
    } else {
        if (pf->canCdToUpper(arg1)) {
            le->setStyleSheet("");
            ppath = arg1;
        } else {
            le->setStyleSheet("color: rgb(200, 0, 0)");
            ppath = "";
        }
    }
}

void SaveProjectDialog::cd(const QString &path)
{
    if (pf->canCdTo(path)) {
        delete fsw;
        gbList.append(pf->currentDir());
        pf->cd(path);
        fillList();
        fsw = new QFileSystemWatcher(QStringList(pf->currentDir()));
        connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
    }
}

bool SaveProjectDialog::isProjectDir(const QString &path)
{
    return pf->isProjectDir(path);
}

QString SaveProjectDialog::currentPath()
{
    return pf->currentDir();
}

QLineEdit *SaveProjectDialog::lineEdit()
{
    return ui->lineEdit;
}

void SaveProjectDialog::accept()
{
        if (ui->lineEdit->text() != "")
            if (pf->canCdTo(ui->lineEdit->text())) {
                if (pf->isProjectDir(ui->lineEdit->text()))
                    return;
                delete fsw;
                gbList.append(pf->currentDir());
                pf->cd(ui->lineEdit->text());
                ui->lineEdit->setText("");
                fillList();
                fsw = new QFileSystemWatcher(QStringList(pf->currentDir()));
                connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
                return;
            }
            if (ppath == "")
                return;
    QDialog::accept();
}



void SaveProjectDialog::fillList()
{
    QStringList directories = pf->directories();
    if (directories.count() > 1) {
        directories.removeAt(0);
        directories.removeAt(0);
    }
    QStringList projects = pf->projects();
    directories.sort();
    projects.sort();
    ui->listWidget->clear();
    foreach (QString s, directories) {
        FileObjectItem * item = new FileObjectItem(pf->isDirEmpty(s) ? FileObjectItem::EmptyDirectory : FileObjectItem::Directory, s, ui->listWidget);
        ui->listWidget->addItem(item);
    }
    foreach (QString s, projects) {
        FileObjectItem * item = new FileObjectItem(FileObjectItem::Project, s);
        ui->listWidget->addItem(item);
    }
}

void SaveProjectDialog::goUp() {
    delete fsw;
    gbList.append(pf->currentDir());
    pf->cd(pf->upDir(pf->currentDir()));
    fillList();
    fsw = new QFileSystemWatcher(QStringList(pf->currentDir()));
    connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
}

void SaveProjectDialog::reject()
{
    ppath = "";
    QDialog::reject();
}

void SaveProjectDialog::goBackwards()
{
    if (!gbList.isEmpty()) {
        delete fsw;
        pf->cd(gbList.last());
        gbList.removeLast();
        fillList();
        fsw = new QFileSystemWatcher(QStringList(pf->currentDir()));
        connect(fsw, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)), Qt::QueuedConnection);
    }
}

void SaveProjectDialog::lineEditTextChanged(const QString &arg1)
{
    onLineEditTextChanged(ui->lineEdit, arg1);
}

void SaveProjectDialog::directoryChanged(const QString s)
{
    pf->cd(pf->currentDir());
    fillList();
}
