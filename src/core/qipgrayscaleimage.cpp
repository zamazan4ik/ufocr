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

#include "settings.h"
#include "qipgrayscaleimage.h"
#include "binarize.h"
#include <cmath>
#include "common.h"
#include <QRect>
#include <QFile>
#include <QFuture>
#include <QtConcurrentRun>
#include <future>
#include <opencv2/imgproc.hpp>


const QString fheader = QString::fromUtf8("YGF1");

static const int kSharpen[3][3] = {{0,  -1, 0},
                                   {-1, 5,  -1},
                                   {0,  -1, 0}};
static const int wkSharpen = 1;
static const int kBlur[3][3] = {{1, 1, 1},
                                {1, 1, 1},
                                {1, 1, 1}};
static const int wkBlur = 5;
static const int kEdges[3][3] = {{-1, -1, -1},
                                 {-1, 8,  -1},
                                 {-1, -1, -1}};
static const int wkEdges = 1;

QIPGrayscaleImage::QIPGrayscaleImage(const QImage& image, GrayscaleConversion conversionMethod)/* : data(
        new quint8[image.width() * image.height()], deallocator<quint8>)*/
{
    //data = std::make_shared<quint8>(image.width() * image.height());
    data.resize(image.width() * image.height());
    h = image.height();
    w = image.width();
    //data = QSharedPointer<quint8>();
    switch (conversionMethod)
    {
        case RGBDevideByThree:
            toGrayScale(image);
            break;
        case MinMaxValue:
            toGrayscaleMinMax(image);
            break;
        case MinValue:
            toGrayscaleMinOrMax(image, true);
            break;
        case MaxValue:
            toGrayscaleMinOrMax(image, false);
            break;
        case MaxEntropyChannel:
            toGrayScaleByEntropyChannel(image, true);
            break;
        case MinEntropyChannel:
            toGrayScaleByEntropyChannel(image, false);
            break;
        case FastConversion:
            toGrayscaleFast(image);
        default:
            toGrayScale(image);
            break;
    }
}

QIPGrayscaleImage::QIPGrayscaleImage(QIPBlackAndWhiteImage& image1, QIPBlackAndWhiteImage& image2) /*: data(
        new quint8[image1.width() * image1.height()], deallocator<quint8>)*/
{
    //data = std::make_shared<quint8>(image1.width() * image1.height());
    w = std::min(image1.width(), image2.width());
    h = std::min(image1.height(), image2.height());
    data.resize(w * h);
    quint8* d1 = (quint8*)image1.data.data();
    quint8* d2 = (quint8*)image2.data.data();
    quint8* d = (quint8*)data.data();
    for (int i = 0; i < w * h; i++)
    {
        quint8 s = d1[i] + d2[i];
        switch (s)
        {
            case 0:
                d[i] = 0;
                break;
            case 1:
                d[i] = 128;
                break;
            case 2:
                d[i] = 255;
                break;
            default:
                break;
        }
    }
}

QIPGrayscaleImage
QIPGrayscaleImage::fromImage(const QImage& image, QIPGrayscaleImage::GrayscaleConversion conversionMethod)
{
    return QIPGrayscaleImage(image, conversionMethod);
}

bool QIPGrayscaleImage::isNull() const
{
    return w * h == 0;
}

QIPGrayscaleImage::QIPGrayscaleImage(const QIPGrayscaleImage& I)// : data(I.data.data(), deallocator<quint8>)
{
    data = I.data;
    w = I.w;
    h = I.h;
    FilterType = I.FilterType;
}

QIPGrayscaleImage& QIPGrayscaleImage::operator=(const QIPGrayscaleImage& I)// : data(I.data.data(), deallocator<quint8>)
{
    if(this == &I)
    {
        return *this;
    }
    w = I.w;
    h = I.h;
    data = I.data;
    FilterType = I.FilterType;
    return *this;
}

QIPGrayscaleImage::QIPGrayscaleImage(const QString& ygfFileName) : w(0), h(0)
{
    QFile f(ygfFileName);
    if (!f.open(QIODevice::ReadOnly))
    {
        return;
    }
    QPoint p = loadHeader(&f);
    w = p.x();
    h = p.y();
    if (w * h == 0)
    {
        f.close();
        return;
    }
    //data = std::make_shared<quint8>(w * h);
    data.resize(w * h);
    f.read((char*) data.data(), w * h);
    f.flush();
    f.close();
}

QIPGrayscaleImage::~QIPGrayscaleImage()
{
    //data.clear();
}

QImage QIPGrayscaleImage::toImage() const
{
    QImage image(w, h, QImage::Format_ARGB32);
    IntRect r = {0,0,image.width(),image.height()};
    toImageInternal(image.scanLine(0),r, image.width());
    return image;
}

void QIPGrayscaleImage::histogram(QIPHistogram& result, quint32 x1, quint32 x2, quint32 y1, quint32 y2) const
{
    if (x2 == 0)
    { x2 = w; }
    if (y2 == 0)
    { y2 = h; }
    IntRect r = {(int)x1, (int)y1, (int)x2, (int)y2};
    histogramInternal(&result[0], r);
}

void QIPGrayscaleImage::histogramInternal(qreal* result, const IntRect& r) const
{
    uint ht[256] = {0};
    for (int y = r.y1; y < r.y2; y++)
    {
        quint8* lineIn = scanLine(y);
        for (int x = r.x1; x < r.x2; x++)
        {
            ht[lineIn[x]]++;
        }
    }


    int size = (r.x2 - r.x1) * (r.y2 - r.y1);

    for (int i = 0; i < 256; i++)
    {
        result[i] = ht[i];
        result[i] /= size;
    }
}

