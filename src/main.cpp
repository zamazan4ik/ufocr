/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009-2015 Andrei Borovsky <borovsky.andrei@gmail.com>

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
#include "mainform.h"
#include "settings.h"
#include "langselectdialog.h"

//#define MEM_DEBUG
#ifdef MEM_DEBUG
#include <mcheck.h>
#endif

void parseCmdLine(const QStringList &args)
{
    foreach (const QString &arg, args) {
        if (arg == "-h" || arg == "--help") {
            printf("Using:\n"
                   "  yagf\n"
                   "  yagf <Key>\n"
                   "  yagf <file name> [file name [file name]...]\n"
                   "YAGF is a graphical interface for cuneiform and tesseract.\n"
                   "\n"
                   "Keys:\n"
                   "  -h, --help\t Show this message and exit\n"
                   "  -V, --version\t Show version string and exit\n");
            exit(0);
        } else if (arg == "-V" || arg == "--version") {
            printf("YAGF version: %s\n", version.toUtf8().constData());
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
#ifdef MEM_DEBUG
    mtrace();
#endif

    QApplication app(argc, argv);
    parseCmdLine(app.arguments());
    Settings *settings = Settings::instance();
    settings->readSettings(settings->workingDir());
    settings->writeSettings();
    QTranslator translator;
    QString qmName = "yagf_" + QLocale::system().name();
    if (!settings->useNoLocale()) {
        translator.load(qmName, QString(QML_INSTALL_PATH));
        app.installTranslator(&translator);

    }
    settings->makeLanguageMaps();
    QTranslator translator2;
    translator2.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!settings->useNoLocale())
        app.installTranslator(&translator2);
    MainForm window;
    window.show();
    int res = app.exec();
#ifdef MEM_DEBUG
    muntrace();
#endif
    return res;
}
