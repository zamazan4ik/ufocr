/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "qipgrayscaleimage.h"
#include "utils.h"
#include <QObject>
#include <QImage>
#include <QList>

class QIPBlackAndWhiteImage;
class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = 0);
    ~ImageProcessor();
    QRect crop();
    void loadImage(const QImage &image);
    QImage loadFromFile(const QString &fn);
    QImage gsImage() const;
    void binarize();
    void altBinarize();
    QIPBlackAndWhiteImage fastBinarize();
    static void saveForPDF(const QImage &image, const QString &fileName, int squish = 1);
    void saveYGF(const QImage &image, const QString &fileName);
    QImage loadYGF(const QString &fileName);
    static void polishImage(QImage &image);
    static void polishImage2(QImage &image);
    static bool isTextHorizontal(QImage &image);
    static void cropAngles(QImage &image);
    void flatten();
    static void bust(QImage &image);
    QImage upScale(const QImage &image, bool bolden);
    QList<Rect> splitTable(const QRect &bounds);
    QList<Rect> splitTableForce(const QRect &bounds);
    QRect deskewByTable(const QRect &bounds);
signals:
    
public slots:

private:
    QIPGrayscaleImage img;
private:
    void normalizeBounds(QRect &bounds, int w, int h);
};

#endif // IMAGEPROCESSOR_H
