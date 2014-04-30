/*
* ThinProfileSkDet.cpp
*
*  Created on: Jul 11, 2013
*      Author: Jakub Cidlik
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "LeftRightHullSkDet.h"
#include "SkewDetector.h"

using namespace std;
using namespace cv;

namespace cmp
{

	/**
	* @param precision
	* @param isRight
	* @param contourForm
	*/
	LeftRightHullSkDet::LeftRightHullSkDet(int approximatioMethod, double epsilon, float precision, bool isRight, int ignoreAngle): 
		ContourSkewDetector(approximatioMethod, epsilon),  precision(precision), isRight(isRight), ignoreAngle(ignoreAngle)
	{


	}

	LeftRightHullSkDet::~LeftRightHullSkDet()
	{
		// TODO Auto-generated destructor stub
	}

	double LeftRightHullSkDet::detectSkew( std::vector<cv::Point>& outerContour, cv::Mat* debugImage )
	{
		cv::Rect bbox = cv::boundingRect(outerContour);
		int topMost = bbox.height;
		int bottomMost = 0;
		float angle;

		for(int i=0;i<outerContour.size();i++)
		{
			if(outerContour[i].y < topMost)
			{
				topMost = outerContour[i].y;
			}
			if(outerContour[i].y > bottomMost)
			{
				bottomMost = outerContour[i].y;
			}
		}

		//vypocet velikosti pisma
		int letterSize = 0;
		letterSize = bottomMost - topMost;
		int addEdgeThickness = 0;
		addEdgeThickness = letterSize * precision;

		//ziskani souradnic X
		int TLX = bbox.width;
		int TRX = 0;
		int BLX = bbox.width;
		int BRX = 0;
		for (int c = 0; c < outerContour.size();c++)
		{
			if(outerContour[c].y < (topMost + addEdgeThickness))
			{
				TLX = MIN(TLX, outerContour[c].x);
				TRX = MAX(TRX, outerContour[c].x);
			}
			if(outerContour[c].y > (bottomMost - addEdgeThickness))
			{
				BLX = MIN(BLX, outerContour[c].x);
				BRX = MAX(BRX, outerContour[c].x);
			}
		}

		//konstrukce krajnich bodu ze souradic X a Y
		Point TL(TLX, topMost);
		Point TR(TRX, topMost);
		Point BL(BLX, bottomMost);
		Point BR(BRX, bottomMost);

		//pomocne body pro vztvareni usecek
		Point P1(0, topMost + addEdgeThickness);
		Point P2(bbox.height, topMost + addEdgeThickness);
		Point P3(0, bottomMost - addEdgeThickness);
		Point P4(bbox.height, bottomMost - addEdgeThickness);

		int deltaX=0, deltaY=0;
		if(isRight)
		{
			deltaX = TR.x - BR.x;
			deltaY = TR.y - BR.y;
			angle = atan((deltaX)*1.0/(deltaY));
		}else										// isRight=false
		{
			deltaX = TL.x - BL.x;
			deltaY = TL.y - BL.y;
			angle = atan((deltaX)*1.0/(deltaY));
		}

		if(debugImage != NULL)
		{
			Mat& drawing =  *debugImage;

			drawing =  Mat::zeros( bbox.height, bbox.width, CV_8UC3 );

			Scalar color = Scalar( 255, 255, 255 );
			std::vector<std::vector<cv::Point> > contours;
			contours.push_back(outerContour);
			drawContours( drawing, contours, 0, color, 1, 8);

			for(int i=0;i<outerContour.size();i++)
			{
				cv::circle(drawing, outerContour[i], 1, Scalar( 0, 0, 255 ), 1,5);
			}

			cv::line(drawing, P1, P2, cv::Scalar(255, 255, 0), 1 );
			cv::line(drawing, P3, P4, cv::Scalar(255, 255, 0), 1 );			

			cv::circle(drawing, TL, 4, Scalar( 0, 255, 255 ), 1);
			cv::circle(drawing, TR, 4, Scalar( 0, 255, 255 ), 1);
			cv::circle(drawing, BL, 4, Scalar( 0, 255, 0 ), 1);
			cv::circle(drawing, BR, 4, Scalar( 0, 255, 0 ), 1);
		}

		//y-souradnice je opacne
		return -angle;
	}

} /* namespace cmp */
