//
//  DiscreteVotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "DiscreteVotingWordSkDet.h"
#include "WordSkewDetector.h"
#include "SkewDetection.h"

//#define VERBOSE

namespace cmp
{

DiscreteVotingWordSkDet::DiscreteVotingWordSkDet() : ContourWordSkewDetector()
{

	detectors.push_back( new VerticalDomSkDet(CV_CHAIN_APPROX_NONE, 0.022));
	weights.push_back(1.0);
	detectorNames.push_back("VertDom");


	detectors.push_back( new VerticalDomSkDet(CV_CHAIN_APPROX_NONE, 0.022, 3, 3, IGNORE_ANGLE, 3, true));
	weights.push_back(0.9);
	detectorNames.push_back("VertDomCH");


	detectors.push_back( new CentersSkDet(CV_CHAIN_APPROX_NONE, 0, 0.08, true, 0.9) );
	weights.push_back(1.0);
	detectorNames.push_back("CentersSkDet");


	detectors.push_back( new ThinProfileSkDet(CV_CHAIN_APPROX_NONE, 0.023, IGNORE_ANGLE, 0.02, true) );
	weights.push_back(1.0);
	detectorNames.push_back("Thin Profile");

	detectors.push_back( new LongestEdgeSkDetector() );
	weights.push_back(1.0);
	detectorNames.push_back("LongestEdgeSkDetector");


}
DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
{

}

double DiscreteVotingWordSkDet::detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage)
{
	double histogram[180];
	memset (histogram, 0, 180 * sizeof(double));

	double epsilon = 0.014;

	for (size_t i = 0; i < contours.size(); i++)
	{
		for( size_t detId = 0; detId < this->detectors.size(); detId++)
		{

			/*
			double skewAngle = this->detectors[detId]->detectSkew( *contours[i], true,  debugImage);

			cv::imshow("det", *debugImage);
			cv::waitKey(0);

			double angleDeg = skewAngle * 180 / M_PI + 90;
			assert(angleDeg >= 0);
			assert(angleDeg <= 180);
			histogram[(int) round(angleDeg)] += detectors[detId]->lastDetectionProbability * weights[detId];
*/

			this->detectors[detId]->voteInHistogram(*contours[i], lineK, histogram, this->weights[detId], true, debugImage);

		}
	}

	double histogramSmooth[180];
	memset (histogramSmooth, 0, 180 * sizeof(double));
	double delta = 3;
	int range = (int) (delta * 3);
	for(int k=0; k < 180; k++)
	{
		for (int i = k-range; i <= k+range; i++)
		{
			int j = i;
			if(j < 0) j += 180;
			if (j >= 180) j -= 180;
			histogramSmooth[k] += histogram[j]/(delta*sqrt(2*M_PI))*pow(M_E, -((i - k)*(i - k))/(2*delta*delta));
		}
	}

	int ignoreAngle = 30;
	int maxI = 0;
	double totalLen = 0;
	double maxVal = 0;
	for(int i=0; i < 180; i++)
	{
		if (i > ignoreAngle && i < (180-ignoreAngle))
		{
			if (histogramSmooth[i] > histogramSmooth[maxI]) maxI = i;
			totalLen += histogramSmooth[i];
			maxVal = MAX(maxVal, histogramSmooth[i]);
		}
	}

	int sigma = 3;
	range = 3;
	double resLen = 0;
	for (int i = maxI-sigma*range; i <= maxI+sigma*range; i++)
	{
		int j = i;
		if (j < 0) j = j + 180;
		if (j >= 180) j = j - 180;
		if (j > ignoreAngle && j < (180-ignoreAngle))
		{
			resLen += histogramSmooth[j];
		}
	}

	double angle = maxI*M_PI/180-M_PI/2;
	probability =  (resLen/totalLen);
	//draw the histogram
	if(debugImage != NULL)
	{
		int noOfGroups = 180;
		std::vector<cv::Scalar> colors;
		colors.push_back(cv::Scalar(255, 0, 0));
		colors.push_back(cv::Scalar(0, 255, 0));
		colors.push_back(cv::Scalar(0, 0, 255));
		colors.push_back(cv::Scalar(0, 255, 255));
		colors.push_back(cv::Scalar(255, 0, 255));
		cv::Mat histogramImg;
		int histWidth = 180;
		int histHeight = 100;
		int colWidth = histWidth / noOfGroups;

		histogramImg = cv::Mat::zeros(histHeight, histWidth, CV_8UC3) + cv::Scalar(255, 255, 255);
		cv::line(histogramImg, cv::Point(0, 0), cv::Point(0, histogramImg.rows), cv::Scalar(0, 0, 0) );
		cv::line(histogramImg, cv::Point(90, 0), cv::Point(90, histogramImg.rows), cv::Scalar(0, 0, 0) );
		cv::line(histogramImg, cv::Point(90 + 45, 0), cv::Point(90 + 45, histogramImg.rows), cv::Scalar(100, 100, 100) );
		cv::line(histogramImg, cv::Point(45, 0), cv::Point(45, histogramImg.rows), cv::Scalar(100, 100, 100) );
		if(maxVal < 1)
			maxVal = 1;
		double norm = histHeight / maxVal;
		int graphWidth =noOfGroups*colWidth;
		for (int i =0; i <noOfGroups; i++) {
			int colHeight = histogramSmooth[i] * norm;

			cv::rectangle(histogramImg, cv::Point(i*colWidth, histHeight), cv::Point(colWidth*i+colWidth, histHeight-colHeight), cv::Scalar(255,0,0),CV_FILLED);
		}
		cv::line(histogramImg, cv::Point(0, histogramImg.rows - 1), cv::Point(180, histogramImg.rows - 1), cv::Scalar(100, 100, 100) );
		*debugImage = histogramImg;

#ifdef VERBOSE
		cv::imshow("Histogram", histogram);
		cv::waitKey(0);
#endif
	}
	return angle;
}

}//namespace cmp
