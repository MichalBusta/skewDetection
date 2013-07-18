/*
 * LongestEdgeSkDetector.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LongestEdgeSkDetector.h"

using namespace std;
using namespace cv;

namespace cmp {

LongestEdgeSkDetector::LongestEdgeSkDetector(int approximatioMethod, double epsilon) : ContourSkewDetector(approximatioMethod, epsilon)
{
	// TODO Auto-generated constructor stub

}

LongestEdgeSkDetector::~LongestEdgeSkDetector() {
	// TODO Auto-generated destructor stub
}

double LongestEdgeSkDetector::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
{
	std::vector<cv::Point>& outerContour = contours[0];
	double epsilon = 0.01 * mask.rows;
	std::vector<cv::Point> apCont;
	if(epsilon > 0){
		approxPolyDP(outerContour, apCont, epsilon, true);
		outerContour = apCont;
	}




	double angle = 0, atanAngle = 0;
	double angle2 = 0, atanAngle2 = 0;
	double QactLength=0, maxLength=0;
	double secondLength=0, actLength=0;
	double deltaX=0, deltaY=0, deltaX2=0, deltaY2=0, actDeltaX=0, actDeltaY=0;
	for(int c=0;c<outerContour.size();c++)
	{
		if(c<outerContour.size())
		{
			//srovnani 1+2,.....predposledni+posledni
			actDeltaX = outerContour[c+1].x - outerContour[c].x;
			actDeltaY = outerContour[c+1].y - outerContour[c].y;
			QactLength = (actDeltaX)*(actDeltaX) + (actDeltaY)*(actDeltaY);
			actLength = sqrt(QactLength);
			if(actLength>maxLength)
			{
				secondLength=maxLength;
				maxLength=actLength;
				deltaX2=deltaX;
				deltaY2=deltaY;
				deltaX=actDeltaX;
				deltaY=actDeltaY;
				atanAngle=(deltaX)*1.0/(deltaY);
				atanAngle2=(deltaX2)*1.0/(deltaY2);
				angle = -atan(atanAngle);
				angle2 = -atan(atanAngle2);
			}
			else if (actLength>secondLength)
			{
				secondLength=actLength;
				deltaX2=actDeltaX;
				deltaY2=actDeltaY;
				atanAngle2=(deltaX2)*1.0/(deltaY2);
				angle2 = -atan(atanAngle2);
			}
		}else 			//c=outerContour.size()
		{
			//srovnani posledniho a prvniho bodu
			actDeltaX = outerContour[outerContour.size()].x - outerContour[0].x;
			actDeltaY = outerContour[outerContour.size()].y - outerContour[0].y;
			QactLength = (actDeltaX)*(actDeltaX) + (actDeltaY)*(actDeltaY);
			actLength = sqrt(QactLength);
			if(actLength>maxLength)
			{
				secondLength=maxLength;
				maxLength=actLength;
				deltaX2=deltaX;
				deltaY2=deltaY;
				deltaX=actDeltaX;
				deltaY=actDeltaY;
				atanAngle=(deltaX)*1.0/(deltaY);
				atanAngle2=(deltaX2)*1.0/(deltaY2);
				angle = -atan(atanAngle);
				angle2 = -atan(atanAngle2);
			}
			else if (actLength>secondLength)
			{
				secondLength=actLength;
				deltaX2=actDeltaX;
				deltaY2=actDeltaY;
				atanAngle2=(deltaX2)*1.0/(deltaY2);
				angle2 = -atan(atanAngle2);
			}
		}
	}
	double avAngle=0;
	avAngle = (angle + angle2)/2.0;

	cout << "maxLength is: " << maxLength << "\n";
	cout << "angle is: " << angle << "\n";\
	cout << "secondLength is: " << secondLength << "\n";
	cout << "angle2 is: " << angle2 << "\n";
	cout << "avAngle is: " << avAngle << "\n";

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

		for(size_t j = 0; j < outerContour.size(); j++)
		{
			cv::circle(drawing, outerContour[j], 2, cv::Scalar(0, 255, 255), 2);
		}
	}

	return angle;

}

} /* namespace cmp */
