/*
 * CentersSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

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

bool sortPointY (const Point& i,const Point& j) { return (i.y < j.y); }

double CentersSkDet::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage)
{
	//TODO implement
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	std::vector<cv::Point>& outerCountour = contours[0];
	std::sort (outerCountour.begin(), outerCountour.end(), &sortPointY );

	for(size_t i = 0; i < outerCountour.size(); i++)
	{
		std::cout << "Point " << outerCountour[i].x << ", " << outerCountour[i].y << std::endl;
	}

	int minY = mask.rows;
	int maxY = 0;
	size_t maxIndex, minIndex;
	for(size_t i = 0; i < outerCountour.size(); i++)
	{
		cv::Point pt = outerCountour[i];
		if(pt.y > maxY)
		{
			maxIndex = i;
			maxY = pt.y;
		}
	}
	cv::Point bottomPoint = outerCountour[maxIndex];


	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );

		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 2, 8, hierarchy, 0, Point() );

		imshow( "Contours", drawing );
	}

	return 0;
}

} /* namespace cmp */

