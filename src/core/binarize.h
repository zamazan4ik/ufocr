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

#ifndef BINARIZE_H
#define BINARIZE_H

#if !defined(_qrcode_binarize_H)
# define _qrcode_binarize_H (1)

void qr_image_cross_masking_median_filter(unsigned char* _img,
                                          int _width, int _height);

void qr_wiener_filter(unsigned char* _img, int _width, int _height);

/*Binarizes a grayscale image.*/
void qr_binarize(unsigned char* _img, int _width, int _height);

unsigned char* qr_binarize1(const unsigned char* _img, int _width, int _height);

#endif

#endif // BINARIZE_H
