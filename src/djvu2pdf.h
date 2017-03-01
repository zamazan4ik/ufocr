/*
   YAGF - cuneiform and tesseract OCR graphical front-end
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

#ifndef DJVU2PDF_H
#define DJVU2PDF_H

#include <QObject>
#include <QProcess>

class Djvu2PDF : public QObject
{
    Q_OBJECT
public:
    explicit Djvu2PDF(QObject *parent = 0);
    ~Djvu2PDF();
    void convert(const QString &fileName);
    QString pdfName() const;
signals:
    void started();
    void finished();
    void error(const QString &text);
public slots:
    void onError();
    void cancel();
    void onFinished();
private:
    QProcess process;
    QString pdfout;
private:
    void clearFiles();
};

#endif // DJVU2PDF_H
