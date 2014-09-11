/*
 * VerticalDomSkDet.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#include "VerticalDomSkDet.h"
#include "SkewDetector.h"
//#define VERBOSE


namespace cmp{


VerticalDomSkDet::VerticalDomSkDet(int approximatioMethod, double epsilon, int sigma, int range, int ignoreAngle,
		int correctAngle, bool doConvexHull): ContourSkewDetector(approximatioMethod, epsilon), sigma(sigma), range(range), ignoreAngle(ignoreAngle), correctAngle(correctAngle), doConvexHull(doConvexHull) {
	// TODO Auto-generated constructor stub

	hist = new double[180];

	probabilities.push_back(0.25);
	probabilities.push_back(0.43);
	probabilities.push_back(0.57);
	probabilities.push_back(0.69);
	probabilities.push_back(0.81);
	probabilities.push_back(0.89);
	probabilities.push_back(0.95);

	borderForVis = 5;

}

VerticalDomSkDet::~VerticalDomSkDet() {
	delete [] hist;
}

double VerticalDomSkDet::detectSkew( std::vector<cv::Point>& contourOrig, cv::Mat* debugImage)
{
	memset (hist, 0, 180 * sizeof(double));

	std::vector<cv::Point>& contour = contourOrig;
	if(doConvexHull){
		std::vector<cv::Point> chull;
		cv::convexHull(contour, chull);
		contour = chull;
	}

	cv::Point prev = contour.back();
	double maxHistVal = 0;
	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		cv::Point pt = *it;
		cv::Point vector = pt - prev;
		double angle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (angle < 0) angle += 180;
		if (angle >= 180) angle -= 180;
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		int ang = angle;
		//ignoreAngle = 0;
		//hist[ang] = hist[ang] + length;
		//if (hist[ang] > hist[highestCol]) highestCol = ang;

		if (! (ang > ignoreAngle && ang < (180-ignoreAngle)) )
		{
			prev = pt;
			it++;
			continue;
		}

		for (int i = ang-sigma*range; i <= ang+sigma*range; i++)
		{
			int j = i;
			if(j < 0) j += 180;
			if (j >= 180) j -= 180;

			hist[j] = hist[j] + length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i - angle)*(i - angle)/(2*sigma*sigma));
			maxHistVal = MAX(maxHistVal, hist[j]);
		}
		prev = pt;
		it++;
	}

	int maxI = 0;
	double totalLen = 0.0;
	double resLen = 0.0;

	for(int i=0; i < 180; i++)
	{
		if (i > ignoreAngle && i < (180-ignoreAngle))
		{
			if (hist[i] > hist[maxI]) maxI = i;
			totalLen += hist[i];
		}
	}

	for (int i = maxI-sigma*range; i <= maxI+sigma*range; i++)
	{
		int j = i;
		if (j < 0) j = j + 180;
		if (j >= 180) j = j - 180;
		if (j > ignoreAngle && j < (180-ignoreAngle))
		{
			resLen += hist[j];
		}
	}
#ifdef VERBOSE
	cv::imshow("Histogram", histogram);
	cv::waitKey(0);
#endif

	double minValue = 0.14; //TODO what is the value
	double maxValue = 0.1333;


	int index = ((resLen/totalLen) - minValue) / ( (maxValue - minValue ) / 10);
	index = MAX(0, index);
	index = MIN(index, probabilities.size()-1);
	assert(index<probabilities.size());

	this->lastDetectionProbability = (resLen/totalLen);
	assert(lastDetectionProbability == lastDetectionProbability);
	this->probMeasure1 = probabilities[index] * 100;
	this->probMeasure2 = (resLen/totalLen);

	if(debugImage != NULL)
	{
		int height = 100;
		double norm = height / maxHistVal;
		cv::Mat histogram = Mat::zeros(height, 180, CV_8UC3) + cv::Scalar(255, 255, 255);
		cv::rectangle(histogram, Rect(0, 0, 1, height), Scalar(0,0,0), CV_FILLED);
		cv::rectangle(histogram, Rect(45, 0, 1, height), Scalar(100,100,100), CV_FILLED);
		cv::rectangle(histogram, Rect(90, 0, 1, height), Scalar(0,0,0), CV_FILLED);
		cv::rectangle(histogram, Rect(135, 0, 1, height), Scalar(100,100,100), CV_FILLED);

		for(int i=0;i<int(180);i++)
		{
			int rectH = norm * hist[i];
			cv::rectangle(histogram, Rect(i, height-rectH, 1, rectH), Scalar(0,0,255), CV_FILLED);
		}


		Mat& drawing =  *debugImage;
		cv::Rect bbox = cv::boundingRect(contour);
		drawing =  Mat::zeros( bbox.height*scalefactor+borderForVis*2, bbox.width*scalefactor+borderForVis*2, CV_8UC3 ) + cv::Scalar(255, 255, 255);

		Scalar color = Scalar( 0, 0, 0 );
		std::vector<std::vector<cv::Point> > contours;
		contours.push_back(contour);

		int miny = INT_MAX;
		int minx = INT_MAX;
		//get contour max
		for (size_t i=0; i<contour.size(); i++) {
			miny = MIN(contour[i].y, miny);
			minx = MIN(contour[i].x, minx);//  borderForVis / 2;
		}

		minx -= 2;
		miny -= 2;

		for (size_t i = 0; i < contour.size(); i++)
		{
			size_t i2 = (i==contour.size()-1) ? 0 : i+1;

			cv::circle(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y - miny)*scalefactor), 2, Scalar( 255, 0, 0 ), 2);

			cv::line(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y - miny)*scalefactor), cv::Point((contour[i2].x - minx)*scalefactor,(contour[i2].y- miny)*scalefactor), color);

			double ang = atan2(double(contour[i2].y-contour[i].y), double(contour[i2].x-contour[i].x));
			if (ang < 0) ang = ang + M_PI;
			if (ang >= M_PI) ang -= M_PI;

			if (fabs(ang - maxI * M_PI / 180.0) < correctAngle*M_PI/180)
			{
				cv::line(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y- miny)*scalefactor), Point((contour[i2].x - minx)*scalefactor,(contour[i2].y- miny)*scalefactor), Scalar( 29, 207, 34 ), 2);
			}
		}
		std::vector<cv::Mat> imagesToMerge;
		imagesToMerge.push_back(drawing);
		imagesToMerge.push_back(histogram);
		*debugImage = mergeHorizontal(imagesToMerge, 1, 0, NULL, cv::Scalar(255, 255, 255) );
		/*
		cv::imshow("img", *debugImage);
		cv::waitKey(0);
		cv::imwrite("/tmp/verticalDominant.png", *debugImage);
		*/
	}
	return maxI*M_PI/180-M_PI/2;
}

