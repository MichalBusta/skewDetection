/*
 * CentersSkDet.h
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

#ifndef CENTERSSKDET_H_
#define CENTERSSKDET_H_

#include "SkewDetector.h"

namespace cmp
{

/**
 * @class cmp::CentersSkDet
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class CentersSkDet : public SkewDetector
{
public:
	CentersSkDet();
	virtual ~CentersSkDet();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );
};

} /* namespace cmp */
#endif /* CENTERSSKDET_H_ */
