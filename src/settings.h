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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <QString>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QMap>
#include <QLocale>
#include <QStringList>


enum SelectedEngine {
    UseCuneiform,
    UseTesseract
};

class Settings
{
public:
    static Settings *instance();
    void readSettings(const QString &path);
    bool firstRun();
    void writeSettings();
    QString getLanguage();
    bool useNoLocale();
    void setNoLocale(bool value);
    void setRussianLocale(bool value);
    QString getOutputFormat();
    QString getLastDir();
    QString getLastOutputDir();
    bool getCheckSpelling();
    QString getTessdataPath();
    SelectedEngine getSelectedEngine();
    QSize getSize();
    QSize getIconSize();
    QPoint getPosition();
    bool getFullScreen();
    int getFontSize();
    QStringList getRecentProjects();
    int getMaxRecentProjects();
    void setMaxRecentProjects(const int value);
    QString getFullLanguageName(const QString &abbr);
    QString getFullLanguageName(const QString &abbr, const QString &engine);
    QString getShortLanguageName(const QString &lang);
    QString getShortLanguageName(const QString &lang, const QString &engine);
    bool getAutoDeskew();
    bool getCropLoaded();
    bool getPreprocessed();
    bool getDoublePreprocessed();
    void setLanguage(const QString &value);
    void setOutputFormat(const QString &value);
    void setLastDir(const QString &value);
    void setLastOutputDir(const QString &value);
    void setCheckSpelling(const bool value);
    void setTessdataPath(const QString &value);
    void setSelectedEngine(const SelectedEngine value);
    void setSize(const QSize &value);
    void setIconSize(const QSize &value);
    void setPosition(const QPoint &value);
    void setFullScreen(const bool value);
    void setFontSize(const int &value);
    void setCropLoaded(const bool value);
    void setAutoDeskew(const bool value);
    void setPreprocessed(const bool value);
    void setDoublePreprocessed(const bool value);
    void setUpscale(bool value);
    bool getUpscale();
    void setSkipWidth(int value);
    QString getRecognizeInputFile() const;
    QString getRecognizeOutputExt() const;
    QString getRecognizeOutputFile() const;
    QString getRecognizeOutputBase() const;
    QString getScanOutputFile() const;
    int getSkipWidth();
    QString uniqueSeed();
    QString tiffPageSize();
    QString tiffDensity();
    int getDarkBackgroundThreshold();
    int getForegroundBrightenFactor();
    int getGlobalBrightenFactor();
    int getGlobalDarkenFactor();
    int getGlobalDarkenThreshold();
    int getAutosaveInterval();
    QStringList fullLanguageNames();
    QStringList getSelectedLanguages();
    QStringList selectedLanguagesAvailableTo(const QString &engine);
    QStringList languagesAvailableTo(const QString &engine);
    QStringList installedTesseractLanguages();
    QString getRowBegin();
    QString getRowEnd();
    QString getCellSeparator();
    bool getRowFromBNewLine();
    void setSelectedLanguages(const QStringList &value);
    QString workingDir();
    void startLangPair();
    bool getLangPair(QString &full, QString &abbr, bool forceTesseract = false);
    void setProjectDir(const QString &dir);
    QString getProjectDir();
    void makeLanguageMaps();
    QString tmpFileName();
    QString tmpYGFFileName();
    void setLangTess();
    bool getKeepLines() const;
    void setKeepLines(const bool value);
    void setRowBegin(const QString &value);
    void setRowEnd(const QString &value);
    void setCellSeparator(const QString &value);
    void setRowFromBNewLine(const bool value);
    void addRecentProject(const QString &project);
    void removeRecentProject(const QString &project);
    void setAutosaveInterval(const int value);
    bool projectExists(const QString &dir);
private:
    void findTessDataPath();
    QString selectDefaultLanguageName();
    Settings();
    Settings(const Settings &);
    bool useRussianLocale();
    ~Settings();
private:
    QString language;
    QString outputFormat;
    QString lastDir;
    QString lastOutputDir;
    QString projectDir;
    QString version;
    bool checkSpelling;
    QString tessdataPath;
    SelectedEngine selectedEngine;
    QSize size;
    QSize iconSize;
    QPoint position;
    bool fullScreen;
    int fontSize;
    bool cropLoaded;
    bool preprocess;
    bool fr;
    QMap<QString, QString> cuMap;
    QMap<QString, QString> tesMap;
    int lpi;
    bool noLocale;
    bool RussianLocale;
    bool autoDeskew;
    int darkBackgroundThreshold;
    int globalBrightenFactor;
    int foregroundBrightenFactor;
    int globalDarkenFactor;
    int globalDarkenThreshold;
    int uSeed;
    QStringList languages;
    QString mPath;
    QSettings *settings;
    static Settings *m_instance;
    QString tiffPS;
    QString tiffD;
    bool upscale;
    int skipWidth;
    bool keepLines;
    bool doublePreprocess;
    QString rowBegin;
    QString rowEnd;
    QString cellSeparator;
    bool rowFromNewLine;
    QStringList recentProjects;
    int maxRecentProjects;
    int autosaveInterval;
};

#endif
