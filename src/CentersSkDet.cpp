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

CentersSkDet::CentersSkDet()
{
	// TODO Auto-generated constructor stub

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

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );

	std::vector<cv::Point> drawContour;
	if(debugImage != NULL)
	{
		drawContour = contours[0];
	}

	std::vector<cv::Point>& outerCountour = contours[0];
	std::sort (outerCountour.begin(), outerCountour.end(), &sortPointY );

	//definice 4 krajnich bodu
	Point TL=outerCountour[0], TR, BL, BR=outerCountour[outerCountour.size()-1] ;

	//cykly na nalezeni zbylych dvou bodu (TR a BL)
	for (int c = 0; c < outerCountour.size();c++)
	{
		if(outerCountour[c].y > TL.y)
		{
			TR = outerCountour[c-1];
			break;

		}
	}
	for (int c = outerCountour.size()-1; c > 0;c--)
	{
		if(outerCountour[c].y < BR.y)
		{
			BL = outerCountour[c+1];
			break;
		}
	}
	//ziskani prostrednich bodu
	Point TM, BM;
	TM.y = TL.y;
	TM.x = (TL.x + TR.x)/2.0;
	BM.y = BR.y;
	BM.x = (BL.x + BR.x)/2.0;



	float angle=0, deltaX=0, deltaY=0;
	deltaX = TM.x - BM.x;
	deltaY = BM.y - TM.y;
	angle = atan( deltaX/deltaY);
	//uhel promenne angle je v radianech

	std::cout << "Top Center: " << TM.x << ", " << TM.y << std::endl;
	std::cout << "Bottom Center: " << BM.x << ", " << BM.y << std::endl;

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

		cv::line(drawing, TL, TR, cv::Scalar(0, 255, 0), 2 );
		cv::line(drawing, BL, BR, cv::Scalar(0, 255, 0), 2 );
		cv::circle(drawing, TM, 4, cv::Scalar(0, 0, 255), 2);
		cv::circle(drawing, BM, 4, cv::Scalar(0, 0, 255), 2);
	}

	return angle;
}

} /* namespace cmp */

