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

class LongestEdgeSkDetector : public SkewDetector {
public:
	LongestEdgeSkDetector();
	virtual ~LongestEdgeSkDetector();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );
};

} /* namespace cmp */
#endif /* LONGESTEDGESKDETECTOR_H_ */
