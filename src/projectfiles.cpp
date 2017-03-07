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

#include "projectfiles.h"
#include "settings.h"
#include <QDir>
#include<QFile>
#include <QFileInfoList>
#include <QStringList>

ProjectFiles::ProjectFiles(const QString& rootDir)
{
    cd(rootDir);
}

ProjectFiles::~ProjectFiles()
{
}

QStringList ProjectFiles::directories()
{
    return dirs;
}

void ProjectFiles::cd(const QString& newRoot)
{
    QString newRoot1 = newRoot;
    if (newRoot1 != "")
    {
        if (!newRoot.startsWith("/"))
        {
            newRoot1 = root + newRoot1;
        }
    }
    QFileInfo fi(newRoot1);
    if (!fi.isDir())
    {
        return;
    }
    dirs.clear();
    projs.clear();
    QDir dir(newRoot1);
    root = newRoot1;
    if (!root.endsWith("/"))
    {
        root = root + "/";
    }
    isPD = false;
    QFileInfoList fil = dir.entryInfoList();
    QStringList tmp;
            foreach(QFileInfo fi, fil)
        {
            if (fi.fileName() == "yagf_project.xml")
            {
                isPD = true;
                tmp.clear();
                break;
            }
            if (fi.isDir())
            {
                tmp.append(fi.absoluteFilePath());
            }
        }
            foreach (QString d, tmp)
        {
            if (!d.endsWith("/"))
            {
                d = d + "/";
            }
            QFile f(d + "yagf_project.xml");
            if (f.exists())
            {
                projs.append(d);
            }
            else
            {
                dirs.append(d);
            }
        }
}

bool ProjectFiles::pathExists(const QString& path)
{
    if (!path.startsWith("/"))
    {
        QFileInfo fi(root + path);
        if (fi.exists())
        {
            return true;
        }
    }
    else
    {
        QFileInfo fi(path);
        if (fi.exists())
        {
            return true;
        }
    }
    return false;
}

bool ProjectFiles::canCdTo(const QString& path)
{
    QString lPath = path;
    if (!path.startsWith("/"))
    {
        lPath = root + path;
    }
    QFileInfo fi(lPath);
    if (fi.isDir())
    {
        return !isProjectDir(path);
    }
    return false;
}

bool ProjectFiles::canCdToUpper(const QString& path)
{
    QString lPath = upDir(path);
    return canCdTo(lPath);
    return false;
}

QString ProjectFiles::upDir(const QString& path)
{
    QString lPath;
    if (!path.startsWith("/"))
    {
        lPath = root + path;
    }
    else
    {
        lPath = path;
    }
    QStringList sl = lPath.split("/", QString::SkipEmptyParts);
    if (sl.count() > 0)
    {
        sl.removeLast();
        if (sl.count() == 0)
        {
            return "/";
        }
        QString nPath = sl.join("/");
        if (!nPath.startsWith("/"))
        {
            nPath = "/" + nPath;
        }
        if (!nPath.endsWith("/"))
        {
            nPath = nPath + "/";
        }
        return nPath;
    }
    return "/";
}

QString ProjectFiles::currentDir()
{
    return root;
}

QString ProjectFiles::projectName(const QString& path)
{
    QString lPath;
    if (!path.startsWith("/"))
    {
        lPath = root + path;
    }
    else
    {
        lPath = path;
    }
    if (!lPath.endsWith("/"))
    {
        lPath = lPath + "/";
    }

    QFileInfo fi(lPath);
    if (fi.isDir())
    {
        if (pathExists(lPath + "yagf_project.xml"))
        {
            QStringList sl = lPath.split("/", QString::SkipEmptyParts);
            if (sl.count() > 0)
            {
                return sl.last();
            }
            else
            { return ""; }
        }
        else
        { return ""; }
    }
    else
    {
        if (lPath.endsWith("yagf_project.xml"))
        {
            QStringList sl = lPath.split("/", QString::SkipEmptyParts);
            if (sl.count() > 1)
            {
                return sl.at(sl.count() - 1);
            }
            else
            { return ""; }
        }
        else
        { return ""; }
    }
    return "";
}

bool ProjectFiles::isDirEmpty(const QString& dirName)
{
    QDir dir(dirName);
    if (dir.entryList().count() == 2)
    {
        return true;
    }
    QFileInfoList fil = dir.entryInfoList();
    fil.removeAt(0);
    fil.removeAt(0);
            foreach(QFileInfo fi, fil)
            if (fi.isDir())
            {
                return false;
            }
    return true;
}

bool ProjectFiles::canProjectBeSavedToDir(const QString& name)
{
    QString path;
    if (!name.startsWith("/"))
    {
        path = root + name;
    }
    else
    {
        path = name;
    }
    QFileInfo fi(path);
    if (!fi.isDir())
    {
        return false;
    }
    QDir dir(path);
    return dir.entryList().count() == 2;
}

bool ProjectFiles::isProjectDir()
{
    return isPD;
}

bool ProjectFiles::isProjectDir(const QString& path)
{
    if (!path.startsWith("/"))
    {
        return isProjectDirInternal(root + path);
    }
    else
    {
        return isProjectDirInternal(path);
    }
    return false;
}

bool ProjectFiles::hasTemporaryData()
{
    QString wd = Settings::instance()->workingDir() + "autosave/";
    QDir dir(wd);
    return dir.entryList().count() > 2;
}

QStringList ProjectFiles::projects()
{
    return projs;
}

bool ProjectFiles::isProjectDirInternal(const QString& path)
{
    QFileInfo fi(path);
    if (fi.isDir())
    {
        QDir d;
        if (!path.startsWith("/"))
        {
            d.setPath(root + path);
        }
        else
        {
            d.setPath(path);
        }
        QStringList sl = d.entryList();
        if (sl.contains("yagf_project.xml"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}


