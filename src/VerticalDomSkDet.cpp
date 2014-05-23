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

#define VERBOSE 1

namespace cmp{


VerticalDomSkDet::VerticalDomSkDet(int approximatioMethod, double epsilon, int histColWidth, int sigma, int range, int ignoreAngle, int correctAngle): ContourSkewDetector(approximatioMethod, epsilon), histColWidth(histColWidth), sigma(sigma), range(range), ignoreAngle(ignoreAngle), correctAngle(correctAngle) {
	// TODO Auto-generated constructor stub

	hist = new double [int(180/histColWidth)];

	probabilities.push_back(0.25);
	probabilities.push_back(0.43);
	probabilities.push_back(0.57);
	probabilities.push_back(0.69);
	probabilities.push_back(0.81);
	probabilities.push_back(0.89);
	probabilities.push_back(0.95);

}

VerticalDomSkDet::~VerticalDomSkDet() {
	delete [] hist;
}

double VerticalDomSkDet::detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage)
{
	memset (hist, 0, int(180/histColWidth) * sizeof(double));

	cv::Point prev = contour.back();

	for(std::vector<cv::Point>::iterator it = contour.begin(); it < contour.end(); )
	{
		cv::Point pt = *it;
		cv::Point vector = pt - prev;
		double angle = atan2(double (vector.y), double (vector.x))*180/M_PI;
		if (angle < 0) angle += 180;
		if (angle >= 180) angle -= 180;
		double length = sqrt(vector.x*vector.x+vector.y*vector.y+0.0);

		int ang = angle/histColWidth;
		//hist[ang] = hist[ang] + length;
		//if (hist[ang] > hist[highestCol]) highestCol = ang;

		for (int i = ang-sigma*range; i <= ang+sigma*range; i++)
		{
			int j = i;
			if (j<0) j += int(180/histColWidth);
			if (j>=int(180/histColWidth)) j -= int(180/histColWidth);

			hist[j] = hist[j] + length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i*histColWidth+histColWidth/2-angle)*(i*histColWidth+histColWidth/2-angle)/(2*sigma*sigma));
		}
		prev = pt;
		it++;
	}

	int height = 300;

	cv::Mat histogram = Mat::zeros(height, 380, CV_8UC3);
	int maxI = ceil(double(ignoreAngle/histColWidth));

	double totalLen = 0.0;
	double resLen = 0.0;

	for(int i=0;i<int(180/histColWidth);i++)
	{
		int rectH = 20*hist[i];
		cv::rectangle(histogram, Rect(10+histColWidth*i*2, height-rectH-10, histColWidth*2, rectH), Scalar(0,0,255), CV_FILLED);
		if (i > ignoreAngle/histColWidth && i < (180-ignoreAngle)/histColWidth)
		{
			if (hist[i] > hist[maxI]) maxI = i;
			totalLen += hist[i];
		}
	}
	for (int i = maxI-sigma*range; i <= maxI+sigma*range; i++)
	{
		int j = i;
		if (j<0) j = j + int(180/histColWidth);
		if (j>=int(180/histColWidth)) j = j - int(180/histColWidth);

		resLen += hist[j];
	}
#ifdef VERBOSE
	cv::imshow("Histogram", histogram);
	cv::waitKey(0);
#endif

	double minValue = 0.0151;
	double maxValue = 0.1333;


	int index = ((resLen/totalLen) - minValue) / ( (maxValue - minValue ) / 10);
	index = MAX(0, index);
	index = MIN(0, probabilities.size());

	this->lastDetectionProbability = probabilities[index];
	this->probMeasure2 = this->lastDetectionProbability;

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		cv::Rect bbox = cv::boundingRect(contour);
		drawing =  Mat::zeros( bbox.height, bbox.width, CV_8UC3 );

		Scalar color = Scalar( 255, 255, 255 );
		std::vector<std::vector<cv::Point> > contours;
		contours.push_back(contour);
        
        int miny;
        int minx;
        //get contour max
        for (size_t i=0; i<contour.size(); i++) {
            miny = MIN(contour[i].y, miny);
            minx = MIN(contour[i].x, minx);
        }
        
		for (size_t i = 0; i < contour.size(); i++)
		{
			size_t i2 = (i==contour.size()-1) ? 0 : i+1;
            
			cv::circle(drawing, cv::Point(contour[i].x - minx,contour[i].y - miny), 2, Scalar( 0, 0, 255 ), 1);
            
            cv::line(drawing, cv::Point(contour[i].x - minx,contour[i].y - miny), cv::Point(contour[i2].x - minx,contour[i2].y- miny), color);
            
			double ang = atan2(double(contour[i2].y-contour[i].y), double(contour[i2].x-contour[i].x));
			if (ang < 0) ang = ang + M_PI;
			if (abs(ang-(maxI*histColWidth+histColWidth/2)*M_PI/180) < correctAngle*M_PI/180)
			{
				cv::line(drawing, cv::Point(contour[i].x - minx,contour[i].y- miny), Point(contour[i2].x - minx,contour[i2].y- miny), Scalar( 0, 255, 0 ), 1);
			}
		}
	}

	return (maxI*histColWidth+histColWidth/2)*M_PI/180-M_PI/2;
}

}//namespace cmp
