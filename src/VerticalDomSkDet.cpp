/*
 * VerticalDomSkDet.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#define _USE_MATH_DEFINES
#include <math.h>
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
	
	cv::Point prev;
	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		if(it == contour.begin()) 
		{
			prev = (*it);
			it++;
		}
		
		cv::Point vector = (*it) - prev;
		double angle = atan2(double (vector.x), double (vector.y))*180/M_PI;
		double size = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		printf("%10.6f   %10.6f \n", angle, size);

		prev = (*it);
		it++;

	}

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
