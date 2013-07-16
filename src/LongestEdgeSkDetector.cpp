/*
 * LongestEdgeSkDetector.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LongestEdgeSkDetector.h"

using namespace std;
using namespace cv;

namespace cmp {

LongestEdgeSkDetector::LongestEdgeSkDetector() {
	// TODO Auto-generated constructor stub

}

LongestEdgeSkDetector::~LongestEdgeSkDetector() {
	// TODO Auto-generated destructor stub
}

double LongestEdgeSkDetector::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage)
{
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );

	std::vector<cv::Point> drawContour;
	if(debugImage != NULL)
	{
		drawContour = contours[0];
	}

	std::vector<cv::Point>& outerCountour = contours[0];

	double angle = 0;
	float actLength=0, QactLength=0, maxLength=0;
	int c=0, deltaX =0, deltaY=0, longNumber=0;
	for(c=0;c<outerCountour.size()-1;c++)
	{
		//srovnani 1+2,.....predposledni+posledni
		deltaX = outerCountour[c].x - outerCountour[c+1].x;
		deltaY = outerCountour[c].y - outerCountour[c+1].y;
		QactLength = (deltaX)^2 + (deltaY)^2;
		actLength = sqrt(QactLength);
		if(actLength>maxLength)
		{
			maxLength=actLength;
			angle = atan(double(deltaX/deltaY));
		}
		//srovnani prvniho a posledniho bodu
		deltaX = outerCountour[outerCountour.size()].x - outerCountour[0].x;
		deltaY = outerCountour[outerCountour.size()].y - outerCountour[0].y;
		QactLength = (deltaX)^2 + (deltaY)^2;
		actLength = sqrt(QactLength);
		if(actLength>maxLength)
		{
			maxLength=actLength;

		}
	}
	cout << maxLength << "\n";
	cout << angle << "\n";

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		contours[0] = drawContour;
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

		for(size_t j = 0; j < drawContour.size(); j++)
		{
			cv::circle(drawing, drawContour[j], 2, cv::Scalar(0, 255, 255), 2);
		}
	}

	return angle;
}

} /* namespace cmp */