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

} /* namespace cmp */

void cmp::filterContour(std::vector<cv::Point>& vector)
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

void cmp::SkewDetector::getBigestContour(
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
