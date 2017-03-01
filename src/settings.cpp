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

#include "settings.h"
#include "utils.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QLocale>
#include <QVariant>

Settings *Settings::m_instance = NULL;

Settings::Settings()
{
    //makeLanguageMaps();
}

Settings::Settings(const Settings &)
{
    upscale = false;
}

Settings::~Settings()
{
}

Settings *Settings::instance()
{
    if (!m_instance)
        m_instance = new Settings();
    return m_instance;
}

void Settings::readSettings(const QString &path)
{
    mPath = path;
    mPath = mPath.append("yagf.ini");
    settings = new QSettings(mPath, QSettings::IniFormat);
    version = settings->value("program/version").toString();
    if (version.isEmpty())
        fr = true;
    else
        fr = false;
    lastDir = settings->value("mainwindow/lastDir").toString();
    lastOutputDir = settings->value("mainwindow/lastOutputDir").toString();
    QString defEngine;
    if (findProgram("tesseract"))
        defEngine = "tesseract";
    else
        defEngine = "cuneiform";
    QString engine = settings->value("ocr/engine", QVariant(defEngine)).toString();
    if (engine == "cuneiform")
        selectedEngine = UseCuneiform;
    else
        selectedEngine = UseTesseract;
    language = settings->value("ocr/language",  selectDefaultLanguageName()).toString();
    //selectLangsBox->setCurrentIndex(selectLangsBox->findData(QVariant(language)));
    outputFormat = settings->value("ocr/outputFormat", QString("text")).toString();
    if (outputFormat == "") outputFormat = "text";
    checkSpelling = settings->value("mainWindow/checkSpelling", bool(true)).toBool();
    bool ok;
    fontSize = settings->value("mainwindow/fontSize", int(12)).toInt(&ok);
    noLocale = settings->value("mainwindow/nolocale", QVariant(false)).toBool();
    RussianLocale = settings->value("mainwindow/rulocale", QVariant(false)).toBool();
    findTessDataPath();
    tessdataPath = settings->value("ocr/tessData", QVariant(tessdataPath)).toString();
    keepLines = settings->value("ocr/keepLines", QVariant(false)).toBool();
    if (tessdataPath.isEmpty())
        findTessDataPath();
    languages = settings->value("ocr/selectedLanguages", QStringList(language)).toStringList();
    rowBegin = settings->value("ocr/rowBegin", QVariant(QString("["))).toString();
    rowEnd = settings->value("ocr/rowEnd", QVariant(QString("]"))).toString();
    cellSeparator =settings->value("ocr/cellSeparator", QVariant("|")).toString();
    rowFromNewLine = settings->value("ocr/rowFromNewLine", QVariant(true)).toBool();
    cropLoaded =  settings->value("processing/crop1", QVariant(true)).toBool();
    autoDeskew =  settings->value("processing/deskew", QVariant(true)).toBool();
    preprocess = settings->value("processing/preprocess", QVariant(true)).toBool();
    doublePreprocess = settings->value("processing/dpreprocess", QVariant(false)).toBool();
    size = settings->value("mainwindow/size", QSize(800, 600)).toSize();
    iconSize = settings->value("mainwindow/iconSize", QSize(24, 24)).toSize();
    position = settings->value("mainwindow/pos", QPoint(0, 0)).toPoint();
    fullScreen = settings->value("mainwindow/fullScreen", QVariant(false)).toBool();
    darkBackgroundThreshold = settings->value("tweaks/darkBackgroundThreshold", QVariant(208)).toInt();
    foregroundBrightenFactor = settings->value("tweaks/foregroundBrightenFactor", QVariant(32)).toInt();
    globalBrightenFactor = settings->value("tweaks/globalBrightenFactor", QVariant(32)).toInt();
    globalDarkenFactor = settings->value("tweaks/globalDarkenFactor", QVariant(20)).toInt();
    globalDarkenThreshold = settings->value("tweaks/globalDarkenThreshold", QVariant(180)).toInt();
    skipWidth = settings->value("tweaks/skipWidth", QVariant(5)).toInt();
    uSeed = settings->value("tweaks/seed", QVariant(1)).toInt();
    tiffPS = settings->value("tweaks/tiffPageSize", QVariant(4000)).toString();
    tiffD = settings->value("tweaks/tiffDensity", QVariant(300)).toString();
    projectDir = settings->value("projects/storage", QVariant(QDir::homePath())).toString();
    recentProjects = settings->value("projects/recent", QVariant(QStringList())).toStringList();
    maxRecentProjects = settings->value("projects/maxRecent", QVariant(8)).toInt();
    autosaveInterval = settings->value("projects/autosaveInterval", QVariant(15)).toInt();
}

