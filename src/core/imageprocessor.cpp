/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2014 Andrei Borovsky <anb@symmetrica.net>

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

#include "imageprocessor.h"
#include "qipgrayscaleimage.h"
#include "qipblackandwhiteimage.h"
#include "subimagepp.h"
#include "settings.h"
#include "tableanalizer.h"
#include <QImage>
#include <QMap>
#include <QVector>
#include <QImageReader>
#include <QImageWriter>

const uint m_treshold = 0;

ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)

{
    //img = NULL;
    //bwimg = NULL;
}

ImageProcessor::~ImageProcessor()
{
}

QRect ImageProcessor::crop()
{
    QRect r;
    try {
        QIPBlackAndWhiteImage bwimg1 = QIPBlackAndWhiteImage(img.binarize(QIPGrayscaleImage::OtsuMABinarization));
        r = bwimg1.cropGrayScaleImage(img);
        //bwimg1.free();
        img = img.copy(r.x(), r.x()+r.width(), r.y(), r.y()+r.height());
    } catch(...) {
        r.setX(0);
        r.setY(0);
        r.setWidth(img.width());
        r.setHeight(img.height());
    }

    return r;
}

void ImageProcessor::loadImage(const QImage &image)
{
    img = QIPGrayscaleImage(image,QIPGrayscaleImage::MinValue);
}

QImage ImageProcessor::loadFromFile(const QString &fn)
{
    if (!fn.endsWith(".ygf", Qt::CaseInsensitive)) {
        QImageReader ir(fn);
        if (!ir.canRead())
            return QImage(0,0,QImage::Format_ARGB32);
        int total = ir.size().width()*ir.size().height();
        if ( total > 0x8000000) {
            ir.setScaledSize(QSize(ir.size().width()/2, ir.size().height()/2));
        }

        return ir.read();
    }
    else
    {
        ImageProcessor ipx;
        return ipx.loadYGF(fn);
    }
}

QImage ImageProcessor::gsImage() const
{
    return img.toImage();
}

void ImageProcessor::binarize()
{
    img.smoother();
    QIPBlackAndWhiteImage bwimg = img.binarize(QIPGrayscaleImage::GatosBinarization);
    img.blendImage(bwimg);
    bwimg.width();
    //img->darken(235);
}

void ImageProcessor::altBinarize()
{
    img.smoother();
    //QIPBlackAndWhiteImage bwimg = img.binarize(QIPGrayscaleImage::BradleyBinarization);
    QIPBlackAndWhiteImage bwimg = img.binarize(QIPGrayscaleImage::BernsenBinarization);
    img.blendImage(bwimg);
}

QIPBlackAndWhiteImage ImageProcessor::fastBinarize()
{
    //img.smoother();
    return img.binarize(QIPGrayscaleImage::BradleyBinarization);
}

