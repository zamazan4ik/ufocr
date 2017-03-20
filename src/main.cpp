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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <stdio.h>
#include <QLibraryInfo>
#include <QProcessEnvironment>
#include <QDir>
#include "mainform.h"
#include "settings.h"
#include "langselectdialog.h"
#include "logger.hpp"

void parseCmdLine(const QStringList& args)
{
    for (const QString& arg : args)
    {
        if (arg == "-h" || arg == "--help")
        {
            printf("Using:\n"
                           "  ufocr\n"
                           "  ufocr <Key>\n"
                           "  ufocr <file name> [file name [file name]...]\n"
                           "UFOCR is a program for OCR.\n"
                           "\n"
                           "Keys:\n"
                           "  -h, --help\t Show this message and exit\n"
                           "  -v, --version\t Show version string and exit\n");
            exit(0);
        }
        else if (arg == "-v" || arg == "--version")
        {
            printf("UFOCR version: %s\n", version.toUtf8().constData());
            exit(0);
        }
    }
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    parseCmdLine(app.arguments());
    Settings* settings = Settings::instance();
    settings->readSettings(settings->workingDir());
    settings->writeSettings();
    //Init logger
    QDir(settings->workingDir()).mkdir("logs");
    logger = spdlog::basic_logger_mt("MainLog", createLogFilename(settings->workingDir().toStdString() + "logs/"));
    logger->info("Start program");

    QTranslator translator;
    QString qmName = "yagf_" + QLocale::system().name();
    if (!settings->useNoLocale())
    {
        translator.load(qmName, QString(QML_INSTALL_PATH));
        app.installTranslator(&translator);
    }
    settings->makeLanguageMaps();
    QTranslator translator2;
    translator2.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!settings->useNoLocale())
    {
        app.installTranslator(&translator2);
    }
    MainForm window;
    window.show();
    int res = app.exec();
    logger->info("End program");
    return res;
}