bool Settings::firstRun()
{
    return fr;
}

void Settings::writeSettings()
{
    settings->setValue("program/version", QString::fromUtf8("0.9.5"));
    settings->setValue("mainwindow/size", size);
    settings->setValue("mainwindow/iconSize", iconSize);
    settings->setValue("mainwindow/pos", position);
    settings->setValue("mainwindow/fullScreen", fullScreen);
    settings->setValue("mainwindow/lastDir", lastDir);
    settings->setValue("mainWindow/checkSpelling", checkSpelling);
    settings->setValue("mainwindow/lastOutputDir", lastOutputDir);
    settings->setValue("mainwindow/fontSize", fontSize);
    settings->setValue("mainwindow/nolocale", noLocale);
    settings->setValue("mainwindow/rulocale", RussianLocale);
    settings->setValue("ocr/language", language);
    for (int i = languages.count()-1; i >= 0; i--)
        if (languages.at(i).contains("+"))
            languages.removeAt(i);
    if (!languages.contains("digits"))
        languages << "digits";
    settings->setValue("ocr/selectedLanguages", languages);

    //settings->setValue("ocr/singleColumn", singleColumn);
    settings->setValue("ocr/outputFormat", outputFormat);
    QString engine = selectedEngine == UseCuneiform ? QString("cuneiform") : QString("tesseract");
    settings->setValue("ocr/engine", engine);
    settings->setValue("ocr/tessData", tessdataPath);
    settings->setValue("processing/crop1", cropLoaded);
    if (doublePreprocess)
        autoDeskew = true;
    settings->setValue("processing/deskew", autoDeskew);
    settings->setValue("processing/preprocess", preprocess);
    settings->setValue("processing/dpreprocess", doublePreprocess);
    settings->setValue("ocr/keepLines", keepLines);
    settings->setValue("ocr/rowBegin", rowBegin);
    settings->setValue("ocr/rowEnd", rowEnd);
    settings->setValue("ocr/cellSeparator", cellSeparator);
    settings->setValue("ocr/rowFromNewLine", rowFromNewLine);
    settings->setValue("tweaks/darkBackgroundThreshold", darkBackgroundThreshold);
    settings->setValue("tweaks/foregroundBrightenFactor", foregroundBrightenFactor);
    settings->setValue("tweaks/globalBrightenFactor", globalBrightenFactor);
    settings->setValue("tweaks/globalDarkenFactor", globalDarkenFactor);
    settings->setValue("tweaks/globalDarkenThreshold", globalDarkenThreshold);
    settings->setValue("tweaks/tiffPageSize", tiffPS);
    settings->setValue("tweaks/tiffDensity", tiffD);
    settings->setValue("tweaks/skipWidth", skipWidth);
    settings->setValue("tweaks/seed", uSeed);
    settings->setValue("projects/storage", projectDir);
    settings->setValue("projects/recent", recentProjects);
    settings->setValue("projects/maxRecent", maxRecentProjects);
    settings->setValue("projects/autosaveInterval", autosaveInterval);
    settings->sync();
}

QString Settings::getLanguage()
{
    return language;
}

QString Settings::getOutputFormat()
{
    return outputFormat;
}

QString Settings::getLastDir()
{
    return lastDir;
}

QString Settings::getLastOutputDir()
{
    return lastOutputDir;
}

bool Settings::getCheckSpelling()
{
    return checkSpelling;
}

QString Settings::getTessdataPath()
{
    if (!tessdataPath.endsWith("/"))
        tessdataPath = tessdataPath.append("/");
    return tessdataPath;
}

SelectedEngine Settings::getSelectedEngine()
{
    return selectedEngine;
}

