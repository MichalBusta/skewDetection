/*
 * CentersSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "CentersSkDet.h"

using namespace std;
using namespace cv;


namespace cmp
{

CentersSkDet::CentersSkDet(float precision) : precision(precision)
{

}

CentersSkDet::~CentersSkDet()
{
	// TODO Auto-generated destructor stub
}

bool sortPointY (const Point& i,const Point& j)
{
	if( i.y == j.y)
		return (i.x < j.x);
	return (i.y < j.y);
}

double CentersSkDet::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage)
{

	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
	
	if (contours[0].size() < 3) return 0;
	std::vector<cv::Point> drawContour;
	if(debugImage != NULL)
	{
		drawContour = contours[0];
	}

	std::vector<cv::Point>& outerContour = contours[0];
	std::sort (outerContour.begin(), outerContour.end(), &sortPointY );

	//ziskani souradnic Y
	int topPoint = mask.rows;
	int bottomPoint = 0;
	for (int c = 0; c < outerContour.size();c++)
	{
		if(outerContour[c].y < topPoint)
		{
			topPoint = outerContour[c].y;
		}
		if(outerContour[c].y > bottomPoint)
		{
			bottomPoint = outerContour[c].y;
		}
	}

	//vypocet velikosti pisma
	int letterSize = 0;
	letterSize = bottomPoint - topPoint;
	int addEdgeThickness = 0;
	addEdgeThickness = letterSize * precision;

	//ziskani souradnic X
	int TLX = mask.cols;
	int TRX = 0;
	int BLX = mask.cols;
	int BRX = 0;
	for (int c = 0; c < outerContour.size();c++)
	{
		if(outerContour[c].y < (topPoint + addEdgeThickness))
		{
			TLX = MIN(TLX, outerContour[c].x);
			TRX = MAX(TRX, outerContour[c].x);
		}
		if(outerContour[c].y > (bottomPoint - addEdgeThickness))
		{
			BLX = MIN(BLX, outerContour[c].x);
			BRX = MAX(BRX, outerContour[c].x);
		}
	}
	//konstrukce krajnich bodu ze souradic X a Y
	Point TL(TLX, topPoint);
	Point TR(TRX, topPoint);
	Point BL(BLX, bottomPoint);
	Point BR(BRX, bottomPoint);

	//pomocne body pro vztvareni usecek
	Point P1(0, topPoint + 100*precision);
	Point P2(mask.rows, topPoint + 100*precision);
	Point P3(0, bottomPoint - 100*precision);
	Point P4(mask.rows, bottomPoint - 100*precision);

	//ziskani prostrednich bodu
	Point TM((TL.x + TR.x)/2.0,TL.y);
	Point BM((BL.x + BR.x)/2.0,BR.y);

	//vypocet uhlu zkoseni
	float angle=0, deltaX=0, deltaY=0;
	deltaX = TM.x - BM.x;
	deltaY = BM.y - TM.y;
	angle = atan((deltaX)*1.0/(deltaY));
	//uhel promenne angle je v radianech

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

		cv::line(drawing, P1, P2, cv::Scalar(255, 255, 0), 1 );
		cv::line(drawing, P3, P4, cv::Scalar(255, 255, 0), 1 );
		cv::line(drawing, TL, TR, cv::Scalar(0, 255, 0), 2 );
		cv::line(drawing, BL, BR, cv::Scalar(0, 255, 0), 2 );
		cv::circle(drawing, TM, 4, cv::Scalar(0, 0, 255), 2);
		cv::circle(drawing, BM, 4, cv::Scalar(0, 0, 255), 2);
	}

	return angle;
}

} /* namespace cmp */

