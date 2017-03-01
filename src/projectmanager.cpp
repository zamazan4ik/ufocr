/*
    YAGF - cuneiform and tesseract OCR graphical front-end
    Copyright (C) 2011 Andrei Borovsky <anb@symmetrica.net>

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

#include "projectmanager.h"
#include "tpagecollection.h"
#include "settings.h"
#include "globallock.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QXmlStreamAttributes>
#include <QStringList>

const QString URI = "symmetrica.net/yagf";
const QString VERSION = "0.9.5";



inline QString boolToString(bool value)
{
    return value ? "true" : "false";
}

ProjectSaver::ProjectSaver(QObject *parent) :
    QObject(parent)
{
}

bool ProjectSaver::save(const QString &dir)
{
    bool res = GlobalLock::instance()->lock();
    if (!res)
        return false;
    Unlocker unlocker(true);
    directory = dir;
    if (!directory.endsWith("/")) directory = directory + "/";
    QDir _dir(directory);
    if (QFile::exists(directory+"yagf_project.old")) {
            QFile f(directory+"yagf_project.old");
            f.remove();
    }
    if (QFile::exists(directory+"yagf_project.xml")) {
            QFile f(directory+"yagf_project.xml");
            f.copy(directory+"yagf_project.old");
    }
    if (!_dir.exists())
        _dir.mkdir(directory);
    QString fileName = directory+"yagf_project.xml";
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        return false;
    }
    stream = new QXmlStreamWriter(&f);
    stream->setAutoFormatting(true);
    stream->writeStartDocument();
    stream->writeStartElement(URI, "yagf");
    stream->writeAttribute(URI, "version", VERSION);
    writeSettings();
    writePages();
    stream->writeEndDocument();
    f.flush();
    delete stream;
    f.close();
    Settings::instance()->addRecentProject(dir);
    deleteGarbageFiles();
    return true;
}

void ProjectSaver::writePages()
{
    PageCollection *pc = PageCollection::instance();
    if (pc->count())
        pc->storeCurrentIndex();
    for (int i =0; i < pc->count(); i++) {
        stream->writeStartElement(URI, "page");
        pc->makePageCurrent(i);
        stream->writeAttribute(URI, "image", copyFile(pc->fileName()));
        QFileInfo fi(pc->fileName());
        imagesSaved.append(fi.fileName());
        stream->writeAttribute(URI, "title", pc->originalFileName());
        stream->writeAttribute(URI, "deskewed", boolToString(pc->isDeskewed()));
        stream->writeAttribute(URI, "rotation", QString::number(pc->getRotation()));
        stream->writeAttribute(URI, "preprocessed", boolToString(pc->isPreprocessed()));
        stream->writeAttribute(URI, "cropped", boolToString(pc->isCropped()));
        stream->writeAttribute(URI, "rtext", pc->text());
        writeBlocks();
        stream->writeEndElement();
    }
    if (pc->count())
        pc->restoreCurrentIndex();

}

void ProjectSaver::writeBlocks()
{
    PageCollection *pc = PageCollection::instance();
    for (int i = 0; i < pc->blockCount(); i++) {
        stream->writeStartElement(URI, "block");
        Block b =pc->getBlock(i);
        if (b.blockNumber()>0)
            stream->writeAttribute(URI, "number", QString::number(b.blockNumber()));
        b = pc->scaleRect(b, 1);
        stream->writeAttribute(URI, "left", QString::number(b.left()));
        stream->writeAttribute(URI, "top", QString::number(b.top()));
        stream->writeAttribute(URI, "width", QString::number(b.width()));
        stream->writeAttribute(URI, "height", QString::number(b.height()));
        // stream->writeAttribute(URI, "language", "eng");
        stream->writeEndElement();
    }
}

void ProjectSaver::writeSettings()
{
    stream->writeStartElement(URI, "settings");
    Settings *settings = Settings::instance();
    QString engine;
    if (settings->getSelectedEngine() == UseCuneiform)
        engine = "cuneiform";
    if (settings->getSelectedEngine() == UseTesseract)
        engine = "tesseract";
    stream->writeAttribute(URI, "engine", engine);
    stream->writeAttribute(URI, "defaultlanguage", settings->getLanguage());
    stream->writeEndElement();
}

QString ProjectSaver::copyFile(const QString &source)
{
    QFileInfo fi(source);
    QString dir = fi.absolutePath();
    if (!dir.endsWith("/"))
        dir = dir + "/";
    QString base = fi.baseName();
    QString fileName = base+".ygf";
    if (dir == directory)
        return fileName;
    QString newName = directory + fileName;
    if (source.endsWith(".ygf", Qt::CaseInsensitive)) {
        if (QFile::copy(source, newName))
            return fileName;
        else {
            QFileInfo  fs(newName);
            return fs.fileName();
        }
    } else {
        QImage image(source);
        if (image.save(newName))
            return fileName;
        else
            return "";

    }
    return "";
}

void ProjectSaver::deleteGarbageFiles()
{
    QDir dir;
    dir.setPath(directory);
    QFileInfoList fil = dir.entryInfoList();
    foreach (QFileInfo fi, fil) {
        if (!imagesSaved.contains(fi.fileName()))
            if ((!fi.fileName().endsWith(".xml"))&&(!fi.fileName().endsWith(".old")))
                dir.remove(fi.filePath());
    }
}

ProjectLoader::ProjectLoader(QObject *parent):   QObject(parent)
{
}

bool ProjectLoader::load(const QString &dir)
{
    bool res = GlobalLock::instance()->lock();
    if (!res)
        return false;
    Unlocker unlocker(true);
    if (loadInternal(dir, "yagf_project.xml"))
            return true;
    return loadInternal(dir, "yagf_project.old");
}

bool ProjectLoader::readSettings()
{
    Settings *settings = Settings::instance();
    if (!readNextElement())
        return false;
    version = stream->attributes().value(URI, "version").toString();
    QStringRef n;

    while ((n = stream->name()) != "settings")
        if (!readNextElement())
            return false;
    QStringRef engine = stream->attributes().value(URI, "engine");
    if (engine == "tesseract")
        settings->setSelectedEngine(UseTesseract);
    if (engine == "cuneiform")
        settings->setSelectedEngine(UseCuneiform);
    emit engineChanged();
    QString language = stream->attributes().value(URI, "defaultlanguage").toString();
    if (!language.isEmpty())
        settings->setLanguage(language);
    emit languageChanged();
    return true;
}

int versionToInt(const QString &version)
{
    int res = 0;
    for (int i = 0; i < version.count(); i++)
        if (version.at(i).isDigit()) {
            res *=10;
            res += version.at(i).digitValue();
        }
    return res;
}

void ProjectLoader::loadPage()
{
    QString image = stream->attributes().value(URI, "image").toString();
    QString fn = directory + image;
    filesLoaded.append(image);
    bool oldcl = Settings::instance()->getCropLoaded();
    Settings::instance()->setCropLoaded(false);
    PageCollection *pc = PageCollection::instance();
    Settings::instance()->setCropLoaded(oldcl);
    //pc->appendPage(fn);
    QString value = stream->attributes().value(URI, "rotation").toString();
    bool deskewed = false;
    bool preprocessed =false;
    bool cropped = false;
    qreal rotation = 0;
    if (!value.isEmpty()) {
        rotation = (value.toDouble());
    }
    value = stream->attributes().value(URI, "deskewed").toString();
    if (!value.isEmpty()) {
        deskewed = value.endsWith("true", Qt::CaseInsensitive) ? true : false;
    }
    value = stream->attributes().value(URI, "cropped").toString();
    if (!value.isEmpty()) {
        cropped = value.endsWith("true", Qt::CaseInsensitive) ? true : false;
    }
    value = stream->attributes().value(URI, "preprocessed").toString();
    if (!value.isEmpty()) {
        preprocessed = (value.endsWith("true", Qt::CaseInsensitive) ? true : false);
    }
    QString title = stream->attributes().value(URI, "title").toString();

    Page * page = pc->newPage(fn,rotation,preprocessed, deskewed, cropped);
    if (versionToInt(version) >= 94) {
        value = stream->attributes().value(URI, "rtext").toString();
        if (page) {
            page->setRecognizedText(value);
            page->setOriginalFileName(title);
        }
    }
}

bool ProjectLoader::readPages()
{
    if (!readNextElement())
        return false;
    QString name;
    if ((name = stream->name().toString()) != "page")
        return false;
    while (stream->name() == "page") {
        loadPage();

        if (!readBlocks())
            break;
    }
    PageCollection::instance()->reloadPage();
    return true;

}

bool ProjectLoader::readBlocks()
{
    if (!readNextElement())
        return false;
    while (stream->name() == "block") {
        int top = stream->attributes().value(URI, "top").toString().toInt();
        int left = stream->attributes().value(URI, "left").toString().toInt();
        int width = stream->attributes().value(URI, "width").toString().toInt();
        int height = stream->attributes().value(URI, "height").toString().toInt();
        int bn = stream->attributes().value(URI, "number").toString().toInt();
        QRect r(left, top, width, height);
       // r = PageCollection::instance()->scaleRect(r);
        r = PageCollection::instance()->scaleRectToPage(r);
        if (bn > 0)
            PageCollection::instance()->addBlock(r, bn);
        else
            PageCollection::instance()->addBlock(r);
        if (!readNextElement())
            return false;
    }
    return true;
}

bool ProjectLoader::readNextElement()
{
    while (!stream->readNextStartElement())
        if (stream->atEnd())
            return false;
    return true;
}

bool ProjectLoader::loadInternal(const QString &dir, const QString &fn)
{
    directory = dir;
    if (!directory.endsWith("/")) directory = directory + "/";
    QString fileName = directory+fn;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    stream = new QXmlStreamReader(&f);
    stream->setNamespaceProcessing(true);
    if (!readSettings())
        return false;
    if (!readPages())
        return false;
    f.close();
    Settings::instance()->addRecentProject(dir);
    deleteGarbageFiles();
    return true;
}

void ProjectLoader::deleteGarbageFiles()
{
    QDir dir;
    dir.setPath(directory);
    QFileInfoList fil = dir.entryInfoList();
    foreach (QFileInfo fi, fil) {
        if (!filesLoaded.contains(fi.fileName())) {
            QString bm =fi.fileName();
            if (!bm.endsWith(".xml"))
                dir.remove(fi.filePath());
        }
    }
}
