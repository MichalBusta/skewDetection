/*
 * ThinProfileSkDet.h
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

#ifndef THINPROFILESKDET_H_
#define THINPROFILESKDET_H_

#include "SkewDetector.h"

namespace cmp
{

/**
 * @class cmp::ThinProfileSkDet
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class ThinProfileSkDet : public SkewDetector
{
public:
	ThinProfileSkDet();
	virtual ~ThinProfileSkDet();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );
};

} /* namespace cmp */
#endif /* THINPROFILESKDET_H_ */
