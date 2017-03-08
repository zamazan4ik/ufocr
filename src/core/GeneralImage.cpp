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

#include "GeneralImage.hpp"

GeneralImage::GeneralImage(const cv::Mat& val) : img_(val.clone())
{
}

GeneralImage::GeneralImage(const QImage& val) :
        img_(cv::Mat(val.height(), val.width(), CV_8UC3,
                     const_cast<uchar*>(val.bits()), val.bytesPerLine()).clone())
{
}

QImage GeneralImage::toQImage(QImage::Format format /*= QImage::Format_ARGB32*/) const
{
    return QImage(img_.data, img_.cols, img_.rows,
                  img_.step, format).copy();
}

cv::Mat& GeneralImage::Ref()
{
    return img_;
}

cv::Mat GeneralImage::toMat() const
{
    return img_.clone();
}
