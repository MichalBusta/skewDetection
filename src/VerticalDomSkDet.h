/*
 * VerticalDomSkDet.h
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#ifndef VERTICALDOMSKDET_H_
#define VERTICALDOMSKDET_H_

#include "SkewDetector.h"

namespace cmp{

class VerticalDomSkDet : public SkewDetector {
public:
	VerticalDomSkDet();
	virtual ~VerticalDomSkDet();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );
};

}//namespace cmp

#endif /* VERTICALDOMSKDET_H_ */