void QIPGrayscaleImage::copyInternal(const IntRect& r, uint* image) const
{
    for (int y = r.y1; y < r.y2; y++)
    {
        quint8* line = scanLine(y);
        uint* lineout = &image[y * w];
        for (int x = r.x1; x < r.x2; x++)
            lineout[x] = line[x];
    }

}

void QIPGrayscaleImage::darken(quint8 factor)
{
    uint dataSize = w * h;
    quint8* d = (quint8*) data.data();
    for (uint i = 0; i < dataSize; i++)
    {
        d[i] = d[i] >> factor;
    }
}

void QIPGrayscaleImage::smoother()
{
    const int msn = 64; //increasing this increases smoothness
    const int thr = 90;
    quint8 steps[256] = {0};
    uint dataSize = w * h;
    quint8* d = (quint8*) data.data();
    for (uint i = 1; i < dataSize; i++)
    {
        quint8 diff = abs(d[i - 1] - d[i]);
        steps[diff]++;
    }
    int counter = 0;
    quint8 min_diff = 255;
    for (int i = 255; (i >= 0) && (counter < msn); i--)
    {
        if (steps[i] != 0)
        {
            min_diff = i;
            counter++;
        }
    }
    if (min_diff < thr)
    { min_diff = thr; }
    for (uint i = 1; i < dataSize; i++)
    {
        quint8 diff = abs(d[i - 1] - d[i]);
        if (diff >= min_diff)
        {
            if (d[i] > d[i - 1])
            {
                d[i] = d[i] - diff / 2;
            }
            else
            {
                d[i - 1] = d[i - 1] - diff / 2;
            }
        }
    }

}

void QIPGrayscaleImage::quantisize()
{
    //TODO: REWRITE
    uint dataSize = w * h;
    quint8* d = (quint8*) data.data();
    for (uint i = 0; i < dataSize; i++)
    {
        d[i] = d[i] >> 2;
    }
    for (uint i = 0; i < dataSize; i++)
    {
        d[i] = d[i] << 2;
    }
}

bool QIPGrayscaleImage::save(const QString& fileName, bool overwrite)
{
    quint16 hx = h;
    quint16 wx = w;
    QFile f(fileName);
    if ((f.exists()) && (!overwrite))
    {
        return false;
    }
    bool res = f.open(QIODevice::WriteOnly);
    if (!res)
    {
        return false;
    }
    f.write(fheader.toLatin1(), 4);
    f.write((char*) &hx, 2);
    f.write((char*) &wx, 2);
    f.write((char*) data.data(), wx * hx);
    f.flush();
    f.close();
    return true;
}


bool QIPGrayscaleImage::saveGrayscale(const QImage& image, const QString& fileName, bool overwrite)
{
    quint16 hx = image.height();
    quint16 wx = image.width();
    QFile f(fileName);
    if ((f.exists()) && (!overwrite))
    {
        return false;
    }
    if (!f.open(QIODevice::WriteOnly))
    {
        return false;
    }
    f.write(fheader.toLatin1(), 4);
    f.write((char*) &hx, 2);
    f.write((char*) &wx, 2);
    quint8* d = new quint8[wx];
    for (int y = 0; y < hx; y++)
    {
        quint8* line = (quint8*) image.scanLine(y);
        for (int x = 0; x < wx; x++)
            d[x] = line[x * 4];
        quint64 rl = f.write((char*) d, wx);
        while (rl != 0)
        {
            rl = f.write((char*) &d[rl], wx - rl);
        }
    }
    delete[] d;
    f.flush();
    f.close();
    return true;
}

int QIPGrayscaleImage::width() const
{
    return w;
}

int QIPGrayscaleImage::height() const
{
    return h;
}

QIPGrayscaleImage QIPGrayscaleImage::copy(int x1, int x2, int y1, int y2) const
{
    if (y2 > h)
    { y2 = h; }
    if (x2 > w)
    { x2 = w; }
    if (x1 > x2)
    { x1 = 0; }
    if (y1 > y2)
    { y1 = 0; }

    QIPGrayscaleImage result(x2 - x1, y2 - y1);
    quint8* s = (quint8*)data.data();
    quint8* d = (quint8*)result.data.data();

    IntRect r1;
    r1.x1 = x1;
    r1.x2 = x2;
    r1.y1 = y1;
    //r1.y2 = (y2 - y1) / 2;
    r1.y2 = y2;
    copyInternal2(r1, s, d);
    //copyInternal2(r2, s, d);

    return result;
}

void QIPGrayscaleImage::copyInternal2(IntRect& r, quint8* s, quint8* d) const
{
    for (int y = r.y1; y < r.y2; y++)
    {
        quint8* src = &scanLinePtr(s, y, w)[r.x1];
        quint8* dst = scanLinePtr(d, y - r.y1, r.x2 - r.x1);
        memcpy(dst, src, r.x2 - r.x1);
    }
}

QIPBlackAndWhiteImage QIPGrayscaleImage::binarize(QIPGrayscaleImage::BinarizationMethod method) const
{
    switch (method)
    {
        case OtsuBinarization :
            return otsuBinarize();
            break;
        case OtsuMABinarization:
            return otsuBinarizeMA();
            break;
        case NiblackBinarization :
            return niblackSauvolaBinarize(false);
            break;
        case SauvolaBinarization :
            return niblackSauvolaBinarize(true);
            break;
        case MaxEntropyBinarization :
            return maxEntropyBinarize();
            break;
        case BradleyBinarization :
            return bradleyBinarize();
            break;
        case IterativeBinarization:
            return iterativeBinarize();
            break;
        case BernsenBinarization:
            return bernsenBinarize();
            break;
        case GatosBinarization:
            return gatosBinarize();
            break;

        default:
            break;
    }
    return QIPBlackAndWhiteImage(0, 0);
}

QIPGrayscaleImage QIPGrayscaleImage::sharpen() const
{
    return applyFilter(SharpenFilter);
}

