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

#include <QObject>
#include <QStringList>


class QXmlStreamWriter;
class QXmlStreamReader;

class ProjectSaver : public QObject
{
    Q_OBJECT
public:
    explicit ProjectSaver(QObject *parent = 0);
    bool save(const QString &dir);
signals:

public slots:
private:
    void writePages();
    void writeBlocks();
    void writeSettings();
    QString copyFile(const QString &source);
private:
    QXmlStreamWriter *stream;
    QString directory;
    QStringList imagesSaved;
private:
   void deleteGarbageFiles();
};

class ProjectLoader : public QObject
{
    Q_OBJECT
public:
    explicit ProjectLoader(QObject *parent = 0);
    bool load(const QString &dir);
signals:
    void languageChanged();
    void engineChanged();
public slots:
private:
    bool readPages();
    bool readBlocks();
    bool readSettings();
    void loadPage();
    bool readNextElement();
private:
    QXmlStreamReader *stream;
    QString directory;
    QString version;
    QStringList filesLoaded;
private:
    bool loadInternal(const QString &dir, const QString &fn);
    void deleteGarbageFiles();
};
