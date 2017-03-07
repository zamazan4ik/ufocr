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



#ifndef QIPBLACKANDWHITEIMAGE_H
#define QIPBLACKANDWHITEIMAGE_H

#include "common.h"
//#include "qipconnectedcomponents.h"
#include <QImage>
#include <QPoint>
#include <QSharedPointer>
#include <memory>


/*!
  \class QIPBlackAndWhiteImage
  This class holds internal representations of black and white (binary) images and allows you to perform different operations on them.
  The only way to create an instance of the class is to call QIPGrayscaleImage::binarize() method. When you are done with transformations you can convert the data to QImage image (which will contain only black and white pixels) using the toImage() method.
  \sa QIPGrayscaleImage
 */


class QIPGrayscaleImage;

class QIPBlackAndWhiteImage
{
    QIPBlackAndWhiteImage(quint32 width, quint32 height);

public:
    QIPBlackAndWhiteImage();

    QIPBlackAndWhiteImage(const QIPBlackAndWhiteImage& I);

    ~QIPBlackAndWhiteImage();

    /*!
      Converts binary image to QImage object.
    */
    QImage toImage() const;

    /*!
     *  Returns the part of the original black and white image bounded by the rectangle (x1,y1) (x2,y2) as a new instance of QIPBlackAndWhiteImage.
     *  Note that the boundaries should not exceed those of the original image (this is not checked).
     *  \sa toImage
    */
    QIPBlackAndWhiteImage copy(quint32 x1, quint32 x2, quint32 y1, quint32 y2) const;

    /*!
     *  Returns the image's width.
     * \sa height
    */
    quint32 width();

    /*!
     *  Returns the image's height.
     * \sa width
    */
    quint32 height();


    /*!
     *  Returns an image obtained by morphologically dilating the original image with the structuring element defined by the structuringElement variable.
     *  structuringElement is an array of quint8 values where 1 corresponds to white and 0 - to black. The array should form a square with an odd side size for example, the structureing element 3*3 might be declared as
     * \code quint8 se[9] = {1,0,1,0,0,0,1,0,1}; \endcode
     *  The hotspot is in the center of the square.
     *  dimensions is the size of the side of the square. Odd values like 3, 5, 7 should be used for squares 3*3, 5*5, and 7*7 respectively.
     * \sa erode
    */

    QIPBlackAndWhiteImage dilate(quint8* structuringElement, int dimensions) const;

    /*!
     *  Returns an image obtained by morphologically eroding the original image with the structuring element defined by the structuringElement variable.
     *  structuringElement is an array of quint8 values where 1 corresponds to white and 0 - to black. The array should form a square with an odd side size for example, the structureing element 3*3 might be declared as
     * \code quint8 se[9] = {1,0,1,0,0,0,1,0,1}; \endcode
     *  The hotspot is in the center of the square.
     *  dimensions is the size of the side of the square. Odd values like 3, 5, 7 should be used for squares 3*3, 5*5, and 7*7 respectively.
     * \sa dilate
    */

    QIPBlackAndWhiteImage erode(quint8* structuringElement, int dimensions) const;

    /*!
     *  Returns an image obtained by morphologically opening the original image with the structuring element defined by the structuringElement variable.
     *  structuringElement is an array of quint8 values where 1 corresponds to white and 0 - to black. The array should form a square with an odd side size for example, the structureing element 3*3 might be declared as
     * \code quint8 se[9] = {1,0,1,0,0,0,1,0,1}; \endcode
     *  The hotspot is in the center of the square.
     *  dimensions is the size of the side of the square. Odd values like 3, 5, 7 should be used for squares 3*3, 5*5, and 7*7 respectively.
     * \sa close
    */

    QIPBlackAndWhiteImage open(quint8* structuringElement, int dimensions) const;

    /*!
     *  Returns an image obtained by morphologically closing the original image with the structuring element defined by the structuringElement variable.
     *  structuringElement is an array of quint8 values where 1 corresponds to white and 0 - to black. The array should form a square with an odd side size for example, the structureing element 3*3 might be declared as
     * \code quint8 se[9] = {1,0,1,0,0,0,1,0,1}; \endcode
     *  The hotspot is in the center of the square.
     *  dimensions is the size of the side of the square. Odd values like 3, 5, 7 should be used for squares 3*3, 5*5, and 7*7 respectively.
     * \sa open
    */

    QIPBlackAndWhiteImage close(quint8* structuringElement, int dimensions) const;

    /*!
      *  Returns an inverted image.
      */

    QIPBlackAndWhiteImage inverse() const;

    /*!
      *  Returns the image with black or white border frames cropped out.
      */
    QIPBlackAndWhiteImage crop() const;

    QRect cropGrayScaleImage(const QIPGrayscaleImage& image);

    bool isNull() const;

    void free();

    quint8* scanLine(quint32 y) const;

    quint8 pixel(int x, int y);

private:
    int w, h;
    //QSharedPointer<quint8> data;
    //std::shared_ptr<quint8> data;
    QByteArray data;
private:
    void toImageInternal(uchar* image, const IntRect& rect, int imageWidth) const;


    inline void setPixel(quint32 x, quint32 y, quint8 value);

    bool compareElements(quint8** se, quint8** w, int dimensions) const;

    IntRect cropInternal(bool upperLeft) const;

    void copyInternal(quint8* data, int x1, int x2, int y1, int y2) const;

    friend class QIPGrayscaleImage;

    friend class QIPConnectedComponents;

};

#endif // QIPBLACKANDWHITEIMAGE_H
