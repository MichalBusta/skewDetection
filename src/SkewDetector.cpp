/*
 * SkewDetector.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: Michal Busta
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include "SkewDetector.h"

namespace cmp
{

SkewDetector::SkewDetector()
{
	// TODO Auto-generated constructor stub

}

SkewDetector::~SkewDetector()
{
	// TODO Auto-generated destructor stub
}

void filterContour(std::vector<cv::Point>& vector)
{
	double maxAllowedDivergence = 5; // uhel ve stupnich !!!
	maxAllowedDivergence = maxAllowedDivergence/180*M_PI; // prevod na radiany
	cv::Point prev;
	int del = 0;
	int notdel = 0;
	for(std::vector<cv::Point>::iterator it = vector.begin(); it < vector.end(); )
	{
		bool remove = false;
		
		if(it == vector.begin()) 
		{
			prev = (*it);
			it++;
		}
		std::vector<cv::Point>::iterator nextit = it;
		nextit++;
		if(nextit == vector.end()) 
		{
			break;
		}
		
		cv::Point v1 = (*it) - prev;
		cv::Point v2 = (*nextit) - (*it);
		double divergence = acos((v1.x*v2.x + v1.y*v2.y)/(sqrt(v1.x*v1.x+v1.y*v1.y+0.0)*sqrt(v2.x*v2.x+v2.y*v2.y+0.0)));

		prev = (*it);

		if(divergence <= maxAllowedDivergence)
		{
			it = vector.erase(it);
			del++;
		}else{
			it++;
			notdel++;
		}

	}
}

void ContourSkewDetector::getBigestContour(
		std::vector<std::vector<cv::Point> >& contours)
{

	std::vector<std::vector<cv::Point> > contours2;
	int maxArea=0;
	double y=0;
	int numberOfContour=0;
	if(contours.size() == 1)
		return;

	for(int c = 0; c < contours.size(); c++)
	{
		cv::Rect rect=boundingRect(contours[c]);
		y=rect.height*rect.width;
		if(y > maxArea)	//contour[numberOfContour] je nejvetsi contoura(na plochu)
		{
			maxArea = y;
			numberOfContour=c;
		}


	}

	contours2.push_back( contours[numberOfContour] );
	contours = contours2;

}

/**
 * Constructor
 * @param approximatioMethod the approximation method
 * @param epsilon if value > 0, the polygon
 */
ContourSkewDetector::ContourSkewDetector( int approximatioMethod, double epsilon ) : approximatioMethod( approximatioMethod ), epsilon(epsilon)
{

}

/**
 * The skew detection
 *
 * @param mask
 * @param lineK
 * @param debugImage
 * @return
 */
double ContourSkewDetector::detectSkew(cv::Mat& mask, double lineK, cv::Mat* debugImage )
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	/** find the contour */
	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, approximatioMethod, cv::Point(0, 0) );

	if( contours.size() == 0)
		return 0;

	if( contours.size() > 1)
		ContourSkewDetector::getBigestContour( contours );

	if(this->epsilon > 0)
	{
		std::vector<cv::Point> apCont;
		approxPolyDP(contours[0], apCont, epsilon, true);
		contours[0] = apCont;
	}

	return detectSkew(mask, contours, debugImage );
}

} /* namespace cmp */


