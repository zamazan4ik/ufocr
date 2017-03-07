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

#ifndef RECOGNIZERWRAPPER_H
#define RECOGNIZERWRAPPER_H

#include <QObject>
#include "tblock.h"

class QString;

class RecognizerWrapperInternal;

class RecognizerWrapper : public QObject
{
Q_OBJECT
public:
    explicit RecognizerWrapper(QObject* parent = 0);

    ~RecognizerWrapper();

    static bool findEngine(bool selected = false);

signals:

    void started();

    void finished(int);

    void error(const QString& text);

    void blockRecognized(int n);

    void readOutput(QString text, QChar separator);

public slots:

    void start();

    void startSingleBlock();

    void cancel();

private slots:

    void onError();

private:
    RecognizerWrapperInternal* rwi;
};

#endif // RECOGNIZERWRAPPER_H
