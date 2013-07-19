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
	LongestEdgeSkDetector(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.01, int ignoreAngle = 2);
	virtual ~LongestEdgeSkDetector();

	virtual double detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage = NULL );

	/* ve stupnich */
	int ignoreAngle;
};

} /* namespace cmp */
#endif /* LONGESTEDGESKDETECTOR_H_ */