QSize Settings::getSize()
{
    return size;
}

QPoint Settings::getPosition()
{
    return position;
}

bool Settings::getFullScreen()
{
    return fullScreen;
}

int Settings::getFontSize()
{
    return fontSize;
}

QStringList Settings::getRecentProjects()
{
    return this->recentProjects;
}

int Settings::getMaxRecentProjects()
{
    return maxRecentProjects;
}

void Settings::setMaxRecentProjects(const int value)
{
    maxRecentProjects = value;
}

QString Settings::getFullLanguageName(const QString &abbr)
{
    QMap<QString, QString> * map;
    if (selectedEngine == UseCuneiform)
        map = &cuMap;
    if (selectedEngine == UseTesseract)
        map = &tesMap;
    return map->key(abbr, "");
}

QString Settings::getFullLanguageName(const QString &abbr, const QString &engine)
{
    QMap<QString, QString> * map;
    if (engine == "cuneiform")
        map = &cuMap;
    if (engine == "tesseract") {
        map = &tesMap;
        if (abbr.contains("+")) {
            QStringList sl = abbr.split("+");
            return map->key(sl[0], "") + "+" + map->key(sl[1], "");
        }
    }
    return map->key(abbr, "");
}

QString Settings::getShortLanguageName(const QString &lang)
{
    QMap<QString, QString> * map;
    if (selectedEngine == UseCuneiform)
        map = &cuMap;
    if (selectedEngine == UseTesseract)
        map = &tesMap;
    return map->value(lang, "");
}

QString Settings::getShortLanguageName(const QString &lang, const QString &engine)
{
    QMap<QString, QString> * map;
    if (engine == "tesseract")
        map = &tesMap;
    if (engine == "cuneiform")
        map = &cuMap;
    return map->value(lang, "");
}

bool Settings::getAutoDeskew()
{
    return autoDeskew;
}

bool Settings::getCropLoaded()
{
    return cropLoaded;
}

bool Settings::getPreprocessed()
{
    return preprocess;
}

bool Settings::getDoublePreprocessed()
{
    return doublePreprocess;
}

void Settings::setLanguage(const QString &value)
{
    language = value;
}

void Settings::setOutputFormat(const QString &value)
{
    outputFormat = value;
}

void Settings::setLastDir(const QString &value)
{
    lastDir = value;
}

void Settings::setLastOutputDir(const QString &value)
{
    lastOutputDir = value;
}

void Settings::setCheckSpelling(const bool value)
{
    checkSpelling = value;
}

void Settings::setTessdataPath(const QString &value)
{
    tessdataPath = value;
}

void Settings::setSelectedEngine(const SelectedEngine value)
{
    selectedEngine = value;
}

void Settings::setSize(const QSize &value)
{
    size = value;
}

void Settings::setPosition(const QPoint &value)
{
    position = value;
}

void Settings::setFullScreen(const bool value)
{
    fullScreen = value;
}

void Settings::setFontSize(const int &value)
{
    fontSize = value;
}

void Settings::setCropLoaded(const bool value)
{
    cropLoaded = value;
}

void Settings::setAutoDeskew(const bool value)
{
    autoDeskew = value;
}

void Settings::setPreprocessed(const bool value)
{
    preprocess = value;
}

void Settings::setDoublePreprocessed(const bool value)
{
    doublePreprocess = value;
}

void Settings::setUpscale(bool value)
{
    upscale = value;
}

bool Settings::getUpscale()
{
    return upscale;
}

void Settings::setSkipWidth(int value)
{
    skipWidth = value;
}

QString Settings::getRecognizeInputFile() const
{
    return "input.bmp";
}

QString Settings::getRecognizeOutputExt() const
{
    return ".txt";
}

QString Settings::getRecognizeOutputFile() const
{
    return "output.txt";
}

QString Settings::getRecognizeOutputBase() const
{
    return "output";
}

QString Settings::getScanOutputFile() const
{
    return "input.png";
}

int Settings::getSkipWidth()
{
    return skipWidth;
}

QString Settings::uniqueSeed()
{
    return QString::number(uSeed++);
}

