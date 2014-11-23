//
//  VotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 21/03/14.
//
//

#include "VotingWordSkDet.h"
#include "WordSkewDetector.h"
#include "stdlib.h"

#include "SkewDetection.h"

namespace cmp
{

VotingWordSkDet::VotingWordSkDet(int approximatioMethod, double epsilon) : ContourSkewDetector(approximatioMethod, epsilon)
{
	detectors.push_back( new VerticalDomSkDet());
	weights.push_back(1.0);
	//detectors.push_back( new ThinProfileSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.1));
	//weights.push_back(1.0);
	detectors.push_back( new LongestEdgeSkDetector(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.4));
	weights.push_back(1.0);
	//detectors.push_back( new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.014, IGNORE_ANGLE, true) );
	//weights.push_back(0.5);
}

VotingWordSkDet::~VotingWordSkDet()
{
	// TODO Auto-generated destructor stub
}

double VotingWordSkDet::detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage )
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

#ifdef VERBOSE
	std::cout << "BestGuess angle is: " << angles[bestDetIndex] << " with prob: " << lastDetectionProbability << std::endl;
#endif
	return angles[bestDetIndex];

}

double VotingWordSkDet::detectSkew( std::vector<cv::Point>& outerContour, double lineK, cv::Mat* debugImage )
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

	for(size_t i = 0; i < this->detectors.size(); i++)
	{
		angles.push_back( this->detectors[i]->detectSkew( outerContour, lineK, debugImage) );
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
}

void VotingWordSkDet::getSkewAngles( std::vector<cv::Point>& outerContour, double lineK, std::vector<double>& angles, std::vector<double>& probabilities, cv::Mat* debugImage)
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
		angles.push_back( this->detectors[i]->detectSkew( outerContour, lineK, debugImage) );
		probabilities.push_back(this->detectors[i]->lastDetectionProbability * weights[i]);
	}
}

}//namespace cmp
