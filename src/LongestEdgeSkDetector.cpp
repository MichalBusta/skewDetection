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

LongestEdgeSkDetector::LongestEdgeSkDetector(int approximatioMethod, double epsilon, double ignoreAngle, double edgeRatio, bool normalizeLength) :
				ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), edgeRatio(edgeRatio), normalizeLength(normalizeLength)
{
	probabilities.push_back(0.7);
	probabilities.push_back(0.25);
}

LongestEdgeSkDetector::~LongestEdgeSkDetector() {
	// TODO Auto-generated destructor stub
}

double LongestEdgeSkDetector::detectSkew( std::vector<cv::Point>& contour, double lineK, bool approximate, cv::Mat* debugImage )
{
	std::vector<cv::Point> workCont;
	if( approximate )
	{
		approximateContour(contour, workCont);
	}else
	{
		workCont = contour;
	}
	double angle = doEstimate( workCont, debugImage );
	return angle;
}

double LongestEdgeSkDetector::doEstimate( std::vector<cv::Point>& outerContour, cv::Mat* debugImage)
{
	probMeasure1 = 0;
	probMeasure2 = 0;

	double maxLength=0;
	double actAngle=0;
	double deltaX=0, deltaY=0;
	int counter=0;
	double range = ignoreAngle*M_PI/180;

	vector<double> actAngles;
	vector<double> actLenghts;

	double angle = 0;

	for(int c=0;c<outerContour.size();c++)
	{
		int index2 = c + 1;
		if(index2 >= outerContour.size())
			index2 = 0; //srovnani 1+2,.....predposledni+posledni

		cv::Point vector = outerContour[index2] - outerContour[c];
		double currentAngle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (currentAngle < 0) currentAngle += 180;
		if (currentAngle >= 180) currentAngle -= 180;
		assert(currentAngle >= 0 && currentAngle <= 180);
		if (! (currentAngle > ignoreAngle && currentAngle < (180-ignoreAngle)) )
		{
			continue;
		}
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		if(normalizeLength)
		{
			double norm = sin(currentAngle * M_PI/180);
			length *= norm;
		}
		actAngles.push_back(currentAngle * M_PI/180);
		actLenghts.push_back(length);
		if(length > maxLength)
		{

			maxLength=length;
			angle = currentAngle * M_PI/180;
			counter=c;
		}
	}

	vector<double>actAnglesOrig = actAngles;
	vector<double>actLenghtsOrig = actLenghts;

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
	filterValuesBySimiliarAngle( actLenghtsOrig, actAnglesOrig, actLenghts2, actAngles2 );

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
		drawing =  Mat::zeros( bbox.height*scalefactor+brd, bbox.width*scalefactor+brd, CV_8UC3 ) + cv::Scalar(255, 255, 255);
		*debugImage = drawing;
		Scalar color = Scalar( 0, 0, 0 );
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
			cv::circle(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2), 2, Scalar( 255, 0, 0 ), 2);

			cv::Scalar color = cv::Scalar( 0, 0, 0 );

			cv::Point vector = outerContour[index] - outerContour[j];
			double currentAngle = atan2(double (vector.y), double (vector.x))*180/M_PI;
			if (currentAngle < 0) currentAngle += 180;
			if (currentAngle >= 180) currentAngle -= 180;

			if (! (currentAngle > ignoreAngle && currentAngle < (180-ignoreAngle)) )
			{
				continue;
			}

			double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);
			//if(length >= ( maxLength - edgeRatio * maxLength ))
			//	color = cv::Scalar( 0, 0, 255 );

			cv::line(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2),
					cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), color, 1, 0);
		}
		int index = counter + 1;
		if(counter == outerContour.size() - 1 )
			index = 0;
		cv::line(drawing,cv::Point((outerContour[counter].x-min_X)*scalefactor + brd / 2,(outerContour[counter].y-min_Y)*scalefactor + brd / 2),
				cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), cv::Scalar( 0, 255, 0 ), 2, 0);
		//cv::circle(drawing, cv::Point((outerContour[counter].x-min_X)*scalefactor + brd / 2,(outerContour[counter].y-min_Y)*scalefactor + brd / 2), 4, cv::Scalar( 0, 255, 0 ), 1, 0);
		//cv::circle(drawing, cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), 4, cv::Scalar( 0, 255, 0 ), 1, 0);
	}


	int index = (int) (probMeasure2 - 1);
	index = MIN(index, this->probabilities.size() - 1);
	lastDetectionProbability = 0.65;

	assert(anglesCount>0);
	//if(probMeasure2 == 2)
		//return - (anglesSum / anglesCount);

	//y-souradnice je opacne
	return angle - M_PI/2;
}

