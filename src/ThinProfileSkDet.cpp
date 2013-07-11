/*
 * ThinProfileSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ThinProfileSkDet.h"

using namespace std;
using namespace cv;

namespace cmp
{

ThinProfileSkDet::ThinProfileSkDet()
{
	// TODO Auto-generated constructor stub

}

ThinProfileSkDet::~ThinProfileSkDet()
{
	// TODO Auto-generated destructor stub
}

double ThinProfileSkDet::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage)
{
	//TODO implement
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );

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