QString Settings::tiffPageSize()
{
    return tiffPS;
}

QString Settings::tiffDensity()
{
    return tiffD;
}

int Settings::getDarkBackgroundThreshold()
{
    return darkBackgroundThreshold;
}

int Settings::getForegroundBrightenFactor()
{
    return foregroundBrightenFactor;
}

int Settings::getGlobalBrightenFactor()
{
    return globalBrightenFactor;
}

int Settings::getGlobalDarkenFactor()
{
    return globalDarkenFactor;
}

int Settings::getGlobalDarkenThreshold()
{
    return globalDarkenThreshold;
}

int Settings::getAutosaveInterval()
{
    return autosaveInterval;
}

QStringList Settings::fullLanguageNames()
{
    QStringList res = tesMap.keys();
    res << QObject::trUtf8("Russian-English");
    return res;
}

QStringList Settings::getSelectedLanguages()
{
    QStringList res;
    foreach (QString s, languages) {
        QString l = getFullLanguageName(s, "tesseract");
        if (l!="")
            res.append(l);
        l = getFullLanguageName(s, "cuneiform");
        if (l!="")
            res.append(l);
    }
    res.removeDuplicates();
    return res;
}

QStringList Settings::selectedLanguagesAvailableTo(const QString &engine)
{
    QStringList res;
    if (engine == "cuneiform") {
        foreach(QString s, languages) {
            if (cuMap.values().contains(s))
                res.append(cuMap.key(s, ""));
        }
    }
    if (engine == "tesseract") {
        foreach(QString s, languages) {
            if (tesMap.values().contains(s))
                res.append(tesMap.key(s, ""));
        }
    }
    return res;
}

QStringList Settings::languagesAvailableTo(const QString &engine)
{
    if (engine == "cuneiform")
        return cuMap.keys();
    if (engine == "tesseract")
        return tesMap.keys();
    return QStringList();
}

QStringList Settings::installedTesseractLanguages()
{
    QString tessPath = tessdataPath + "/tessdata/";
    QDir d(tessPath);
    QStringList res;
    QStringList sl = d.entryList(QStringList("*.traineddata"));
    foreach (QString s, tesMap.values()) {
        foreach(QString s1, sl) {
            if (s1.startsWith(s))
                res.append(tesMap.key(s, ""));
        }
    }
    res.removeDuplicates();
    return res;
}


QString Settings::getRowBegin()
{
    return rowBegin;
}

QString Settings::getRowEnd()
{
    return rowEnd;
}

QString Settings::getCellSeparator()
{
    return cellSeparator;
}

bool Settings::getRowFromBNewLine()
{
    return rowFromNewLine;
}

void Settings::setSelectedLanguages(const QStringList &value)
{
    languages.clear();
    foreach (QString s, value) {
        QString l = getShortLanguageName(s, "tesseract");
        if (l!="") languages.append(l);
        l =getShortLanguageName(s, "cuneiform");
        if (l!="") languages.append(l);
    }
    languages.removeDuplicates();
}

QString Settings::workingDir()
{
    QString wDir = QDir::homePath();
    if (!wDir.endsWith("/"))
        wDir += '/';
    QDir d(wDir + ".config");
    if (d.exists()) wDir += ".config/";
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    wDir = env.value("XDG_CONFIG_HOME", wDir);
    if (!wDir.endsWith("/"))
        wDir += '/';
    wDir += "yagf/";
    QDir dir(wDir);
    if (!dir.exists()) {
        dir.mkdir(wDir);
        fr = true;
    } else fr= false;
    return wDir;
}

void Settings::startLangPair()
{
    lpi = 0;
}

bool Settings::getLangPair(QString &full, QString &abbr, bool forceTesseract)
{
    QMap<QString, QString> * map;
    if (selectedEngine == UseCuneiform)
        map = &cuMap;
    if (selectedEngine == UseTesseract)
        map = &tesMap;
    if (forceTesseract)
        map = &tesMap;
    if (lpi < map->count()) {
        full = map->keys().at(lpi);
        abbr = map->value(full);
        lpi++;
        return true;
    }
    return false;
}

void Settings::setProjectDir(const QString &dir)
{
    projectDir = dir;
}

