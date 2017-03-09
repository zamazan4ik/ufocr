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


#ifndef QIPGRAYSCALEIMAGE_H
#define QIPGRAYSCALEIMAGE_H

#include "qipblackandwhiteimage.h"
#include "common.h"
#include <QImage>
#include <QSharedPointer>
#include <QPoint>

/*!
  \typedef qreal QIPHistogram[256];
  Array of 256 double values for storing image intensity histograms.
*/

typedef qreal QIPHistogram[256];

class QFile;

/*!
  \class QIPGrayscaleImage
  This class holds internal representations of grayscale images and allows you to perform different grayscale operations and binarize images.
  You pass the data to the instance of this class when you create the instance. The colored QImage is grayscaled using one of the GrayscaleConversion
  types. If you want obtain the grayscaled QImage use the toImage() method. The binarize() method produces black and white (binary) images.
  \sa QIPBlackAndWhiteImage
 */

class QIPGrayscaleImage
{
public:
    QIPGrayscaleImage();

    /*!
      These codes specify which type of conversion from RGB to 256-level grayscale is to be used.
      MaxEntropyChannel and MinValue suite best if you canvert to grayscale for the further text extarction.
    */

    enum class GrayscaleConversion
    {
        RGBDevideByThree = 0, /*!< gray(i) = (r(i)+g(i)+b(i))/3 */
        MinMaxValue, /*!< gray(i) = (min(r(i), g(i), b(i)) + max(r(i), g(i), b(i)))/2 */
        MinValue, /*!< gray(i) = min(r(i), g(i), b(i)) */
        MaxValue, /*!< gray(i) = max(r(i), g(i), b(i)) */
        MaxEntropyChannel, /*!< gray(i) = ch(i)  where ch is either r, g or b depending on which has the maximum entropy. */
        MinEntropyChannel, /*!< gray(i) = ch(i)  where ch is either r, g or b depending on which has the minimum entropy. */
        FastConversion,
        NoConnversion
    };
    Q_DECLARE_FLAGS(GrayscaleTransformations, GrayscaleConversion)

    /*!
      This is the constructor for an QIPGrayscaleImage object that you should use to start your image processing.
      \param image the QImage object holding an image to be processed.
      \param conversionMethod the type of conversion from RGB to grayscale.
      \sa GrayscaleConversion
     */


    explicit QIPGrayscaleImage(const QImage& image, GrayscaleConversion conversionMethod = GrayscaleConversion::RGBDevideByThree);

    QIPGrayscaleImage(QIPBlackAndWhiteImage& image1, QIPBlackAndWhiteImage& image2);

    /*!
      Makes QIPGrayscaleImage from QImage.
      \sa toImage
    */
    static QIPGrayscaleImage fromImage(const QImage& image, GrayscaleConversion conversionMethod = GrayscaleConversion::RGBDevideByThree);

    /*!
      Returns true if the instance contains no data.
    */

    bool isNull() const;

    QIPGrayscaleImage(const QIPGrayscaleImage& I);

    QIPGrayscaleImage& operator=(const QIPGrayscaleImage& I);

    QIPGrayscaleImage(const QString& ygfFileName);

    ~QIPGrayscaleImage();

    /*!
      Converts grayscaled image to QImage object.
    */
    QImage toImage() const;

    /*!
      These flags specify which type of binariztion to use when calling binarize() method.
      \sa binarize()
    */
    enum class BinarizationMethod
    {
        OtsuBinarization, /*!< Otsu global binarization */
        OtsuMABinarization,
        NiblackBinarization, /*!< Niblack adaptive binarization */
        SauvolaBinarization, /*!< Sauvola adaptive binarization */
        MaxEntropyBinarization, /*!< Maximum entropy global binarization */
        BradleyBinarization, /*!< Bradley adaptive binarization */
        IterativeBinarization, /*!< Global iterative binarization */
        BernsenBinarization, /*!< Bernsen adaptive binarization */
        GatosBinarization /*!< Ga'tos adaptive binarization */
    };
    Q_DECLARE_FLAGS(BinarizationMethods, BinarizationMethod)

    /*! Calculates image intensity histogram for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the histogram for the who;e image is calculated.
        \sa equalize
        \sa entropy
    */
    void histogram(QIPHistogram& result, quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0) const;

    /*!
     *  Finds foreground/background threshold using Otsu method for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the result for the whole image is calculated.
     *  Note that the values of x1, x2, y1, y2 should not exceed those of the original image (this is not checked).
        \sa simpleThreshold
        \sa maxEntropyThreshold
    */

    quint8 otsuThreshold(quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0) const;

    /*!
     *  Finds foreground/background threshold as a mean intencity value for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the result for the whole image is calculated.
     *  Note that the values of x1, x2, y1, y2 should not exceed those of the original image (this is not checked).
        \sa otsuThreshold
        \sa maxEntropyThreshold
    */
    quint8 simpleThreshold(quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0) const;

    /*!
     *  Finds foreground/background threshold as a maximum entropy threshold for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the result for the whole image is calculated.
     *  Note that the values of x1, x2, y1, y2 should not exceed those of the original image (this is not checked).
        \sa otsuThreshold
        \sa simpleThreshold
    */
    quint8 maxEntropyThreshold(quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0) const;