QIPGrayscaleImage QIPGrayscaleImage::blur() const
{
    return applyFilter(BlurFilter);
}

void QIPGrayscaleImage::isolateEdges()
{
    applyFilter(EdgesFilter);
}

void QIPGrayscaleImage::invert()
{
    for (int y = 0; y < h; y++)
    {
        quint8* line = scanLine(y);
        for (int x = 0; x < w; x++)
            line[x] = 255 - line[x];
    }
}

void QIPGrayscaleImage::wienerFilter()
{
    qr_wiener_filter((quint8*)data.data(), w, h);
}

void QIPGrayscaleImage::blendImage(const QIPBlackAndWhiteImage& image)
{
    IntRect r;
    r.x1 = 0;
    r.y1 = 0;
    r.x2 = width();
    r.y2 = height();
    quint8 * d1 = (quint8*)data.data();
    quint8 * d2 = (quint8*)image.data.data();
    blendImageInternal(r, d1, d2);
}

void QIPGrayscaleImage::blendImageInternal(const IntRect& r, quint8* p1, const quint8* p2)
{
    int w = r.x2 - r.x1;
    int h = r.y2 - r.y1;
    if (w * h < 32)
    { return; }
    if (h < 4)
    { return; }
    const quint8* bw = p2 + r.y1 * w;
    quint8* gs = p1 + r.y1 * w;
    for (int i = 0; i < w; i++)
        gs[i] = bw[i] == 1 ? qMin(gs[i] + 32, 255) : gs[i] * 3 / 4;
    for (int i = 0; i < w * h; i += w)
    {
        gs[i] = bw[i] == 1 ? qMin(gs[i] + 32, 255) : gs[i] * 3 / 4;
        gs[i + w - 1] = bw[i + w - 1] == 1 ? qMin(gs[i + w - 1] + 32, 255) : gs[i + w - 1] * 3 / 4;
    }
    for (int i = w * (h - 1); i < w * h; i++)
        gs[i] = bw[i] == 1 ? qMin(gs[i] + 32, 255) : gs[i] * 3 / 4;
    int up = Settings::instance()->getForegroundBrightenFactor();
    uint d1 = 3;
    uint d2 = 4;
    int ra = 0;
    int c = 0;

    for (int i = w + 1; i < w * (h - 1) - 1; i++)
    {
        if (bw[i] == 1)
        {
            ra += gs[i];
            c++;
            if ((bw[i - 1] != 0) || (bw[i + 1] != 0))
            {
                if ((bw[i - 2] != 0) || (bw[i + 1] != 0))
                {
                    if ((bw[i - w] != 0) || (bw[i + w] != 0))
                    {
                        if ((bw[i - w - 1] != 0) || (bw[i + w + 1] != 0))
                        {
                            if ((bw[i - w + 1] != 0) || (bw[i + w - 1] != 0))
                            {
                                gs[i] = qMin(gs[i] + up, 255);
                            }
                        }
                    }
                }
            }

        }
        else
        {
            gs[i] = gs[i] * d1 / d2;
        }
    }
    up = Settings::instance()->getGlobalBrightenFactor();
    if (ra / c < Settings::instance()->getDarkBackgroundThreshold())
    {
        for (int i = w + 1; i < w * (h - 1) - 1; i++)
            gs[i] = qMin(gs[i] + up, 255);
    }
    up = Settings::instance()->getGlobalDarkenFactor();
    int thr = Settings::instance()->getGlobalDarkenThreshold();
    for (int i = w + 1; i < w * (h - 1) - 1; i++)
        if ((gs[i] < thr) && (gs[i] > up))
        { gs[i] -= up; }
}

QIPGrayscaleImage::QIPGrayscaleImage(quint32 width, quint32 height) //: data(new quint8[width * height])
{
    //data = std::make_shared<quint8>(width * height);
    data.resize(width * height);
    w = width;
    h = height;
}

QPoint QIPGrayscaleImage::loadHeader(QFile* file)
{
    QPoint res(0, 0);
    char header[5] = {0};
    file->read(header, 4);
    if (QString::fromLatin1(header) != fheader)
    {
        return res;
    }
    quint16 wx, hx;
    file->read((char*) &hx, 2);
    file->read((char*) &wx, 2);
    res.setX(wx);
    res.setY(hx);
    return res;
}


void QIPGrayscaleImage::toGSRGDBBy3(const QImage& input, int top, int left, int bottom, int right)
{
    uchar lut[256 * 3] = {0};

    int lutcount = 0;
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            lut[lutcount] = i;
            lutcount++;
        }
    }
    for (int y = top; y < bottom; y++)
    {
        QRgb* lineIn = (QRgb*) input.scanLine(y);
        quint8* lineOut = scanLine(y);
        for (int x = left; x < right; x++)
        {
            QRgb cur = lineIn[x];
            uint grayLevel = (qRed(cur) + qGreen(cur) + qBlue(cur));
            lineOut[x] = lut[grayLevel];
        }
    }
}

void QIPGrayscaleImage::toImageInternal(uchar* image, const IntRect& rect, int imageWidth) const
{
    quint32 lut[256];
    for (uint i = 0; i < 256; i++)
    {
        lut[i] = 0xFF000000 + (i << 16) + (i << 8) + i;
    }
    int im4 = 4 * imageWidth;
    for (int y = rect.y1; y < rect.y2; y++)
    {

        quint8* line = scanLine(y);
        QRgb* lineOut = (QRgb*) &(image[im4 * y]);
        for (int x = rect.x1; x < rect.x2; x++)
        {
            lineOut[x] = lut[line[x]];
        }
    }
}

void QIPGrayscaleImage::toGrayScale(const QImage& input)
{
    toGSRGDBBy3(input,0,0,input.height(),input.width());
}