void ImageProcessor::saveForPDF(const QImage &image, const QString &fileName, int squish)
{
    QIPGrayscaleImage img;
    if (squish > 1) {
        QImage im = image.scaled(image.width()/squish, image.height()/squish, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        img = QIPGrayscaleImage::fromImage(im, QIPGrayscaleImage::FastConversion);
    }
    else
        img = QIPGrayscaleImage::fromImage(image, QIPGrayscaleImage::FastConversion);

    //img.smoother();
    QIPBlackAndWhiteImage bwimg = img.binarize(QIPGrayscaleImage::GatosBinarization);
    QString filename = Settings::instance()->workingDir() + fileName;
    bwimg.toImage().save(filename);
}

void ImageProcessor::saveYGF(const QImage &image, const QString &fileName)
{
    //QIPGrayscaleImage gsi(image, QIPGrayscaleImage::MinValue);
    QIPGrayscaleImage::saveGrayscale(image, fileName);
}

QImage ImageProcessor::loadYGF(const QString &fileName)
{
    QIPGrayscaleImage gsi(fileName);
    return gsi.toImage();
}

void ImageProcessor::polishImage(QImage &image)
{
    for(int y =1; y < image.height()-1; y++) {
        QRgb * line = (QRgb *) image.scanLine(y);
        QRgb * linep = (QRgb *) image.scanLine(y-1);
        QRgb * linen = (QRgb *) image.scanLine(y+1);
        //if
        for (int x = 1; x < image.width()-1; x++) {
            if (line[x]%255 > 64) {
                if ((line[x-1]%255 < 64)&&(line[x+1]%255 < 64))
                    line[x] = 0xFF404040;
                else
                if ((linep[x]%255 < 64)&&(linen[x]%255 < 64))
                    line[x] = 0xFF404040;
                else
                    if ((linep[x-1]%255 < 64)&&(linen[x+1]%255 < 64))
                        line[x] = 0xFF404040;
                else
                        if ((linep[x+1]%255 < 64)&&(linen[x-1]%255 < 64))
                            line[x] = 0xFF404040;
            }
        }
    }

}

void ImageProcessor::polishImage2(QImage &image)
{

    /*for (int y = 0; y < image.height(); y++) {
        quint8 * line = image.scanLine(y);
        uint strokLen = 24;
        for (int x = 0; x < image.width()*4 - strokLen*4; x+=strokLen*2) {
            int strokes[8] = {0};
            int xvals[8] = {0};
            int curstrok = 0;
            for (int t = x+4; t < x+strokLen*4; t+=4) {
                if (abs(line[t]-line[t-4]) > strokes[curstrok]) {
                        strokes[curstrok] = line[t]-line[t-4];
                        xvals[curstrok] = t;
                }
                else
                    curstrok++;
                if (curstrok >= 8)
                    break;
            }
            if (curstrok < 4)
                break;
            int start = -1;
            int stop = -1;
            int k = 0;
            while (k < curstrok -1) {
                if (((strokes[k] < 0)&&(strokes[k+1] < 0))||((strokes[k] > 0)&&(strokes[k+1] > 0))) {
                    strokes[k] += strokes[k+1];
                    xvals[k] = xvals[k+1];
                    for (int l = k+1; l < curstrok-1; l++)
                        strokes[l] = strokes[l+1];
                    curstrok--;
                } else k++;
            }
            if (curstrok < 4)
                break;
            for (int m = 1; m < curstrok-1; m++) {
                if (start < 0) {
                    if (strokes[m-1] < -15)
                        if (strokes[m] > 7)
                            if (-strokes[m-1] > strokes[m])
                                start = xvals[m];
                } else {
                    if (strokes[m-1] < -7)
                        if (strokes[m] > 15)
                            if (strokes[m] > -strokes[m-1]) {
                                stop = xvals[m];
                                break;
                            }

                }
                if (stop > 0) {
                    for (int p = start; p < stop; p +=4)
                        line[p] = 60;
                }

            }
        }
    }*/
}

bool ImageProcessor::isTextHorizontal(QImage &image)
{
    return true;
    if (image.width() > image.height())
        return true;
    QMap<int, int> stripes;
    for (int y = 600; y < image.height() - 600; y++) {
        quint8 * line = image.scanLine(y);
        for (int x = 1200; x < image.width()*4 - 1200; x+=160) {
            if (stripes.contains(x)) {
                if (line[x] > 128)
                    stripes.insert(x, stripes.value(x)+1);
                else
                    if (stripes.value(x) < 600)
                        stripes.insert(x, 0);
            } else
                stripes.insert(x, 0);
        }
    }
    int longCount = 0;
    int shortCount = 0;
    foreach (int count, stripes.values()) {
        if (count >= 600)
            longCount++;
        else
            shortCount++;
    }
    if (longCount*2 > shortCount)
        return false;
    if (longCount*3 > shortCount)
        return true;
    return true;
}

void ImageProcessor::cropAngles(QImage &image)
{
//    int prevLen = 0;
    for (int y = 0; y < image.height(); y++) {
        QRgb * line = (QRgb *) image.scanLine(y);
        for (int x = 0; x < image.width(); x++) {
            if ((line[x]&0x00FFFFFF) == 0)
                line[x] =0xFFFFFFFF;
            else
                break;
        }
        for (int x = image.width()-1; x > 0; x--) {
            if ((line[x]&0x00FFFFFF) == 0)
                line[x] =0xFFFFFFFF;
            else
                break;
        }

    }
    for (int x = 0; x < image.width(); x++) {
        for (int y = 0; y < image.height(); y++) {
            QRgb * line = (QRgb *) image.scanLine(y);
            if ((line[x]&0x00FFFFFF) == 0)
                line[x] =0xFFFFFFFF;
            else
                break;
        }
        for (int y = image.height()-1; y > 0 ; y--) {
            QRgb * line = (QRgb *) image.scanLine(y);
            if ((line[x]&0x00FFFFFF) == 0)
                line[x] =0xFFFFFFFF;
            else
                break;
        }
    }
}

void ImageProcessor::flatten()
{
    int tr = Settings::instance()->getDarkBackgroundThreshold();
    for (int y = 0; y < img.height(); y++) {
        quint8 * line = img.scanLine(y);
        for (int x = 0; x < img.width(); x++) {
            line[x] = line[x] > tr ? 255 : line[x];
        }
    }
}

void ImageProcessor::bust(QImage &image)
{
    for (int y = 0; y < image.height(); y++) {
        QRgb * line = (QRgb *) image.scanLine(y);
        for (int x = 0; x < image.width(); x++) {
            quint8 ov = line[x] & 0x000000FF;
            quint32 np = (ov << 16) + (ov << 8) + ov + 0xFF000000;
            line[x] = np;
        }
    }
}

QImage ImageProcessor::upScale(const QImage &image, bool bolden)
{
    QImage scImage = image.scaled(image.width()*2, image.height()*2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    loadImage(scImage);
    binarize();
    if (bolden) {
        QImage limage  = gsImage();
        SubimagePP spp(limage);
        spp.stregthen();
        return limage;
    }
    else
        return gsImage();
}

QList<Rect> ImageProcessor::splitTable(const QRect &bounds)
{
   QRect brect = bounds;
   QIPBlackAndWhiteImage bwi = fastBinarize();
   normalizeBounds(brect, bwi.width(), bwi.height());
   TableAnalizer ti(&bwi);
   return ti.splitTable(brect);
}

QList<Rect> ImageProcessor::splitTableForce(const QRect &bounds)
{
    QRect brect = bounds;
    QIPBlackAndWhiteImage bwi = fastBinarize();
    normalizeBounds(brect, bwi.width(), bwi.height());
    TableAnalizer ti(&bwi);
    return ti.splitTableForce(brect);
}

QRect ImageProcessor::deskewByTable(const QRect &bounds)
{
    QRect brect = bounds;
    QIPBlackAndWhiteImage bwi = fastBinarize();
    normalizeBounds(brect, bwi.width(), bwi.height());
    TableAnalizer ti(&bwi);
    Rect r = ti.getSkew(brect);
    return QRect(r.x1, r.y1, r.x2-r.x1, r.y2-r.y1);
}

void ImageProcessor::normalizeBounds(QRect &bounds, int w, int h)
{
    if ((bounds.width() == 0)||(bounds.height() == 0)) {
        bounds.setX(0);
        bounds.setY(0);
        bounds.setWidth(w);
        bounds.setHeight(h);
    }
}