void VerticalDomSkDet::voteInHistogram( std::vector<cv::Point>& contourOrig, double *histogram, double weight, cv::Mat* debugImage)
{
	memset (hist, 0, 180 * sizeof(double));
	std::vector<cv::Point>& contour = contourOrig;
	if(doConvexHull)
	{
		std::vector<cv::Point> chull;
		cv::convexHull(contour, chull);
		contour = chull;
	}

	cv::Point prev = contour.back();
	double maxHistVal = 0;
	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		cv::Point pt = *it;
		cv::Point vector = pt - prev;
		double angle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (angle < 0) angle += 180;
		if (angle >= 180) angle -= 180;
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		int ang = angle;
		//hist[ang] = hist[ang] + length;
		//if (hist[ang] > hist[highestCol]) highestCol = ang;

		if (! (ang > ignoreAngle && ang < (180-ignoreAngle)) )
		{
			prev = pt;
			it++;
			continue;
		}

		for (int i = ang-sigma*range; i <= ang+sigma*range; i++)
		{
			int j = i;
			if(j < 0) j += 180;
			if (j >= 180) j -= 180;

			hist[j] = hist[j] + length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i - angle)*(i - angle)/(2*sigma*sigma));
			maxHistVal = MAX(maxHistVal, hist[j]);
		}
		prev = pt;
		it++;
	}

	int maxI = 0;
	double totalLen = 0.0;
	double resLen = 0.0;

	for(int i=0; i < 180; i++)
	{
		if (i > ignoreAngle && i < (180-ignoreAngle))
		{
			if (hist[i] > hist[maxI]) maxI = i;
			totalLen += hist[i];
		}
	}

	for (int i = maxI-sigma*range; i <= maxI+sigma*range; i++)
	{
		int j = i;
		if (j < 0) j = j + 180;
		if (j >= 180) j = j - 180;
		if (j > ignoreAngle && j < (180-ignoreAngle))
		{
			resLen += hist[j];
		}
	}
