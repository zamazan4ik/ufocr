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

#ifndef SAVEPROJECTDIALOG_H
#define SAVEPROJECTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QStringList>

namespace Ui {
class SaveProjectDialog;
}

class ProjectFiles;
class QShowEvent;
class QFileSystemWatcher;
class QLineEdit;
class SaveProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveProjectDialog(const QString &dir, QWidget *parent = 0);
    ~SaveProjectDialog();
    QString projectPath();
    QString upDir();
signals:
    void projectClicked(const QString &path, const QString &name);
private slots:
    void onDoubleClicked( const QModelIndex & index );
    void onClicked( const QModelIndex & index );
    void goBackwards();
    void goUp();
    void lineEditTextChanged(const QString &arg1);
    void directoryChanged(const QString s);
    void reject();
    void accept();
protected:
    void 	showEvent ( QShowEvent * event );
    virtual void onLineEditTextChanged(QLineEdit * le, const QString &arg1);
    void cd(const QString &path);
    bool isProjectDir(const QString &path);
    QString currentPath();
    QLineEdit * lineEdit();
private:
    Ui::SaveProjectDialog *ui;
    ProjectFiles * pf;
    QFileSystemWatcher * fsw;
    QString ppath;
    QStringList gbList;
private:
    void fillList();
};


#endif // SAVEPROJECTDIALOG_H