QString Settings::getProjectDir()
{
    return projectDir;
}

void Settings::makeLanguageMaps()
{
    cuMap.insert(QObject::trUtf8("Bulgarian"), "bul");
    cuMap.insert(QObject::trUtf8("Czech"), "cze");
    cuMap.insert(QObject::trUtf8("Danish"), "dan");
    cuMap.insert(QObject::trUtf8("Dutch"), "dut");
    cuMap.insert(QObject::trUtf8("English"), "eng");
    cuMap.insert(QObject::trUtf8("French"), "fra");
    cuMap.insert(QObject::trUtf8("German"), "ger");
    cuMap.insert(QObject::trUtf8("Hungarian"), "hun");
    cuMap.insert(QObject::trUtf8("Italian"), "ita");
    cuMap.insert(QObject::trUtf8("Latvian"), "lav");
    cuMap.insert(QObject::trUtf8("Lithuanian"), "lit");
    cuMap.insert(QObject::trUtf8("Polish"), "pol");
    cuMap.insert(QObject::trUtf8("Portuguese"), "por");
    cuMap.insert(QObject::trUtf8("Romanian"), "rum");
    cuMap.insert(QObject::trUtf8("Russian"), "rus");
    cuMap.insert(QObject::trUtf8("Russian-English"), "ruseng");
    cuMap.insert(QObject::trUtf8("Spanish"), "spa");
    cuMap.insert(QObject::trUtf8("Serbian"), "srp");
    cuMap.insert(QObject::trUtf8("Slovenian"), "slo");
    cuMap.insert(QObject::trUtf8("Swedish"), "swe");
    cuMap.insert(QObject::trUtf8("Ukrainian"), "ukr");
    tesMap.insert(QObject::trUtf8("Afrikaans"), "afr");
    tesMap.insert(QObject::trUtf8("Albanian"), "sqi");
    tesMap.insert(QObject::trUtf8("Ancient Greek"), "grc");
    tesMap.insert(QObject::trUtf8("Arabic"), "ara");
    tesMap.insert(QObject::trUtf8("Azerbaijani"), "aze");
    tesMap.insert(QObject::trUtf8("Belarusian"), "bel");
    tesMap.insert(QObject::trUtf8("Bulgarian"), "bul");
    tesMap.insert(QObject::trUtf8("Catalan"), "cat");
    tesMap.insert(QObject::trUtf8("Chinese (Simplified)"), "chi_sim");
    tesMap.insert(QObject::trUtf8("Chinese (Tradtional)"), "chi_tra");
    tesMap.insert(QObject::trUtf8("Croatian"), "hrv");
    tesMap.insert(QObject::trUtf8("Czech"), "ces");
    tesMap.insert(QObject::trUtf8("Danish"), "dan");
    tesMap.insert(QObject::trUtf8("Danish Gothic"), "dan-frak");
    tesMap.insert(QObject::trUtf8("Dutch"), "nld");
    tesMap.insert(QObject::trUtf8("English"), "eng");
    tesMap.insert(QObject::trUtf8("Esperanto"), "epo");
    tesMap.insert(QObject::trUtf8("Estonian"), "est");
    tesMap.insert(QObject::trUtf8("Finnish"), "fin");
    tesMap.insert(QObject::trUtf8("French"), "fra");
    tesMap.insert(QObject::trUtf8("Galician"), "glg");
    tesMap.insert(QObject::trUtf8("German"), "deu");
    tesMap.insert(QObject::trUtf8("German Gothic"), "deu-frak");
    tesMap.insert(QObject::trUtf8("Greek"), "ell");
    tesMap.insert(QObject::trUtf8("Hebrew"), "heb");
    tesMap.insert(QObject::trUtf8("Hindi"), "hin");
    tesMap.insert(QObject::trUtf8("Hungarian"), "hun");
    tesMap.insert(QObject::trUtf8("Indonesian"), "ind");
    tesMap.insert(QObject::trUtf8("Icelandic"), "isl");
    tesMap.insert(QObject::trUtf8("Italian"), "ita");
    tesMap.insert(QObject::trUtf8("Japanese"), "jpn");
    tesMap.insert(QObject::trUtf8("Kannada"), "kan");
    tesMap.insert(QObject::trUtf8("Korean"), "kor");
    tesMap.insert(QObject::trUtf8("Latvian"), "lav");
    tesMap.insert(QObject::trUtf8("Lithuanian"), "lit");
    tesMap.insert(QObject::trUtf8("Malay"), "msa");
    tesMap.insert(QObject::trUtf8("Macedonian"), "mkd");
    tesMap.insert(QObject::trUtf8("Middle English"), "enm");
    tesMap.insert(QObject::trUtf8("Middle French"), "frm");
    tesMap.insert(QObject::trUtf8("Norwegian"), "nor");
    tesMap.insert(QObject::trUtf8("Polish"), "pol");
    tesMap.insert(QObject::trUtf8("Portuguese"), "por");
    tesMap.insert(QObject::trUtf8("Romanian"), "ron");
    tesMap.insert(QObject::trUtf8("Russian"), "rus");
    tesMap.insert(QObject::trUtf8("Serbian"), "srp");
    tesMap.insert(QObject::trUtf8("Slovenian"), "slv");
    tesMap.insert(QObject::trUtf8("Slovakian"), "slk");
    tesMap.insert(QObject::trUtf8("Slovakian Gothic"), "slk-frak");
    tesMap.insert(QObject::trUtf8("Spanish"), "spa");
    tesMap.insert(QObject::trUtf8("Swahili"), "swa");
    tesMap.insert(QObject::trUtf8("Swedish"), "swe");
    tesMap.insert(QObject::trUtf8("Swedish Gothic"), "swe-frak");
    tesMap.insert(QObject::trUtf8("Thai"), "tha");
    tesMap.insert(QObject::trUtf8("Turkish"), "tur");
    tesMap.insert(QObject::trUtf8("Ukrainian"), "ukr");
    tesMap.insert(QObject::trUtf8("Vietnamese"), "vie");
    tesMap.insert(QObject::trUtf8("Digits Only"), "digits");
    setLangTess();
}

