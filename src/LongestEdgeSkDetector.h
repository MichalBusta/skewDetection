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
    
    int brd =80;

	LongestEdgeSkDetector(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.028, double ignoreAngle = IGNORE_ANGLE, double edgeRatio = 0.2);

	virtual ~LongestEdgeSkDetector();

	virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

	/* ve stupnich */
	double ignoreAngle;
	/* udava % rozsah pro odhad pravdepodobnosti (kolik dalsich hran je s delkou longestEdge +- edgeRatio * longestEdge ) */
	double edgeRatio;

	std::vector<double> probabilities;
};

} /* namespace cmp */
#endif /* LONGESTEDGESKDETECTOR_H_ */