quint8 QIPGrayscaleImage::otsuThreshold(quint32 x1, quint32 x2, quint32 y1, quint32 y2) const
{
    qreal hist[256];
    histogram(hist, x1, x2, y1, y2);
    qreal ihist[256];
    qreal ut = 0;
    for (int i = 0; i < 256; i++)
        ut += (ihist[i] = i * hist[i]);

    int maxK = 0;
    int maxSigmaK = 0;

    qreal wk = 0;
    qreal uk = 0;
    for (int k = 0; k < 256; ++k)
    {
        if (hist[k] == 0)
        {
            continue;
        }
        wk += hist[k];
        uk += ihist[k];

        qreal sigmaK = 0;
        if ((wk != 0) && (wk != 1))
        {
            sigmaK = ((ut * wk - uk) * (ut * wk - uk)) / (wk * (1 - wk));
        }
        if (sigmaK > maxSigmaK)
        {
            maxK = k;
            maxSigmaK = sigmaK;
        }
    }
    return maxK;
}

quint8 QIPGrayscaleImage::simpleThreshold(quint32 x1, quint32 x2, quint32 y1, quint32 y2) const
{
    if (x2 == 0)
    { x2 = w; }
    if (y2 == 0)
    { y2 = h; }
    quint32 accum = 0;
    for (uint y = y1; y < y2; y++)
    {
        quint8* lineIn = scanLine(y);
        for (quint32 x = x1; x < x2; x++)
            accum += lineIn[x];
    }
    return accum / ((x2 - x1) * (y2 - y1));
}

quint8 QIPGrayscaleImage::maxEntropyThreshold(quint32 x1, quint32 x2, quint32 y1, quint32 y2) const
{
    if (x2 == 0)
    { x2 = w; }
    if (y2 == 0)
    { y2 = h; }
    qreal hist[256];
    histogram(hist, x1, x2, y1, y2);
    qreal cdv[256];
    cdv[0] = hist[0];
    for (uint i = 1; i < 256; i++)
        cdv[i] = cdv[i - 1] + hist[i];
    qreal epsilon = .1e-256;
    qreal hBlack[256] = {0.0};
    qreal hWhite[256] = {0.0};
    for (int t = 0; t < 256; t++)
    {
        if (cdv[t] > epsilon)
        {
            qreal hhB = 0;
            for (int i = 0; i <= t; i++)
            {
                if (hist[i] > epsilon)
                {
                    hhB -= hist[i] / cdv[t] * log(hist[i] / cdv[t]);
                }
            }
            hBlack[t] = hhB;
        }
        else
        {
            hBlack[t] = 0;
        }
        double cdi = 1 - cdv[t];
        if (cdi > epsilon)
        {
            qreal hhW = 0;
            for (int i = t + 1; i < 256; ++i)
            {
                if (hist[i] > epsilon)
                {
                    hhW -= hist[i] / cdi * log(hist[i] / cdi);
                }
            }
            hWhite[t] = hhW;
        }
        else
        {
            hWhite[t] = 0;
        }
    }

    qreal maxIndex = hBlack[0] + hWhite[0];
    quint8 maxT = 0;
    for (uint t = 1; t < 256; t++)
    {
        double j = hBlack[t] + hWhite[t];
        if (j > maxIndex)
        {
            maxIndex = j;
            maxT = t;
        }
    }
    return maxT;
}

qreal QIPGrayscaleImage::meanOfDistribution(quint32 x1, quint32 x2, quint32 y1, quint32 y2)
{
    qreal hist[256];
    histogram(hist, x1, x2, y1, y2);
    qreal result = 0;
    for (uint i = 0; i < 256; i++)
        result += hist[i] * i;
    return result;
}

qreal QIPGrayscaleImage::entropy(quint32 x1, quint32 x2, quint32 y1, quint32 y2)
{
    qreal hist[256];
    histogram(hist, x1, x2, y1, y2);
    qreal result = 0;
    for (uint i = 0; i < 256; i++)
        result += hist[i] * log(hist[i]);
    return -result;
}

void QIPGrayscaleImage::equalize()
{
    qreal hist[256];
    histogram(hist);
    quint8 tt[256];
    //tt[0] = 0;
    for (uint i = 0; i < 256; i++)
    {
        tt[i] = cdf(hist, i) * 255;
    }

    for (int y = 0; y < h; y++)
    {
        quint8* lineOut = scanLine(y);
        for (int x = 0; x < w; x++)
            lineOut[x] = tt[lineOut[x]];
    }
}

void QIPGrayscaleImage::equalize2()
{
    qreal hist[256];
    histogram(hist);
    quint8 tt[256];
    //tt[0] = 0;
    for (uint i = 0; i < 256; i++)
    {
        tt[i] = cdf(hist, i) * 255;
    }

    for (int y = 0; y < h; y++)
    {
        quint8* lineOut = scanLine(y);
        for (int x = 0; x < w; x++)
            lineOut[x] = tt[lineOut[x]] >> 1;
        for (int x = 0; x < w; x++)
            lineOut[x] = tt[lineOut[x]] << 1;

    }
}

qreal QIPGrayscaleImage::lpcEntropy(int x1, int x2, int y1, int y2)
{
    uint ihist[256] = {0};
    if (x2 == 0)
    { x2 = w; }
    if (y2 == 0)
    { y2 = h; }
    for (int y = y1; y < y2; y++)
    {
        quint8* lineOut = scanLine(y);
        ihist[lineOut[x1]]++;
        ihist[lineOut[x1 + 1]]++;
        ihist[lineOut[x1 + 2]]++;
        quint8 cl = lineOut[x1 + 2];
        for (int x = x1 + 3; x < x2; x++)
        {
            quint8 cp = predictor(&lineOut[x]);
            if ((cp - lineOut[x]) * (cp - lineOut[x]) < 2)
            {
                ihist[cl]++;
            }
            else
            {
                cl = lineOut[x];
                ihist[cl]++;
            }
        }
    }
    qreal hist[256] = {0.0};
    qreal k = 1 / ((x2 - x1) * (y2 - y1));
    qreal sum = 0;
    for (uint i = 0; i < 256; i++)
    {
        hist[i] = ihist[i] * k * log(ihist[i] * k);
        sum += hist[i];
    }
    return -sum;
}

