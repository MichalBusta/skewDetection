/*
 * LongestEdgeSkDetector.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */
#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LongestEdgeSkDetector.h"

using namespace std;
using namespace cv;

namespace cmp {

LongestEdgeSkDetector::LongestEdgeSkDetector(int approximatioMethod, double epsilon, double ignoreAngle) : ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle)
{
	// TODO Auto-generated constructor stub

}

LongestEdgeSkDetector::~LongestEdgeSkDetector() {
	// TODO Auto-generated destructor stub
}

double LongestEdgeSkDetector::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
{
	std::vector<cv::Point>& outerContour = contours[0];

	double angle = 0, atanAngle = 0;
	double QactLength=0, maxLength=0;
	double actLength=0, actAngle=0;
	double deltaX=0, deltaY=0, actDeltaX=0, actDeltaY=0;
	int counter=0;
	//double range = M_PI/2.0/45.0;
	double range = ignoreAngle*M_PI/180;
	for(int c=0;c<outerContour.size();c++)
	{
		int index2 = c + 1;
		if(index2 > outerContour.size()-1)
			index2 = 0;

		//srovnani 1+2,.....predposledni+posledni
		actDeltaX = outerContour[index2].x - outerContour[c].x;
		actDeltaY = outerContour[index2].y - outerContour[c].y;
		QactLength = (actDeltaX)*(actDeltaX) + (actDeltaY)*(actDeltaY);
		actLength = sqrt(QactLength);
		if(actLength>maxLength && (actAngle < (M_PI/2.0-range)) && (actAngle > (-M_PI/2.0+range)) )
		{
			maxLength=actLength;
			deltaX=actDeltaX;
			deltaY=actDeltaY;
			atanAngle=(deltaX)*1.0/(deltaY);
			actAngle =atan(atanAngle);

			angle = actAngle;
			counter=c;
		}
	}

/*#ifdef VERBOSE
	cout << "maxLength is: " << maxLength << "\n";
	cout << "angle is: " << angle << "\n";\
#endif*/

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

		for(size_t j = 0; j < outerContour.size(); j++)
		{
			cv::circle(drawing, outerContour[j], 2, cv::Scalar(0, 255, 255), 2);
			if(counter==outerContour.size()-1)
			{
				cv::line(drawing, outerContour[outerContour.size()-1], outerContour[0], cv::Scalar(255, 255, 0), 1, 0);
				cv::circle(drawing, outerContour[outerContour.size()-1], 4, cv::Scalar(255, 255, 0), 1, 0);
				cv::circle(drawing, outerContour[0], 4, cv::Scalar(255, 255, 0), 1, 0);
			}
			else
			{
			cv::line(drawing, outerContour[counter], outerContour[counter+1], cv::Scalar(255, 255, 0), 1, 0);
			cv::circle(drawing, outerContour[counter], 4, cv::Scalar(255, 255, 0), 1, 0);
			cv::circle(drawing, outerContour[counter+1], 4, cv::Scalar(255, 255, 0), 1, 0);
			}
		}
	}

	//y-souradnice je opacne
	return -angle;

}

} /* namespace cmp */
