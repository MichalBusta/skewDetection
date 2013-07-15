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

using namespace std;
using namespace cv;

#include "VerticalDomSkDet.h"
#include "SkewDetector.h"

namespace cmp{


VerticalDomSkDet::VerticalDomSkDet() {
	// TODO Auto-generated constructor stub

}

VerticalDomSkDet::~VerticalDomSkDet() {
	// TODO Auto-generated destructor stub
}

double VerticalDomSkDet::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage) 
{
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );

	std::vector<cv::Point> contour = contours[0];
	filterContour(contour);

	float hist [72] = {0};
	int highestCol = 0;
	cv::Point prev = contour.back();
	std::cout << contour.size() << endl;
	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		cv::Point pt = *it;
		cv::Point vector = pt - prev;
		double angle = atan2(double (vector.x), double (vector.y))*180/M_PI;
		if (angle < 0) angle = angle + 360;
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		int ang = angle/5;

		hist[ang] = hist[ang] + length;

		if (hist[ang] > hist[highestCol]) highestCol = ang;

		printf("%10.6f   %10.6f \n", angle, length);

		prev = pt;
		it++;
	}

	int height = hist[highestCol]*2+20;

	cv::Mat histogram = Mat::zeros(height, 450, CV_8UC3);

	for(int i=0;i<72;i++)
	{
		int rectH = 2*hist[i];
		cv::rectangle(histogram, Rect(9+6*i, height-rectH+10, 6, rectH), Scalar(0,0,255), CV_FILLED);
	}

	cv::imshow("Histogram", histogram);


	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );
	}

	return 0;
}

}//namespace cmp