qreal QIPGrayscaleImage::variance(quint32 x1, quint32 x2, quint32 y1, quint32 y2)
{
    qreal hist[256];
    histogram(hist, x1, x2, y1, y2);
    qreal max = 0;
    quint8 imax;
    for (int i = 0; i < 256; i++)
        if (hist[i] > max)
        {
            max = hist[i];
            imax = i;
        }
    qreal sum = 0;
    for (int i = 0; i < 256; i++)
        sum += abs(i - imax) * hist[i];
    return sum;
}

quint8* QIPGrayscaleImage::scanLine(quint32 y) const
{
    return &(reinterpret_cast<quint8*>(const_cast<char*>(data.data()))[y * w]);
}


inline quint8 AVERAGE(quint8 a, quint8 b)
{ return ((a + b) >> 1); }

QIPGrayscaleImage QIPGrayscaleImage::scaleX2()
{
    QIPGrayscaleImage res(width() * 2, height() * 2);
    quint8* source = (quint8*)data.data();
    quint8* target = (quint8*)res.data.data();
    int srcWidth = width() * height();
    int targetWidth = srcWidth * 4;
    int numPixels = srcWidth;
    int mid = srcWidth;
    int e = 0;
    quint8 p;

    numPixels--;
    while (numPixels-- > 0)
    {
        p = *source;
        if (e >= mid)
        {
            p = AVERAGE(p, *(source + 1));
        }
        *target++ = p;
        e += srcWidth;
        if (e >= targetWidth)
        {
            e -= targetWidth;
            source++;
        }
    }
    *target = *source;
    return res;
}

quint8 QIPGrayscaleImage::pixel(quint32 x, quint32 y) const
{
    return data.data()[x + y * w];
}

void QIPGrayscaleImage::setPixel(quint32 x, quint32 y, quint8 value)
{
    data.data()[x + y * w] = value;
}

quint8 QIPGrayscaleImage::nextInColumn(quint32 x, quint32& y)
{
    y++;
    return data.data()[x + y * w];
}

quint8 QIPGrayscaleImage::prevInColumn(quint32 x, quint32& y)
{
    y--;
    return data.data()[x + y * w];
}

qreal QIPGrayscaleImage::cdf(QIPHistogram hist, quint8 x)
{
    qreal result = 0;
    for (int i = 0; i < x + 1; i++)
        result += hist[i];
    return result;
}

quint8 QIPGrayscaleImage::predictor(quint8* x)
{
    quint8 xp[3] = {*(x - 3), *(x - 2), *(x - 1)};
    int d1 = xp[1] - xp[0];
    int d2 = xp[2] - xp[1];
    int dp = 2 * d2 - d1;
    int xd = *x + dp;
    xd = qBound(0, xd, 255);
    return (quint8) xd;
}

QIPGrayscaleImage QIPGrayscaleImage::applyFilter(int type) const
{
    QIPGrayscaleImage result(w, h);
    int kernel[3][3];
    int wk;
    int a;
    if (type == SharpenFilter)
    {
        memcpy(kernel, kSharpen, 9 * sizeof(int));
        wk = wkSharpen;
        a = 0;
    }
    if (type == BlurFilter)
    {
        memcpy(kernel, kBlur, 9 * sizeof(int));
        wk = wkBlur;
        a = 0;
    }
    if (type == EdgesFilter)
    {
        memcpy(kernel, kEdges, 9 * sizeof(int));
        wk = wkEdges;
        a = 128;
    }
    for (int x = 1; x < w - 1; x++)
    {
        for (int y = 1; y < h - 1; y++)
        {
            int c = 0;
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                    c += pixel(x + i, y + j) * kernel[i + 1][j + 1];
                c = qBound(0, c / wk + a, 255);
                result.setPixel(x, y, c);
            }
        }
    }
    if (type == SharpenFilter)
    {
        result.invert();
    }
    return result;
}

void QIPGrayscaleImage::dilateImage(quint8 threshold)
{

}


