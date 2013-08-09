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

	LongestEdgeSkDetector::LongestEdgeSkDetector(int approximatioMethod, double epsilon, double ignoreAngle, double edgeRatio) : 
		ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), edgeRatio(edgeRatio)
	{
		// TODO Auto-generated constructor stub

	}

	LongestEdgeSkDetector::~LongestEdgeSkDetector() {
		// TODO Auto-generated destructor stub
	}

	double LongestEdgeSkDetector::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
	{
		std::vector<cv::Point>& outerContour = contours[0];

		probMeasure1 = 0;
		probMeasure2 = 0;

		double angle = 0, atanAngle = 0;
		double QactLength=0, maxLength=0;
		double actLength=0, actAngle=0;
		double deltaX=0, deltaY=0;
		int counter=0;
		//double range = M_PI/2.0/45.0;
		double range = ignoreAngle*M_PI/180;
		for(int c=0;c<outerContour.size();c++)
		{
			int index2 = c + 1;
			if(index2 >= outerContour.size())
				index2 = 0;

			//srovnani 1+2,.....predposledni+posledni
			deltaX = outerContour[index2].x - outerContour[c].x;
			deltaY = outerContour[index2].y - outerContour[c].y;
			QactLength = (deltaX)*(deltaX) + (deltaY)*(deltaY);
			actLength = sqrt(QactLength);
			atanAngle=(deltaX)*1.0/(deltaY);
			actAngle =atan(atanAngle);

			if(actLength > maxLength && (actAngle < (M_PI/2.0-range)) && (actAngle > (-M_PI/2.0+range)) )
			{
				maxLength=actLength;
				angle = actAngle;
				counter=c;

			}
		}

		vector<double>actAngles;
		vector<double>actLenghts;
		vector<double>actAngles2;
		vector<double>actLenghts2;


		//double range = M_PI/2.0/45.0;
		for(int c=0;c<outerContour.size();c++)
		{
			int index2 = c + 1;
			if(index2 >= outerContour.size())
				index2 = 0;

			//srovnani 1+2,.....predposledni+posledni
			deltaX = outerContour[index2].x - outerContour[c].x;
			deltaY = outerContour[index2].y - outerContour[c].y;
			QactLength = (deltaX)*(deltaX) + (deltaY)*(deltaY);
			actLength = sqrt(QactLength);
			atanAngle=(deltaX)*1.0/(deltaY);
			actAngle =atan(atanAngle);

			actAngles.push_back(actAngle);
			actLenghts.push_back(actLength);

			if( actAngle < (fabs (M_PI/2.0-range)))
			{
				if(actLength >= ( maxLength - edgeRatio * maxLength ) )
				{
					probMeasure2 += actLength;
					//probMeasure1++;
				}
			}
		}

		vector<bool>longestEdgeConditions;
		bool longestEdgeCondition;

		// condition to filter shorter edges
		for(int c=0;c<outerContour.size();c++)
		{
			if( actLenghts[c] >= ( maxLength - edgeRatio * maxLength )) longestEdgeCondition = true;
			else longestEdgeCondition = false;
			longestEdgeConditions.push_back(longestEdgeCondition);
		}

		// function to filter values by similiar angle
		filterValuesBySimiliarAngle(actLenghts,actAngles,actLenghts2,actAngles2, longestEdgeConditions);

		// counting edges filtered by filterValuesBySimiliarAngle
		for(int c=0;c<outerContour.size();c++) if( ( actLenghts2[c] != 0 ) && (actLenghts[c] >= ( maxLength - edgeRatio * maxLength ) ) ) probMeasure1++;

		// ?????
		probMeasure2 = probMeasure2 / maxLength;


#ifdef VERBOSE
		cout << "edgesLengthInRange is: " << probMeasure2 << "\n";
		cout << "noOfEdgesInRange is: " << probMeasure1 << "\n";
#endif

		/*#ifdef VERBOSE
		cout << "maxLength is: " << maxLength << "\n";
		cout << "angle is: " << angle << "\n";
		#endif*/

		if(debugImage != NULL)
		{
			Mat& drawing =  *debugImage;
			drawing =  Mat::zeros( mask.size(), CV_8UC3 );
			Scalar color = Scalar( 255, 255, 255 );
			drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

			int index;
			for(size_t j = 0; j < outerContour.size(); j++)
			{
				cv::circle(drawing, outerContour[j], 2, cv::Scalar(0, 255, 255), 1);
				int index = j+1;
				if(index >=  outerContour.size()) 
					index = 0;
				cv::Scalar color = cv::Scalar( 255, 255, 255 );
				deltaX = outerContour[index].x - outerContour[j].x;
				deltaY = outerContour[index].y - outerContour[j].y;
				QactLength = (deltaX)*(deltaX) + (deltaY)*(deltaY);
				actLength = sqrt(QactLength);
				atanAngle=(deltaX)*1.0/(deltaY);
				actAngle =atan(atanAngle);
				if(actLength >= ( maxLength - edgeRatio * maxLength ))
				{
					color = cv::Scalar( 0, 255, 0 );
				}
				if((actAngle < (M_PI/2.0-range)) && (actAngle > (-M_PI/2.0+range)) )
				{
					cv::line(drawing, outerContour[j], outerContour[index], color, 1, 0);
				}

			}

			index = counter + 1;
			if(counter==outerContour.size()-1)
				index = 0;
			cv::line(drawing, outerContour[counter], outerContour[index], cv::Scalar( 0, 0, 255 ), 1, 0);
			cv::circle(drawing, outerContour[counter], 4, cv::Scalar( 0, 0, 255 ), 1, 0);
			cv::circle(drawing, outerContour[index], 4, cv::Scalar( 0, 0, 255 ), 1, 0);
		}

		//y-souradnice je opacne
		return -angle;

	}

} /* namespace cmp */
