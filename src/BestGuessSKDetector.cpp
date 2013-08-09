/*
 * BestGuessSKDetector.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: Michal Busta
 */

#include "BestGuessSKDetector.h"
#include "SkewDetection.h"

namespace cmp
{

BestGuessSKDetector::BestGuessSKDetector()
{
	detectors.push_back( new VerticalDomSkDet());
	weights.push_back(1.0);
	//detectors.push_back( new ThinProfileSkDet());
	//weights.push_back(1.0);
	detectors.push_back( new LongestEdgeSkDetector(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.4));
	weights.push_back(1.0);
	//detectors.push_back( new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.014, IGNORE_ANGLE, true) );
	//weights.push_back(0.5);
}

BestGuessSKDetector::~BestGuessSKDetector()
{
	// TODO Auto-generated destructor stub
}

double BestGuessSKDetector::detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage )
{
	double bestProb = 0;
	std::vector<double> angles;
	cv::Mat bestDebugImage;
	size_t bestDetIndex = -1;
	for(size_t i = 0; i < this->detectors.size(); i++)
	{
		cv::Mat dbgImage;
		cv::Mat img = mask.clone();
		angles.push_back( this->detectors[i]->detectSkew( img, lineK, &dbgImage) );
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

	return angles[bestDetIndex];

}

} /* namespace cmp */
