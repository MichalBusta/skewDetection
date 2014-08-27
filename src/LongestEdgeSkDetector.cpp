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
		probabilities.push_back(0.84);
		probabilities.push_back(0.25);
	}

	LongestEdgeSkDetector::~LongestEdgeSkDetector() {
		// TODO Auto-generated destructor stub
	}

	double LongestEdgeSkDetector::detectSkew( std::vector<cv::Point>& outerContour, cv::Mat* debugImage)
	{
		probMeasure1 = 0;
		probMeasure2 = 0;

		double angle = 0, atanAngle = 0;
		double QactLength=0, maxLength=0;
		double actLength=0, actAngle=0;
		double deltaX=0, deltaY=0;
		int counter=0;
		//double range = M_PI/2.0/45.0;
		double range = ignoreAngle*M_PI/180;

		vector<double> actAngles;
		vector<double> actLenghts;

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
			atanAngle=(deltaX)/(deltaY);
			actAngle =atan(atanAngle);
            
            //normalizing the angles
            
            while (actAngle > M_PI/2) {
                actAngle -= M_PI;
            }
            while (actAngle <= -M_PI/2){
                actAngle += M_PI;
            }

			if( (actAngle < (M_PI/2.0-range)) && (actAngle > (-M_PI/2.0+range)) )
			{
				actAngles.push_back(actAngle);
				actLenghts.push_back(actLength);
				if(actLength > maxLength)
				{
					maxLength=actLength;
					angle = actAngle;
					counter=c;
				}
			}
		}

		vector<double>::iterator itt = actAngles.begin();
		for(vector<double>::iterator it = actLenghts.begin(); it < actLenghts.end(); )
		{
			if( *it < ( maxLength - edgeRatio * maxLength ) )
			{
				it = actLenghts.erase(it);
				itt = actAngles.erase( itt );
			}else
			{
				it++;
				itt++;
			}
		}

		vector<double>actAngles2;
		vector<double>actLenghts2;

		probMeasure1 = actAngles.size();
        

		filterValuesBySimiliarAngle( actLenghts, actAngles, actLenghts2, actAngles2 );

		int anglesCount = 0;
		double anglesSum = 0;
		for(int c = 0;c < actLenghts2.size(); c++)
		{
			if( ( actLenghts2[c] != 0  ) )
			{
				probMeasure2++;
				anglesCount++;
				anglesSum += actAngles2[c];
			}
		}


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
            cv::Mat drawing;
			cv::Rect bbox = cv::boundingRect(outerContour);

			int brd = 10;
			drawing =  Mat::zeros( bbox.height*scalefactor+brd, bbox.width*scalefactor+brd, CV_8UC3 );
            *debugImage = drawing;
			Scalar color = Scalar( 255, 255, 255 );
			std::vector<std::vector<cv::Point> > contours;
			contours.push_back(outerContour);
            int min_Y = INT_MAX;
            int min_X = INT_MAX;
            for (size_t i=0; i<outerContour.size(); i++) {
            	min_Y = MIN(min_Y, outerContour[i].y);
            	min_X = MIN(min_X, outerContour[i].x);
            }
			for(size_t j = 0; j < outerContour.size(); j++)
			{
				int index = j+1;
				if(index >=  (outerContour.size()) )
					index = 0;

                cv::line(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2), cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), color);
				cv::circle(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2), 2, cv::Scalar(0, 255, 255), 1);

				cv::Scalar color = cv::Scalar( 255, 255, 255 );
				deltaX = outerContour[index].x - outerContour[j].x;
				deltaY = outerContour[index].y - outerContour[j].y;
				QactLength = (deltaX)*(deltaX) + (deltaY)*(deltaY);
				actLength = sqrt(QactLength);
				atanAngle=(deltaX)*1.0/(deltaY);
				actAngle =atan(atanAngle);
				if(actLength >= ( maxLength - edgeRatio * maxLength ))
					color = cv::Scalar( 0, 0, 255 );
				if((actAngle < (M_PI/2.0-range)) && (actAngle > (-M_PI/2.0+range)) )
					cv::line(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2),
							cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), color, 1, 0);
			}
			int index = counter + 1;
			if(counter == outerContour.size() - 1 )
				index = 0;
			cv::line(drawing,cv::Point((outerContour[counter].x-min_X)*scalefactor + brd / 2,(outerContour[counter].y-min_Y)*scalefactor + brd / 2),
					cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), cv::Scalar( 0, 255, 0 ), 1, 0);
			cv::circle(drawing, cv::Point((outerContour[counter].x-min_X)*scalefactor + brd / 2,(outerContour[counter].y-min_Y)*scalefactor + brd / 2), 4, cv::Scalar( 0, 255, 0 ), 1, 0);
			cv::circle(drawing, cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), 4, cv::Scalar( 0, 255, 0 ), 1, 0);
		}
        
        
		int index = (int) (probMeasure2 - 1);
		index = MIN(index, this->probabilities.size() - 1);
		lastDetectionProbability = probabilities[index];
        
        assert(anglesCount>0);
		if(probMeasure2 == 2)
			return - (anglesSum / anglesCount);

		//y-souradnice je opacne
		return -angle;

	}

} /* namespace cmp */
