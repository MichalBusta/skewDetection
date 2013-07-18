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


VerticalDomSkDet::VerticalDomSkDet(int approximatioMethod, double epsilon, int histColWidth, int sigma, int range, int ignoreAngle): ContourSkewDetector(approximatioMethod, epsilon), histColWidth(histColWidth), sigma(sigma), range(range), ignoreAngle(ignoreAngle) {
	// TODO Auto-generated constructor stub

	hist = new float [int(180/histColWidth)];
}

VerticalDomSkDet::~VerticalDomSkDet() {
	delete [] hist;
}

double VerticalDomSkDet::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)//, int sigma, int range, int ignoreAngle)
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

			hist[j] = hist[j] + length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i-ang)*(i-ang)/(2*sigma*sigma));
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
	}
	return (maxI*histColWidth+histColWidth/2)*M_PI/180-M_PI/2;
}

}//namespace cmp