    /*!
     *  Finds mean of distribution for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the result for the whole image is calculated.
     *  Note that the values of x1, x2, y1, y2 should not exceed those of the original image (this is not checked).
        \sa otsuThreshold
        \sa simpleThreshold
    */
    qreal meanOfDistribution(quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0);

    /*!
     *  Finds the entropy (-Sum(p(i)*Log(p(i)) for the rectangle specified by (x1,y1) (x2,y2). the reault is returned in result variable. If x1, y1, x2, y2 are set to zero the result for the whole image is calculated.
     *  Note that the values of x1, x2, y1, y2 should not exceed those of the original image (this is not checked).
    */
    qreal entropy(quint32 x1 = 0, quint32 x2 = 0, quint32 y1 = 0, quint32 y2 = 0);

    /*!
     *  Equalizes the image histogram (spreads colors).
     * \sa histogram
    */
    void equalize();

    void equalize2();

    qreal lpcEntropy(int x1 = 0, int x2 = 0, int y1 = 0, int y2 = 0);

    qreal variance(quint32 x1, quint32 x2, quint32 y1, quint32 y2);

    /*!
     *  Returns the image's width.
     * \sa height
    */
    int width() const;

    /*!
     *  Returns the image's height.
     * \sa width
    */
    int height() const;

    /*!
     *  Returns the part of the original grayscale image bounded by the rectangle (x1,y1) (x2,y2) as a new instance of QIPGrayscaleImage.
     *  Note that the boundaries should not exceed those of the original image (this is not checked).
     *  \sa toImage
    */
    QIPGrayscaleImage copy(int x1, int x2, int y1, int y2) const;

    void copyInternal2(IntRect& r, quint8* s, quint8* d) const;

    static quint8* scanLinePtr(quint8* ptr, int y, int wth);

    /*!
      Converts the grayscaled image to binary format using the specified method.
    */

    QIPBlackAndWhiteImage binarize(BinarizationMethod method) const;

    /*!
      Sharpens the image.
    */
    QIPGrayscaleImage sharpen() const;

    /*!
      Blurs the image.
    */
    QIPGrayscaleImage blur() const;

    /*!
      Transforms the image so then edges appear as white lines on black.
    */
    void isolateEdges();

    /*!
      Inverts the intensity levels of the grayscale image.
    */
    void invert();

    void wienerFilter();

    void blendImage(const QIPBlackAndWhiteImage& image);

    void darken(quint8 factor);

    void smoother();

    void quantisize();

    bool save(const QString& fileName, bool overwrite = false);

    static bool saveGrayscale(const QImage& image, const QString& fileName, bool overwrite = false);

    quint8* scanLine(quint32 y) const;

    QIPGrayscaleImage scaleX2();

signals:

public slots:
protected:
    QIPGrayscaleImage(quint32 width, quint32 height);

private:
    int w, h;
    //QSharedPointer<quint8> data;
    QByteArray data;
    //std::shared_ptr<quint8> data;
    enum
    {
        SharpenFilter,
        BlurFilter,
        EdgesFilter
    } FilterType;
private:
    QPoint loadHeader(QFile* file);

    void toGSRGDBBy3(const QImage& input, int top, int left, int bottom, int right);

    void toImageInternal(uchar* image, const IntRect& rect, int imageWidth) const;

    void toGrayScale(const QImage& input);

    inline quint8 pixel(quint32 x, quint32 y) const;

    inline void setPixel(quint32 x, quint32 y, quint8 value);

    inline quint8 nextInColumn(quint32 x, quint32& y);

    inline quint8 prevInColumn(quint32 x, quint32& y);

    qreal cdf(QIPHistogram hist, quint8 x);

    quint8 predictor(quint8* x);

    QIPGrayscaleImage applyFilter(int type) const;

    void dilateImage(quint8 threshold = 0);

    QIPBlackAndWhiteImage niblackSauvolaBinarize(bool sauvola) const;

    QIPBlackAndWhiteImage otsuBinarize() const;

    QIPBlackAndWhiteImage otsuBinarizeMA() const;

    QIPBlackAndWhiteImage gatosBinarize() const;

    QIPBlackAndWhiteImage maxEntropyBinarize() const;

    QIPBlackAndWhiteImage bradleyBinarize() const;

    QIPBlackAndWhiteImage iterativeBinarize() const;

    QIPBlackAndWhiteImage bernsenBinarize() const;

    quint8 CalculateIterativeThreshold() const;

    void integralImage(uint w, uint h, uint* image) const;

    void toGrayscaleFast(const QImage& input);

    void toGrayscaleMinMax(const QImage& input);

    void toGrayscaleMinOrMax(const QImage& input, bool min);

    void toGrayscaleMinOrMaxInternal(const QImage& input, const IntRect& rect, bool min);

    void toGrayscaleMinMaxInternal(const QImage& input, const IntRect& rect);

    void toGrayScaleByEntropyChannel(const QImage& input, bool maxEntropy);

    void histogramInternal(qreal* result, const IntRect& r) const;

    void copyInternal(const IntRect& r, uint* image) const;

    void blendImageInternal(const IntRect& r, quint8* p1, const quint8* p2);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QIPGrayscaleImage::BinarizationMethods)

Q_DECLARE_OPERATORS_FOR_FLAGS(QIPGrayscaleImage::GrayscaleTransformations)

#endif // QIPGRAYSCALEIMAGE_H