QString Settings::tmpFileName()
{
    return workingDir() + QString::fromUtf8("tmp-%1").arg(uniqueSeed());
}

QString Settings::tmpYGFFileName()
{
    return tmpFileName() + ".ygf";
}

void Settings::setLangTess()
{
    QStringList llanguages = languages;
    llanguages.removeAll("digits");
    int lc = llanguages.count();
    for (int i= 0; i < lc; i++) {
        for (int j = i+1; j < lc; j++) {
            if ((tesMap.values().contains(llanguages[j]))&&(tesMap.values().contains(llanguages[i]))) {
                QString nl = llanguages[i] + "+" + llanguages[j];
                languages.append(nl);
                tesMap.insert(getFullLanguageName((nl), "tesseract"), nl);
            }
        }
    }
}

bool Settings::getKeepLines() const
{
    return keepLines;
}

void Settings::setKeepLines(const bool value)
{
    keepLines = value;
}

void Settings::setRowBegin(const QString &value)
{
    rowBegin = value;
}

void Settings::setRowEnd(const QString &value)
{
    rowEnd = value;
}

void Settings::setCellSeparator(const QString &value)
{
    cellSeparator = value;
}

void Settings::setRowFromBNewLine(const bool value)
{
    rowFromNewLine = value;
}

void Settings::addRecentProject(const QString &project)
{
    if (project.contains("autosave"))
        return;
    for(int i = recentProjects.count()-1; i >= 0; i--) {
        QString p = recentProjects.at(i);
        if (!projectExists(p))
            recentProjects.removeAll(p);
    }

    if(!recentProjects.contains(project)) {
        recentProjects.prepend(project);
        if (recentProjects.count() >= maxRecentProjects)
            recentProjects.removeLast();
    } else {
       int i = recentProjects.indexOf(project);
       if (i > 0)
           recentProjects.swap(i, i-1);
    }
}

void Settings::removeRecentProject(const QString &project)
{
    recentProjects.removeAll(project);
}

void Settings::setAutosaveInterval(const int value)
{
    autosaveInterval = value;
}