void LongestEdgeSkDetector::voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate, cv::Mat* debugImage)
{
	double angle = detectSkew( outerContour, approximate );
	int angleDeg = angle * 180 / M_PI;
	int sigma = 3;
	int range = 3;
	for (int i = angleDeg-sigma*range; i <= angleDeg+sigma*range; i++)
	{
		int j = i;
		if(j < 0) j += 180;
		if (j >= 180) j -= 180;

		histogram[j] = weight * histogram[j] + this->lastDetectionProbability/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i - angle)*(i - angle)/(2*sigma*sigma));
	}
}

//---------------------------------------------------------Bitangent------------------------------------------------------------------

LongestBitgEstimator::LongestBitgEstimator(int approximatioMethod, double epsilon, double ignoreAngle, double edgeRatio) :
				ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), edgeRatio(edgeRatio)
{

}

LongestBitgEstimator::~LongestBitgEstimator() {
	// TODO Auto-generated destructor stub
}

double LongestBitgEstimator::detectSkew( std::vector<cv::Point>& outerContour, cv::Mat* debugImage)
{
	probMeasure1 = 0;
	probMeasure2 = 0;

	double maxLength=0;
	double actAngle=0;
	double deltaX=0, deltaY=0;
	int ptIdxStart=-1;
	int ptIdxEnd=0;
	double range = ignoreAngle*M_PI/180;

	vector<double> actAngles;
	vector<double> actLenghts;

	double angle = 0;

	cv::vector<int> convexHull_IntIdx;
	cv::convexHull(outerContour, convexHull_IntIdx, true);

	std::vector<cv::Vec4i> convexityDefectsSet;
	cv::convexityDefects(outerContour, convexHull_IntIdx, convexityDefectsSet);
	double probability = 0;

	for(size_t i = 0; i <  convexityDefectsSet.size(); i++)
	{
		cv::Point start = outerContour[convexityDefectsSet[i][0]];
		cv::Point end = outerContour[convexityDefectsSet[i][1]];

		cv::Point vector =  end - start;
		double currentAngle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (currentAngle < 0) currentAngle += 180;
		if (currentAngle >= 180) currentAngle -= 180;
		assert(currentAngle >= 0 && currentAngle <= 180);
		if (! (currentAngle > ignoreAngle && currentAngle < (180-ignoreAngle)) )
		{
			continue;
		}
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);
		actAngles.push_back(currentAngle * M_PI/180);
		actLenghts.push_back(length);
		if(length > maxLength)
		{
			maxLength=length;
			angle = currentAngle * M_PI/180;
			ptIdxStart=convexityDefectsSet[i][0];
			ptIdxEnd = convexityDefectsSet[i][1];
		}
	}



	vector<double>actAnglesOrig = actAngles;
	vector<double>actLenghtsOrig = actLenghts;

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
	filterValuesBySimiliarAngle( actLenghtsOrig, actAnglesOrig, actLenghts2, actAngles2 );
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
		drawing =  Mat::zeros( bbox.height*scalefactor+brd, bbox.width*scalefactor+brd, CV_8UC3 ) + cv::Scalar(255, 255, 255);
		*debugImage = drawing;
		Scalar color = Scalar( 0, 0, 0 );
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
			cv::line(drawing, cv::Point((outerContour[j].x-min_X)*scalefactor + brd / 2,(outerContour[j].y-min_Y)*scalefactor + brd / 2),
			cv::Point((outerContour[index].x-min_X)*scalefactor + brd / 2,(outerContour[index].y-min_Y)*scalefactor + brd / 2), color, 1, 0);
		}

		if(ptIdxStart != -1)
		{
			cv::line(drawing,cv::Point((outerContour[ptIdxStart].x-min_X)*scalefactor + brd / 2,(outerContour[ptIdxStart].y-min_Y)*scalefactor + brd / 2),
			cv::Point((outerContour[ptIdxEnd].x-min_X)*scalefactor + brd / 2,(outerContour[ptIdxEnd].y-min_Y)*scalefactor + brd / 2), cv::Scalar( 0, 255, 0 ), 2, 0);
		}
	}


	int index = (int) (probMeasure2 - 1);
	index = MIN(index, this->probabilities.size() - 1);
	if(ptIdxStart != -1)
		lastDetectionProbability = 0.5;

	//y-souradnice je opacne
	return angle - M_PI/2;
}

void LongestBitgEstimator::voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, double weight, cv::Mat* debugImage)
{
	double angle = detectSkew( outerContour);
	int angleDeg = angle * 180 / M_PI;
	angleDeg += 90;
	int sigma = 3;
	int range = 3;
	for (int i = angleDeg-sigma*range; i <= angleDeg+sigma*range; i++)
	{
		int j = i;
		if(j < 0) j += 180;
		if (j >= 180) j -= 180;

		histogram[j] = weight * histogram[j] + this->lastDetectionProbability/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i - angleDeg)*(i - angleDeg)/(2*sigma*sigma));
	}
}

} /* namespace cmp */
