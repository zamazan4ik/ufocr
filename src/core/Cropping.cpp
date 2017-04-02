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

#include "Cropping.hpp"

#include "opencv2/imgproc.hpp"

bool isQuadrangle(const std::vector<cv::Point>& contour)
{
    return contour.size() == 4;
}

std::vector<cv::Point> IPL::getContour(const cv::Mat& src, size_t longSide)
{
    //TODO: Add more algorithms for border detecting
    //TODO: Research good constants for Canny edge detector
    int thresh = 50;

    //TODO: Add longside support
    cv::Mat src_gray;
    cv::cvtColor( src, src_gray, CV_BGR2GRAY );
    cv::blur( src_gray, src_gray, cv::Size(3,3) );

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh * 2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    std::vector<cv::Point> result;
    double maxArea = 0.0;
    for(auto contour : contours)
    {
        if(isQuadrangle(contour))
        {
            double area = cv::contourArea(contour);
            if(area > maxArea)
            {
                maxArea = area;
                result = contour;
            }
        }
    }
    return result;
}