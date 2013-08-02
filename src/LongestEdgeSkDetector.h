/*
 * LongestEdgeSkDetector.h
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#ifndef LONGESTEDGESKDETECTOR_H_
#define LONGESTEDGESKDETECTOR_H_

#include "SkewDetector.h"

namespace cmp {

class LongestEdgeSkDetector : public ContourSkewDetector {
public:
	LongestEdgeSkDetector(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.028, double ignoreAngle = 15, double edgeRatio = 0.20);
	virtual ~LongestEdgeSkDetector();

	virtual double detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage = NULL );

	/* ve stupnich */
	double ignoreAngle;
	/* udava % rozsah pro odhad pravdepodobnosti (kolik dalsich hran je s delkou longestEdge +- edgeRatio * longestEdge ) */
	double edgeRatio;
	/* udava pocet hran v definovanem rozmezi - longestEdge +- edgeRatio* longestEdge */
	int noOfEdgesInRange;
	/* soucet delek hran v definovanem rozmezi - longestEdge +- edgeRatio* longestEdge */
	double edgesLengthInRange;
};

} /* namespace cmp */
#endif /* LONGESTEDGESKDETECTOR_H_ */
