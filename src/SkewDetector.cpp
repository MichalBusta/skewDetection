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

void ContourSkewDetector::filterValuesBySimiliarAngle(
		const std::vector<double>& values, const std::vector<double>& angles,
		std::vector<double>& valuesOut, std::vector<double>& anglesOut,
		std::vector<bool>condition, double angleRange)
{
	anglesOut = angles;
	valuesOut = values;

	for(int c=0;c<values.size();c++)
	{
		if(condition[c] == true)
		{
			int greatestAngle = -M_PI;
			int smallestAngle = M_PI;

			double anglesSum = angles[c];
			int anglesCount = 1;

			for(int i = (c + 1); i < values.size() ; i++)
			{
				if( ( valuesOut[c] != 0 ) && ( fabs (angles[c] - angles[i]) < angleRange ) )
				{	
					anglesSum += angles[i];
					anglesCount++;
					valuesOut[i] = 0;
					anglesOut[i] = 0;
				}
				else anglesOut[i] = angles[i];
			}
			anglesOut[c] = anglesSum / anglesCount;
		}
	}
}

SkewDetector::SkewDetector() : lastDetectionProbability(0.5), probMeasure1(0), probMeasure2(0)
{
	// TODO Auto-generated constructor stub

}

SkewDetector::~SkewDetector()
{
	// TODO Auto-generated destructor stub
}

void ContourSkewDetector::getBigestContour(
		std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy)
{

	std::vector<std::vector<cv::Point> > contours2;
	std::vector<cv::Vec4i> hierarchy2;
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
	hierarchy2.push_back( hierarchy[numberOfContour] );
	hierarchy = hierarchy2;

}

/**
 * Constructor
 * @param approximatioMethod the approximation method
 * @param epsilon if value > 0, the polygon
 */
ContourSkewDetector::ContourSkewDetector( int approximatioMethod, double epsilon ) : SkewDetector(), approximatioMethod( approximatioMethod ), epsilon(epsilon)
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
		ContourSkewDetector::getBigestContour( contours, hierarchy );

	if(this->epsilon > 0)
	{
		cv::Rect rect= cv::boundingRect(contours[0]);
		int size = MIN(rect.width, rect.height);
		double absEpsilon = epsilon * size;
		std::vector<cv::Point> apCont;
		approxPolyDP(contours[0], apCont, absEpsilon, true);
		contours[0] = apCont;
	}

	return detectSkew(mask, contours, hierarchy, debugImage );
}

} /* namespace cmp */

