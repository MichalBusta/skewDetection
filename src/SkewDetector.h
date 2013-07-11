/*
 * SkewDetector.h
 *
 *  Created on: Jul 9, 2013
 *      Author: Michal Busta
 */

#ifndef SKEWDETECTOR_H_
#define SKEWDETECTOR_H_

#include <opencv2/core/core.hpp>

namespace cmp
{

/**
 * @class cmp::SkewDetector
 * 
 * @brief The skew detector interface
 *
 * TODO type description
 */
class SkewDetector
{
public:
	SkewDetector();
	virtual ~SkewDetector();

	/**
	 *
	 * @param mask the character mask - ink is white
	 * @param lineK
	 * @param debugImage
	 * @return the skew angle in radians
	 */
	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL ) = 0;

};

} /* namespace cmp */
#endif /* SKEWDETECTOR_H_ */
