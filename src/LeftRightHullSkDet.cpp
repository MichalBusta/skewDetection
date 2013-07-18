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
LeftRightHullSkDet::LeftRightHullSkDet(int approximatioMethod, double epsilon, float precision, bool isRight, bool contourForm): ContourSkewDetector(approximatioMethod, epsilon),  precision(precision), isRight(isRight), contourForm(contourForm)
{


}

LeftRightHullSkDet::~LeftRightHullSkDet()
{
	// TODO Auto-generated destructor stub
}

double LeftRightHullSkDet::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage )
{
	vector<Point> hull;
	convexHull( contours[0], hull, false, true);
	vector<Point> outerContour = contours[0];

	int topMost = mask.rows;
	int bottomMost = 0;
	float angle;
	if(contourForm==true)
	{
		for(int i=0;i<hull.size();i++)
		{
			if(hull[i].y < topMost)
			{
				topMost = hull[i].y;
			}
			if(hull[i].y > bottomMost)
			{
				bottomMost = hull[i].y;
			}
		}

		//vypocet velikosti pisma
		int letterSize = 0;
		letterSize = bottomMost - topMost;
		int addEdgeThickness = 0;
		addEdgeThickness = letterSize * precision;

		//ziskani souradnic X
		int TLX = mask.cols;
		int TRX = 0;
		int BLX = mask.cols;
		int BRX = 0;
		for (int c = 0; c < hull.size();c++)
		{
			if(hull[c].y < (topMost + addEdgeThickness))
			{
				TLX = MIN(TLX, hull[c].x);
				TRX = MAX(TRX, hull[c].x);
			}
			if(hull[c].y > (bottomMost - addEdgeThickness))
			{
				BLX = MIN(BLX, hull[c].x);
				BRX = MAX(BRX, hull[c].x);
			}
		}

		//konstrukce krajnich bodu ze souradic X a Y
		Point TL(TLX, topMost);
		Point TR(TRX, topMost);
		Point BL(BLX, bottomMost);
		Point BR(BRX, bottomMost);

		//pomocne body pro vztvareni usecek
		Point P1(0, topMost + 100*precision);
		Point P2(mask.rows, topMost + 100*precision);
		Point P3(0, bottomMost - 100*precision);
		Point P4(mask.rows, bottomMost - 100*precision);

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
			drawing =  Mat::zeros( mask.size(), CV_8UC3 );

			Scalar color = Scalar( 255, 255, 255 );
			drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

			cmp::filterContour(contours[0]);

			for(int i=0;i<hull.size();i++)
			{
				cv::circle(drawing, hull[i], 2, Scalar( 255, 0, 0 ), 2);
				cv::circle(drawing, TL, 4, Scalar( 0, 255, 255 ), 2);
				cv::circle(drawing, TR, 4, Scalar( 0, 255, 255 ), 2);
				cv::circle(drawing, BL, 4, Scalar( 0, 255, 0 ), 2);
				cv::circle(drawing, BR, 4, Scalar( 0, 255, 0 ), 2);

			}
			cv::line(drawing, P1, P2, cv::Scalar(255, 255, 0), 1 );
			cv::line(drawing, P3, P4, cv::Scalar(255, 255, 0), 1 );

		}
	}else 											// contourForm=false
	{
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
		int TLX = mask.cols;
		int TRX = 0;
		int BLX = mask.cols;
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
		Point P1(0, topMost + 100*precision);
		Point P2(mask.rows, topMost + 100*precision);
		Point P3(0, bottomMost - 100*precision);
		Point P4(mask.rows, bottomMost - 100*precision);

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
			drawing =  Mat::zeros( mask.size(), CV_8UC3 );

			Scalar color = Scalar( 255, 255, 255 );
			drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

			cmp::filterContour(contours[0]);

			for(int i=0;i<outerContour.size();i++)
			{
				cv::circle(drawing, outerContour[i], 2, Scalar( 255, 0, 0 ), 2);
				cv::circle(drawing, TL, 4, Scalar( 0, 255, 255 ), 2);
				cv::circle(drawing, TR, 4, Scalar( 0, 255, 255 ), 2);
				cv::circle(drawing, BL, 4, Scalar( 0, 255, 0 ), 2);
				cv::circle(drawing, BR, 4, Scalar( 0, 255, 0 ), 2);

			}
			cv::line(drawing, P1, P2, cv::Scalar(255, 255, 0), 1 );
			cv::line(drawing, P3, P4, cv::Scalar(255, 255, 0), 1 );

		}
	}
	return angle;
}

} /* namespace cmp */
