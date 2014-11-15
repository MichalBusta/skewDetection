/*
* BestGuessSKDetector.cpp
*
*  Created on: Jul 26, 2013
*      Author: Michal Busta
*/

#include "BestGuessSKDetector.h"
#include "SkewDetection.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>

namespace cmp
{

BestGuessSKDetector::BestGuessSKDetector(int approximatioMethod, double epsilon) : ContourSkewDetector(approximatioMethod, epsilon)
{
	detectors.push_back( new VerticalDomSkDet(CV_CHAIN_APPROX_NONE, 0.022, 3, 3, IGNORE_ANGLE, 3, false, true));
	weights.push_back(1.0);
    detectorNames.push_back("VertDom");


    detectors.push_back( new VerticalDomSkDet(CV_CHAIN_APPROX_NONE, 0.022, 3, 3, IGNORE_ANGLE, 3, true, true));
    weights.push_back(1.0);
    detectorNames.push_back("VertDomCH");


    detectors.push_back( new CentersSkDet(CV_CHAIN_APPROX_NONE, 0, 0.08, true, false ,0.9 ) );
    weights.push_back(1.0);
    detectorNames.push_back("CentersSkDet");


	detectors.push_back( new ThinProfileSkDet(CV_CHAIN_APPROX_NONE, 0.023, IGNORE_ANGLE, 0.02, true) );
	weights.push_back(1.0);
    detectorNames.push_back("ThinProfileSkDet");
    /*
	detectors.push_back( new LongestEdgeSkDetector() );
    weights.push_back(0.5);
    detectorNames.push_back("LongestEdgeSkDetector");
    
    detectors.push_back( new CentersSkDet() );
    weights.push_back(1.0);
    detectorNames.push_back("CentersSkDet");
    */

}

BestGuessSKDetector::BestGuessSKDetector(std::vector<cv::Ptr<ContourSkewDetector> >& detectors,
			std::vector<double>& weights, std::vector<std::string>& detectorNames,
			int approximatioMethod, double epsilon) : ContourSkewDetector(approximatioMethod, epsilon),
					detectors(detectors), weights(weights), detectorNames(detectorNames)
{

}

BestGuessSKDetector::~BestGuessSKDetector()
{
	// TODO Auto-generated destructor stub
}

double BestGuessSKDetector::detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage )
{
    assert(this->detectors.size()==this->weights.size());
	double bestProb = 0;
	std::vector<double> angles;
	cv::Mat bestDebugImage;
	size_t bestDetIndex = -1;
	for(size_t i = 0; i < this->detectors.size(); i++)
	{

		cv::Mat dbgImage;
		cv::Mat img = mask.clone();
		angles.push_back( this->detectors[i]->detectSkew( img, lineK, &dbgImage) );
        debugImages[detectorNames[i]] = dbgImage;
		if(bestProb < (this->detectors[i]->lastDetectionProbability * weights[i] ) )
		{
			bestDetIndex = i;
			bestDebugImage = dbgImage;
			bestProb = this->detectors[i]->lastDetectionProbability * weights[i];
		}
	}
	this->lastDetectionProbability = bestProb;
	if(debugImage != NULL)
		*debugImage = bestDebugImage;

#ifdef VERBOSE
	std::cout << "BestGuess angle is: " << angles[bestDetIndex] << " with prob: " << lastDetectionProbability << std::endl;
#endif
	return angles[bestDetIndex];

}

double BestGuessSKDetector::detectSkew( std::vector<cv::Point>& outerContour, bool approximate, cv::Mat* debugImage )
{
	double bestProb = 0;
	std::vector<double> angles;
	size_t bestDetIndex = -1;
	cv::Mat img;
	if(this->epsilon > 0)
	{
		cv::Rect rect= cv::boundingRect(outerContour);
		int size = rect.height;
		double absEpsilon = epsilon * size;
		std::vector<cv::Point> apCont;
		approxPolyDP(outerContour, apCont, absEpsilon, true);
		outerContour = apCont;
	}
	/*
	double histogram[180];
	memset (histogram, 0, 180 * sizeof(double));
	for(size_t i = 0; i < this->detectors.size(); i++)
	{
		this->detectors[i]->voteInHistogram(outerContour, histogram, this->weights[i], debugImage);
	}

	int ignoreAngle = 30;
	int maxI = 0;
	double totalLen = 0;
	double maxVal = 0;
	for(int i=0; i < 180; i++)
	{
		if (i > ignoreAngle && i < (180-ignoreAngle))
		{
			if (histogram[i] > histogram[maxI]) maxI = i;
			totalLen += histogram[i];
			maxVal = MAX(maxVal, histogram[i]);
		}
	}
	*/
	for(size_t i = 0; i < this->detectors.size(); i++)
	{


		angles.push_back( this->detectors[i]->detectSkew( outerContour, debugImage) );
		if(bestProb < (this->detectors[i]->lastDetectionProbability * weights[i] ) )
		{
			bestDetIndex = i;
			bestProb = this->detectors[i]->lastDetectionProbability * weights[i];
		}
	}
	this->lastDetectionProbability = bestProb;

#ifdef VERBOSE
	std::cout << "BestGuess angle is: " << angles[bestDetIndex] << " with prob: " << lastDetectionProbability << std::endl;
#endif
	return angles[bestDetIndex];
	/*
	double angle = maxI*M_PI/180-M_PI/2;
	return angle;
	*/
}

void BestGuessSKDetector::getSkewAngles( std::vector<cv::Point>& outerContour, std::vector<double>& angles, std::vector<double>& probabilities, std::vector<int>& detecotrsId, cv::Mat* debugImage)
{
	double bestProb = 0;
	cv::Mat img;
	if(this->epsilon > 0)
	{
		cv::Rect rect= cv::boundingRect(outerContour);
		int size = rect.height;
		double absEpsilon = epsilon * size;
		std::vector<cv::Point> apCont;
		approxPolyDP(outerContour, apCont, absEpsilon, true);
		outerContour = apCont;
	}

	for(size_t i = 0; i < this->detectors.size(); i++)
	{
		angles.push_back( this->detectors[i]->detectSkew( outerContour, debugImage) );
		probabilities.push_back(this->detectors[i]->lastDetectionProbability * weights[i]);
		assert( detectors[i]->lastDetectionProbability == detectors[i]->lastDetectionProbability);
		detecotrsId.push_back(i);
	}
}

void BestGuessSKDetector::voteInHistogram( std::vector<cv::Point>& contourOrig, double *histogram, double weight, bool approximate, cv::Mat* debugImage)
{
	std::vector<cv::Point>& outerContour = contourOrig;
	if(this->epsilon > 0)
	{
		cv::Rect rect= cv::boundingRect(outerContour);
		int size = rect.height;
		double absEpsilon = epsilon * size;
		std::vector<cv::Point> apCont;
		approxPolyDP(outerContour, apCont, absEpsilon, true);
		outerContour = apCont;
	}
	for(size_t i = 0; i < this->detectors.size(); i++)
	{
		this->detectors[i]->voteInHistogram(outerContour, histogram, this->weights[i], debugImage);
	}
}

} /* namespace cmp */