#ifdef VERBOSE
	cv::imshow("Histogram", histogram);
	cv::waitKey(0);
#endif

	double minValue = 0.14; //TODO what is the value
	double maxValue = 0.1333;


	int index = ((resLen/totalLen) - minValue) / ( (maxValue - minValue ) / 10);
	index = MAX(0, index);
	index = MIN(index, probabilities.size()-1);
	assert(index<probabilities.size());

	this->lastDetectionProbability = (resLen/totalLen);
	assert(lastDetectionProbability == lastDetectionProbability);
	this->probMeasure1 = probabilities[index] * 100;
	this->probMeasure2 = (resLen/totalLen);

	if(debugImage != NULL)
	{
		int height = 100;
		double norm = height / maxHistVal;
		cv::Mat histogram = Mat::zeros(height, 180, CV_8UC3);
		cv::rectangle(histogram, Rect(45, 0, 1, height), Scalar(100,100,100), CV_FILLED);
		cv::rectangle(histogram, Rect(90, 0, 1, height), Scalar(255,255,255), CV_FILLED);
		cv::rectangle(histogram, Rect(135, 0, 1, height), Scalar(100,100,100), CV_FILLED);

		for(int i=0;i<int(180);i++)
		{
			int rectH = norm * hist[i];
			cv::rectangle(histogram, Rect(i, height-rectH, 1, rectH), Scalar(0,0,255), CV_FILLED);
		}


		Mat& drawing =  *debugImage;
		cv::Rect bbox = cv::boundingRect(contour);
		drawing =  Mat::zeros( bbox.height*scalefactor+borderForVis, bbox.width*scalefactor+borderForVis, CV_8UC3 );

		Scalar color = Scalar( 255, 255, 255 );
		std::vector<std::vector<cv::Point> > contours;
		contours.push_back(contour);

		int miny = INT_MAX;
		int minx = INT_MAX;
		//get contour max
		for (size_t i=0; i<contour.size(); i++) {
			miny = MIN(contour[i].y, miny);
			minx = MIN(contour[i].x, minx);
		}

		for (size_t i = 0; i < contour.size(); i++)
		{
			size_t i2 = (i==contour.size()-1) ? 0 : i+1;

			cv::circle(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y - miny)*scalefactor), 2, Scalar( 0, 0, 255 ), 1);

			cv::line(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y - miny)*scalefactor), cv::Point((contour[i2].x - minx)*scalefactor,(contour[i2].y- miny)*scalefactor), color);

			double ang = atan2(double(contour[i2].y-contour[i].y), double(contour[i2].x-contour[i].x));
			if (ang < 0) ang = ang + M_PI;
			if (fabs(ang) < correctAngle*M_PI/180)
			{
				cv::line(drawing, cv::Point((contour[i].x - minx)*scalefactor,(contour[i].y- miny)*scalefactor), Point((contour[i2].x - minx)*scalefactor,(contour[i2].y- miny)*scalefactor), Scalar( 0, 255, 0 ), 1);
			}
		}
		std::vector<cv::Mat> imagesToMerge;
		imagesToMerge.push_back(drawing);
		imagesToMerge.push_back(histogram);
		*debugImage = mergeHorizontal(imagesToMerge, 1, 0, NULL );
		//cv::imshow("ts", *debugImage);
		//cv::waitKey(0);
	}

	for(int i=0; i < 180; i++)
	{
		histogram[i] += weight * hist[i] / maxHistVal * this->lastDetectionProbability;
	}
}

}//namespace cmp
