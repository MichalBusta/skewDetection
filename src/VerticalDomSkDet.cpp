/*
 * VerticalDomSkDet.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#define histColWidth 1
//#define sigma 2
//#define range 10
//#define ignoreAngle 10

using namespace std;
using namespace cv;

#include "VerticalDomSkDet.h"
#include "SkewDetector.h"

namespace cmp{


VerticalDomSkDet::VerticalDomSkDet(int approximatioMethod, double epsilon, int histColWidth, int sigma, int range, int ignoreAngle, int correctAngle): ContourSkewDetector(approximatioMethod, epsilon), histColWidth(histColWidth), sigma(sigma), range(range), ignoreAngle(ignoreAngle), correctAngle(correctAngle) {
	// TODO Auto-generated constructor stub

	hist = new float [int(180/histColWidth)];
}

VerticalDomSkDet::~VerticalDomSkDet() {
	delete [] hist;
}

double VerticalDomSkDet::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
{
	std::vector<cv::Point> contour = contours[0];
	filterContour(contour);
	
	memset (hist, 0, int(180/histColWidth) * sizeof(float));

	cv::Point prev = contour.back();
	std::cout << contour.size() << endl;
	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		cv::Point pt = *it;
		cv::Point vector = pt - prev;
		double angle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (angle < 0) angle = angle + 180;
		if (angle >= 180) angle = angle - 180;
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		int ang = angle/histColWidth;
		//hist[ang] = hist[ang] + length;
		//if (hist[ang] > hist[highestCol]) highestCol = ang;

		for (int i = ang-sigma*range; i <= ang+sigma*range; i++)
		{
			int j = i;
			if (j<0) j = j + int(180/histColWidth);
			if (j>=int(180/histColWidth)) j = j - int(180/histColWidth);

			hist[j] = hist[j] + length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i*histColWidth+histColWidth/2-angle)*(i*histColWidth+histColWidth/2-angle)/(2*sigma*sigma));
		}
		prev = pt;
		it++;
	}

	int height = 300;

	cv::Mat histogram = Mat::zeros(height, 380, CV_8UC3);
	int maxI = ceil(double(ignoreAngle/histColWidth));
	for(int i=0;i<int(180/histColWidth);i++)
	{
		int rectH = 2*hist[i];
		cv::rectangle(histogram, Rect(10+histColWidth*i*2, height-rectH-10, histColWidth*2, rectH), Scalar(0,0,255), CV_FILLED);
		if (hist[i] > hist[maxI] && i > ignoreAngle/histColWidth && i < (180-ignoreAngle)/histColWidth) maxI = i;

	}
	cv::imshow("Histogram", histogram);

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

		for (size_t i = 0; i < contour.size(); i++)
		{
			size_t i2 = (i==contour.size()-1) ? 0 : i+1;

			cv::circle(drawing, contour[i], 2, Scalar( 0, 0, 255 ), 1);

			double ang = atan2(double(contour[i2].y-contour[i].y), double(contour[i2].x-contour[i].x));
			if (ang < 0) ang = ang + M_PI;
			if (abs(ang-(maxI*histColWidth+histColWidth/2)*M_PI/180) < correctAngle*M_PI/180)
			{
				cv::line(drawing, contour[i], contour[i2], Scalar( 0, 255, 0 ), 1);
			}
		}
	}

	return (maxI*histColWidth+histColWidth/2)*M_PI/180-M_PI/2;
}

}//namespace cmp
