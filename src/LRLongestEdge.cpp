/*
 * LRLongestEdge.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: Michal Busta
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "LRLongestEdge.h"
#include "SkewDetector.h"

namespace cmp
{


LRLongestEdge::LRLongestEdge(int approximatioMethod, double epsilon, int ignoreAngle, bool left) : ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), left(left)
{
}

LRLongestEdge::~LRLongestEdge()
{
	// TODO Auto-generated destructor stub
}

double LRLongestEdge::detectSkew(const cv::Mat& mask,
		std::vector<std::vector<cv::Point> >& contours,
		std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
{
	if (contours[0].size() < 3) return 0;

	size_t topMost = 0;
	size_t bottomMost = 0;
	for(size_t i=0;i<contours[0].size();i++)
	{
		if(contours[0][i].y < contours[0][topMost].y)
		{
			topMost = i;
		}
		else if(contours[0][i].y > contours[0][bottomMost].y)
		{
			bottomMost = i;
		}
	}

	size_t start, end;

	if(left)
	{
		start = topMost;
		end = bottomMost;
	}
	else 
	{
		end = topMost;
		start = bottomMost;
	}

	size_t maxI = start;
	cv::Point maxVector;

	double maxLen = 0;
	double angle = 0;

	for(size_t i = start; i != end;) 
	{
		size_t next = (i>=contours[0].size()-1) ? 0 : i+1;

		cv::Point2d tmpVector = contours[0][next]-contours[0][i];

		double len = sqrt(tmpVector.x*tmpVector.x+tmpVector.y*tmpVector.y);

		if(len>maxLen)
		{
			double ang = atan2(tmpVector.y, tmpVector.x);
			ang = ang + M_PI/2;
			while (ang > M_PI/2) ang = ang - M_PI;
			while (ang <= -M_PI/2) ang = ang + M_PI;

			if((ang >= (M_PI/180*ignoreAngle-M_PI/2) && ang <= (M_PI/2-M_PI/180*ignoreAngle)))
			{
				maxLen = len;
				maxI = i;
				maxVector = tmpVector;
				angle = ang;
			}
		}

		i = next;
	}

	if(debugImage != NULL)
	{
		cv::Mat& drawing =  *debugImage;
		drawing =  cv::Mat::zeros( mask.size(), CV_8UC3 );
		
		cv::Scalar color = cv::Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, cv::Point() );
		
		//cmp::filterContour(contours[0]);

		for(int i=0;i<contours[0].size();i++)
		{
			cv::circle(drawing, contours[0][i], 2, cv::Scalar( 255, 0, 0 ), 1);
		}
		cv::Point p = contours[0][maxI] + maxVector;
		cv::line(drawing, contours[0][maxI], p, cv::Scalar( 0, 0, 255 ), 1);

		//cv::circle(drawing, contours[0][maxI], 3, cv::Scalar( 0, 255, 255 ), 2);

	}


	return angle;
}

} /* namespace cmp */