QIPBlackAndWhiteImage QIPGrayscaleImage::niblackSauvolaBinarize(bool sauvola) const
{
    QIPBlackAndWhiteImage result(w, h);
    const uint WindowSize = 15;
    const uint halfWindowSize = WindowSize / 2;
    const qreal weight = 0.2;
    int xMin = halfWindowSize;
    int yMin = halfWindowSize;
    int xMax = w - 1 - halfWindowSize;
    int yMax = h - 1 - halfWindowSize;
    quint8* output = (quint8*)result.data.data();
    qreal sumPixelsWindow = 0;
    qreal sum2PixelsWindow = 0;
    qreal localMean = 0;
    qreal localVar = 0;
    qreal localStd = 0;
    qreal localValue = 0;
    qreal mainSumPixelWindow = 0;
    qreal mainSum2PixelWindow = 0;
    qreal* sumCols = new qreal[w]; //= {0.0};
    qreal* sumSqCols = new qreal[w];// = {0.0};
    memset(sumCols, 0, w * sizeof(qreal));
    memset(sumSqCols, 0, w * sizeof(qreal));
    for (uint i = 0; i < WindowSize; i++)
    {
        for (int x = 0; x < w; x++)
        {
            sumCols[x] += pixel(x, i);
            sumSqCols[x] += pixel(x, i) * pixel(x, i);
        }
    }
    for (uint j = 0; j < WindowSize; j++)
    {
        mainSumPixelWindow += sumCols[j];
        mainSum2PixelWindow += sumSqCols[j];
    }
    for (int y = yMin; y <= yMax; y++)
    {
        for (int j = xMin; j <= xMax; j++)
        {
            if (y == xMin)
            {
                if (j == yMin)
                {
                    sumPixelsWindow = mainSumPixelWindow;
                    sum2PixelsWindow = mainSum2PixelWindow;
                }
                else
                {
                    sumPixelsWindow += (sumCols[j + halfWindowSize] - sumCols[j - halfWindowSize - 1]);
                    sum2PixelsWindow += (sumSqCols[j + halfWindowSize] - sumSqCols[j - halfWindowSize - 1]);
                }
            }
            else
            {
                if (j == yMin)
                {
                    for (int x = 0; x < w; x++)
                    {
                        sumCols[x] += (pixel(x, y + halfWindowSize) - pixel(x, y - halfWindowSize - 1));
                        sumSqCols[x] += (pixel(x, y + halfWindowSize) * pixel(x, y + halfWindowSize) -
                                         pixel(x, y - halfWindowSize - 1) * pixel(x, y - halfWindowSize - 1));
                    }
                    mainSumPixelWindow = 0;
                    mainSum2PixelWindow = 0;
                    for (uint k = 0; k < WindowSize; k++)
                    {
                        mainSumPixelWindow += sumCols[k];
                        mainSum2PixelWindow += sumSqCols[k];
                    }

                    sumPixelsWindow = mainSumPixelWindow;
                    sum2PixelsWindow = mainSum2PixelWindow;
                }
                else
                {
                    sumPixelsWindow += (sumCols[j + halfWindowSize] - sumCols[j - halfWindowSize - 1]);
                    sum2PixelsWindow += (sumSqCols[j + halfWindowSize] - sumSqCols[j - halfWindowSize - 1]);
                }
            }

            localMean = sumPixelsWindow / (WindowSize * WindowSize);
            localVar = sum2PixelsWindow / (WindowSize * WindowSize) - localMean * localMean;
            localStd = sqrt(fabs(localVar));

            if (!sauvola)
            {
                localValue = localMean + weight * localStd;
            }
            else
            {
                localValue = localMean * (1 + (localStd / 128 - 1) / 6);
            }

            if (pixel(j, y) < localValue)
            {
                output[y * w + j] = 0;
            }
            else
            {
                output[y * w + j] = 1;
            }

        }
    }

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (y < yMin)
            { output[y * w + x] = 1; }
            if (y > yMax)
            { output[y * w + x] = 1; }
            if (x < xMin)
            { output[y * w + x] = 1; }
            if (x > xMax)
            { output[y * w + x] = 1; }
        }
    }
    delete[] sumCols;
    delete[] sumSqCols;
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::otsuBinarize() const
{
    QIPBlackAndWhiteImage result(w, h);
    quint8 threshold = otsuThreshold();
    for (int y = 0; y < h; y++)
    {
        quint8* line = scanLine(y);
        quint8* lineOut = result.scanLine(y);
        for (int x = 0; x < w; x++)
        {
            if (line[x] > threshold)
            { lineOut[x] = 1; }
            else
            { lineOut[x] = 0; }
        }
    }
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::otsuBinarizeMA() const
{
    quint32 sum;
    QIPBlackAndWhiteImage result(w, h);
    if (w * h < 4)
    {
        return result;
    }
    quint8 threshold = otsuThreshold();
    result.data = data;
    quint8* d = (quint8*)result.data.data();
    sum = d[0] + d[1] + d[2] + d[3] + d[4] + d[5] + d[6];
    for (int i = 3; i < w * h - 4; i++)
    {
        d[i] = sum / 8;
        sum = sum - d[i - 3] + d[i + 4];
    }
    for (int y = 0; y < h; y++)
    {
        quint8* lineOut = result.scanLine(y);
        for (int x = 0; x < w; x++)
        {
            if (lineOut[x] > threshold)
            {
                lineOut[x] = 1;
            }
            else
            {
                lineOut[x] = 0;
            }
        }
    }
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::gatosBinarize() const
{
    QIPBlackAndWhiteImage result(w, h);
    result.data = data;
    quint8* d = (quint8*)result.data.data();
    qr_binarize(d, w, h);
    for (int y = 0; y < h; y++)
    {
        quint8* line = result.scanLine(y);
        for (int x = 0; x < w; x++)
            if (line[x] == 0)
            {
                line[x] = 1;
            }
            else
            {
                line[x] = 0;
            }
    }
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::maxEntropyBinarize() const
{
    QIPBlackAndWhiteImage result(w, h);
    quint8 threshold = maxEntropyThreshold();
    for (int y = 0; y < h; y++)
    {
        quint8* line = scanLine(y);
        quint8* lineOut = result.scanLine(y);
        for (int x = 0; x < w; x++)
        {
            if (line[x] >= threshold)
            {
                lineOut[x] = 1;
            }
            else
            {
                lineOut[x] = 0;
            }
        }
    }
    return result;
}


QIPBlackAndWhiteImage QIPGrayscaleImage::bradleyBinarize() const
{
    const uint windowSize = 41;
    const qreal pixelBrightnessDifferenceLimit = 0.15;
    QIPBlackAndWhiteImage result(w, h);
    //quint8* resultData = (quint8*)result.data.data();
    //memcpy(resultData, data.data(), w * h);
    result.data = data;
    quint8* resultData = (quint8*)result.data.data();
    uint* intImage = new uint[w * h];
    integralImage(w, h, intImage);
    int halfWindowSize = windowSize / 2;
    qreal avgBrightnessPart = 1.0f - pixelBrightnessDifferenceLimit;
    for (int y = 0; y < h; y++)
    {
        int y1 = y - halfWindowSize > 0 ? y - halfWindowSize : 0;
        int y2 = y + halfWindowSize > h - 1 ? h - 1 : y + halfWindowSize;
        for (int x = 0; x < w; x++)
        {
            int x1 = x - halfWindowSize < 0 ? 0 : x - halfWindowSize;
            int x2 = x + halfWindowSize < w - 1 ? x + halfWindowSize : w - 1;
            resultData[x + y * w] = resultData[x + y * w] <
                                    (intImage[y2 * w + x2] + intImage[y1 * w + x1] - intImage[y1 * w + x2] -
                                     intImage[y2 * w + x1]) / ((x2 - x1) * (y2 - y1)) * avgBrightnessPart ? 0 : 1;
        }
    }
    delete[] intImage;
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::iterativeBinarize() const
{
    QIPBlackAndWhiteImage result(w, h);
    quint8 threshold = CalculateIterativeThreshold();
    for (int y = 0; y < h; y++)
    {
        quint8* line = scanLine(y);
        quint8* lineOut = result.scanLine(y);
        for (int x = 0; x < w; x++)
        {
            lineOut[x] = line[x] < threshold ? 0 : 1;
        }
    }
    return result;
}

QIPBlackAndWhiteImage QIPGrayscaleImage::bernsenBinarize() const
{
    const int regSize = 7;
    const quint8 contrastLimit = 120;
    const quint8 confused = 1;
    QIPBlackAndWhiteImage result(w, h);
    for (int y = 0; y < h; y++)
    {
        const quint8* line = scanLine(y);
        quint8* lineOut = result.scanLine(y);
        int istart = y == 0 ? 0 : y - 1;
        int istop = y + regSize >= h ? h : y + regSize - 1;
        quint8 minimum = 255;
        quint8 maximum = 0;
        int maxX = -1;
        int minX = -1;
        for (int x = 0; x < w; x++)
        {
            int jstart = x == 0 ? 0 : x - 1;
            int jstop = x + regSize >= w ? w : x + regSize - 1;
            if ((maxX < jstart) || (minX < jstart))
            {
                minimum = 255;
                maximum = 0;
                for (int i = istart; i < istop; i++)
                {
                    const quint8* lineK = scanLine(i);
                    for (int j = jstart; j < jstop; j++)
                    {
                        if (minimum > lineK[j])
                        {
                            minimum = lineK[j];
                            minX = j;
                        }
                        if (maximum < lineK[j])
                        {
                            maximum = lineK[j];
                            maxX = j;
                        }

                    }
                }
            }
            else
            {
                for (int i = istart; i < istop; i++)
                {
                    if (minimum > scanLine(i)[jstop - 1])
                    {
                        minimum = scanLine(i)[jstop - 1];
                        minX = jstop - 1;
                    }
                    if (maximum < scanLine(i)[jstop - 1])
                    {
                        maximum = scanLine(i)[jstop - 1];
                        maxX = jstop - 1;
                    }
                }
            }
            quint8 c = maximum - minimum;
            quint8 value;
            if (c < contrastLimit)
            {
                value = confused;
            }
            else
            {
                uint p = (maximum + minimum) / 2;
                value = line[x] >= p ? 1 : 0;
            }
            lineOut[x] = value;
        }
    }
    return result;
}


quint8 QIPGrayscaleImage::CalculateIterativeThreshold() const
{
    quint32 distribution[256] = {0};
    for (int y = 0; y < h; y++)
    {
        quint8* line = scanLine(y);
        for (int x = 0; x < w; x++)
            distribution[line[x]]++;
    }
    quint32 integralHist[256];
    quint32 integralDistribution[256];
    integralHist[0] = 0;
    integralDistribution[0] = distribution[0];
    for (int i = 1; i < 256; i++)
    {
        integralHist[i] = integralHist[i - 1] + distribution[i] * i;
        integralDistribution[i] = integralDistribution[i - 1] + distribution[i];
    }

    quint8 oldThreshold = 0;
    quint8 newThreshold = 128;
    while (abs(newThreshold - oldThreshold) > 0)
    {
        uint meanWhite;
        uint meanBlack;
        uint sumBlack = integralHist[newThreshold];
        uint numBlack = integralDistribution[newThreshold];
        uint sumWhite = integralHist[255] - integralHist[newThreshold];
        uint numWhite = integralDistribution[255] - integralDistribution[newThreshold];

        /*for (int i = 0; i < 256; i++)
            if (i < newThreshold) {
                sumBlack += valueHist[i];
                numBlack += distribution[i];
            } else {
                sumWhite += valueHist[i];
                numWhite += distribution[i];
            }*/
        meanBlack = sumBlack != 0 ? sumBlack / numBlack : 0;
        meanWhite = sumWhite != 0 ? sumWhite / numWhite : 0;
        oldThreshold = newThreshold;
        newThreshold = (meanBlack + meanWhite) / 2;
    }
    return newThreshold;
}

void QIPGrayscaleImage::integralImage(uint w, uint h, uint* image) const
{
    IntRect r1 = {0,0,(int)w, (int)h};
    copyInternal(r1, image);
    for (uint x = 1; x < w; x++)
        image[x] += image[x - 1];
    for (uint y = 1; y < h; y++)
        image[y * w] += image[(y - 1) * w];
    for (uint y = 1; y < h; y++)
    {
        for (uint x = 1; x < w; x++)
            image[y * w + x] =
                    image[y * w + x] + image[y * w + x - 1] + image[(y - 1) * w + x] - image[(y - 1) * w + x - 1];
    }
}

void QIPGrayscaleImage::toGrayscaleFast(const QImage& input)
{
    for (int y = 0; y < h; y++)
    {
        quint8* lineIn = (quint8*) input.scanLine(y);
        quint8* lineOut = scanLine(y);
        for (int x = 0; x < w; x++)
        {
            lineOut[x] = lineIn[x << 2];
        }
    }
}

inline quint8 maxOfThree(quint8 v1, quint8 v2, quint8 v3)
{
    quint8 max = v1;
    if (v2 > max)
    {
        max = v2;
    }
    if (v3 > max)
    {
        max = v3;
    }
    return max;
}

inline quint8 minOfThree(quint8 v1, quint8 v2, quint8 v3)
{
    quint8 min = v1;
    if (v2 < min)
    {
        min = v2;
    }
    if (v3 < min)
    {
        min = v3;
    }
    return min;
}


void QIPGrayscaleImage::toGrayscaleMinMax(const QImage& input)
{
    IntRect r = {0,0,input.width(),input.height()};
    toGrayscaleMinMaxInternal(input,r);
}

void QIPGrayscaleImage::toGrayscaleMinOrMax(const QImage& input, bool min)
{
    IntRect r = {0,0,input.width(),input.height()};
    toGrayscaleMinOrMaxInternal(input,r, min);
}

void QIPGrayscaleImage::toGrayscaleMinOrMaxInternal(const QImage& input, const IntRect& rect, bool min)
{
    for (int y = rect.y1; y < rect.y2; y++)
    {
        QRgb* lineIn = (QRgb*) input.scanLine(y);
        quint8* lineOut = scanLine(y);
        for (int x = rect.x1; x < rect.x2; x++)
        {
            QRgb cur = lineIn[x];
            lineOut[x] = min ? minOfThree(qRed(cur), qGreen(cur), qBlue(cur)) : maxOfThree(qRed(cur), qGreen(cur),
                                                                                           qBlue(cur));
        }
    }
}

void QIPGrayscaleImage::toGrayscaleMinMaxInternal(const QImage& input, const IntRect& rect)
{
    for (int y = rect.y1; y < rect.y2; y++)
    {
        QRgb* lineIn = (QRgb*) input.scanLine(y);
        quint8* lineOut = scanLine(y);
        for (int x = rect.x1; x < rect.x2; x++)
        {
            QRgb cur = lineIn[x];
            lineOut[x] =
                    (minOfThree(qRed(cur), qGreen(cur), qBlue(cur)) + maxOfThree(qRed(cur), qGreen(cur), qBlue(cur)))
                            >> 1;
        }
    }

}

double safeLog(double x)
{
    if (x > 0)
    {
        return log(x);
    }
    return 0;
}

void QIPGrayscaleImage::toGrayScaleByEntropyChannel(const QImage& input, bool maxEntropy)
{
    qreal rHist[256];
    qreal gHist[256];
    qreal bHist[256];
    quint32 rChannel[256] = {0};
    quint32 gChannel[256] = {0};
    quint32 bChannel[256] = {0};
    int width = input.width();
    int height = input.height();
    if (width * height == 0)
    { return; }
    for (int y = 0; y < height; y++)
    {
        QRgb* lineIn = (QRgb*) input.scanLine(y);
        for (int x = 0; x < width; x++)
        {
            rChannel[qRed(lineIn[x])]++;
            gChannel[qGreen(lineIn[x])]++;
            bChannel[qBlue(lineIn[x])]++;
        }
    }
    qreal sizeI = 1.0 / (width * height);
    for (int i = 0; i < 256; i++)
    {
        rHist[i] = rChannel[i] * sizeI;
        rHist[i] *= (-safeLog(rHist[i]));
        gHist[i] = gChannel[i] * sizeI;
        gHist[i] *= (-safeLog(gHist[i]));
        bHist[i] = bChannel[i] * sizeI;
        bHist[i] *= (-safeLog(bHist[i]));
    }
    qreal rEntropy = 0.0;
    qreal gEntropy = 0.0;
    qreal bEntropy = 0.0;
    for (int i = 0; i < 256; i++)
    {
        rEntropy += rHist[i];
        gEntropy += gHist[i];
        bEntropy += bHist[i];
    }
    int (* chan)(QRgb);
    if (maxEntropy)
    {
        qreal max = rEntropy;
        chan = qRed;
        if (gEntropy > max)
        {
            max = gEntropy;
            chan = qGreen;
        }
        if (bEntropy > max)
        {
            chan = qBlue;
        }

    }
    else
    {
        qreal min = gEntropy;
        chan = qGreen;
        if (rEntropy < min)
        {
            min = rEntropy;
            chan = qRed;
        }
        if (bEntropy < min)
        {
            chan = qBlue;
        }
    }
    for (int y = 0; y < height; y++)
    {
        QRgb* lineIn = (QRgb*) input.scanLine(y);
        quint8* lineOut = scanLine(y);
        for (int x = 0; x < width; x++)
            lineOut[x] = chan(lineIn[x]);

    }

}

quint8 maxTh(qreal* hist, qreal* ihist, int start, int stop)
{
    qreal ut = 0;
    for (int i = start; i < stop; i++)
        ut += ihist[i];
    int maxK = 0;
    int maxSigmaK = 0;

    qreal wk = 0;
    qreal uk = 0;
    for (int k = start; k < stop; ++k)
    {
        if (hist[k] == 0)
        {
            continue;
        }
        wk += hist[k];
        uk += ihist[k];

        qreal sigmaK = 0;
        if ((wk != 0) && (wk != 1))
        {
            sigmaK = ((ut * wk - uk) * (ut * wk - uk)) / (wk * (1 - wk));
        }
        if (sigmaK > maxSigmaK)
        {
            maxK = k;
            maxSigmaK = sigmaK;
        }
    }
    return maxK;
}

QIPGrayscaleImage::QIPGrayscaleImage() : data(0)
{
    w = 0;
    h = 0;
}

quint8* QIPGrayscaleImage::scanLinePtr(quint8* ptr, int y, int wth)
{
    return ptr + y * wth;
}