bool Settings::projectExists(const QString &dir)
{
    QString project = dir + "yagf_project.xml";
    QFileInfo fi;
    fi.setFile(project);
    return fi.exists();
}

void Settings::findTessDataPath()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains("TESSDATA_PREFIX")) {
        tessdataPath = env.value("TESSDATA_PREFIX");
        if (tessdataPath.contains("$HOME")){
            QString hp = QDir::homePath() +"/";
            tessdataPath = tessdataPath.replace("$HOME", hp);
            tessdataPath = tessdataPath.replace("//", "/");
        }
        return;
    }
    QDir dir;
    dir.setPath("/usr/share/tessdata");
    if (dir.exists()) {
        tessdataPath = "/usr/share/";
        return;
    }
    dir.setPath("/usr/local/share/tessdata");
    if (dir.exists()) {
        tessdataPath = "/usr/local/share/";
        return;
    }
    dir.setPath("/usr/local/share/tesseract-ocr/tessdata");
    if (dir.exists()) {
        tessdataPath = "/usr/local/share/tesseract-ocr/";
        return;
    }
    dir.setPath("/usr/share/tesseract-ocr/tessdata");
    if (dir.exists()) {
        tessdataPath = "/usr/share/tesseract-ocr/";
        return;
    }
    tessdataPath.clear();
    return;
}

QString Settings::selectDefaultLanguageName()
{
    QLocale loc = QLocale::system();
    QString name = "";
    QMap<QString, QString> * map;
    if (selectedEngine == UseCuneiform)
        map = &cuMap;
    if (selectedEngine == UseTesseract)
        map = &tesMap;
    switch (loc.language()) {
        case QLocale::Bulgarian:
            name = map->value("Bulgarian");
            break;
        case QLocale::Czech:
            name = map->value("Czech");
            break;
        case QLocale::Danish:
            name = map->value("Danish");
            break;
        case QLocale::German:
            name = map->value("German");
            break;
        case QLocale::Dutch:
            name = map->value("Dutch");
            break;
        case QLocale::Russian:
            name = map->value("Russian");
            break;
        case QLocale::English:
            name = "eng";
            break;
        case QLocale::Spanish:
            name = map->value("Spanish");
            break;
        case QLocale::French:
            name = map->value("French");
            break;
        case QLocale::Hungarian:
            name = map->value("Hungarian");
            break;
        case QLocale::Italian:
            name = map->value("Italian");
            break;
        case QLocale::Latvian:
            name = map->value("Latvian");
            break;
        case QLocale::Lithuanian:
            name = map->value("Lithuanian");
            break;
        case QLocale::Polish:
            name = map->value("Polish");
            break;
        case QLocale::Portuguese:
            name = map->value("Portuguese");
            break;
        case QLocale::Romanian:
            name = map->value("Romanian");
            break;
        case QLocale::Swedish:
            name = map->value("Swedish");
            break;
        case QLocale::Serbian:
            name = map->value("Serbian");
            break;
        case QLocale::Slovenian:
            name = map->value("Slovenian");
            break;
        case QLocale::Slovak:
            name = map->value("Slovak", "eng");
            break;
        case QLocale::Ukrainian:
            name = map->value("Ukrainian");
            break;
        case QLocale::Finnish:
            name = map->value("Finnish", "eng");
            break;
        case QLocale::Greek:
            name = map->value("Greek", "eng");
            break;
        case QLocale::Hebrew:
            name = map->value("Hebrew", "eng");
            break;
        case QLocale::Norwegian:
            name = map->value("Norwegian", "eng");
            break;
        case QLocale::Turkish:
            name = map->value("Turkish", "eng");
            break;
        default:
            name = "eng";
            break;
    }
    if (name == "")
        name = "eng";
    return name;
}

QSize Settings::getIconSize()
{
    return iconSize;
}

void Settings::setIconSize(const QSize &value)
{
    iconSize = value;
}

bool Settings::useNoLocale()
{
    return noLocale;
}

bool Settings::useRussianLocale()
{
    return RussianLocale;
}

void Settings::setNoLocale(bool value)
{
    noLocale = value;
}

void Settings::setRussianLocale(bool value)
{
    RussianLocale = value;
}
