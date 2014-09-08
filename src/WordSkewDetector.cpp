//
//  WordSkewDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#include "WordSkewDetector.h"
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


namespace cmp {

WordSkewDetector::WordSkewDetector()
{

}
WordSkewDetector::~WordSkewDetector(){

}

ContourWordSkewDetector::ContourWordSkewDetector(cv::Ptr<ContourSkewDetector> detector) : WordSkewDetector()
{
	this->localDetector = detector;
}

ContourWordSkewDetector::~ContourWordSkewDetector()
{

}



double ContourWordSkewDetector::detectSkew(std::vector<Blob1>& blobs, double lineK, cv::Mat* debugImage)
{

	std::vector<double> probs;
	std::vector<double> angles;
	std::vector<int> detectorsIndex;
	int noImg = blobs.size();
	for (int i = 0; i<noImg; i++)
	{
		angles.push_back(localDetector->detectSkew(blobs[i].mask, lineK, debugImage));
		detectorsIndex.push_back(0);
#ifdef VERBOSE

cv::imshow("temp", *tempDebugPtr);
cv::waitKey(0);
#endif
probs.push_back(localDetector->lastDetectionProbability);
	}
	double probability = 0;
	return computeAngle(angles, probs, detectorsIndex, probability, debugImage);
}

double ContourWordSkewDetector::detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage)
{
	std::vector<double> probs;
	std::vector<double> angles;
	std::vector<int> detectorsIndex;
	for (size_t i = 0; i < contours.size(); i++)
	{
#ifdef VERBOSE
		cv::Mat tempDebug;
		debugImage = &tempDebug;
#endif
		localDetector->getSkewAngles(*contours[i], angles, probs, detectorsIndex, debugImage);
#ifdef VERBOSE
		cv::imshow("temp", tempDebug);
		cv::waitKey(0);
#endif
	}
	double angle = computeAngle(angles, probs, detectorsIndex, probability, debugImage);
#ifdef VERBOSE
	std::cout << "Detected skew angle is: " << angle << " with prob.: " << probability << std::endl;
#endif
	return angle;
}
}
