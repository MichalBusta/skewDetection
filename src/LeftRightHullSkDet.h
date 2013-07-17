/*
 * leftRightHullSkDet.h
 *
 *  Created on: Jul 11, 2013
 *      Author: Jakub Cidlik
 */

#ifndef LEFTRIGHTHULLSKDET_H_
#define LEFTRIGHTHULLSKDET_H_

#include "SkewDetector.h"

namespace cmp
{

/**
 * @class cmp::leftRightHullSkDet
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class LeftRightHullSkDet : public SkewDetector
{
public:
	LeftRightHullSkDet(float precision = 0.1, bool isRight = true, bool contourForm=true);

	virtual ~LeftRightHullSkDet();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );

	//precision urcuje cast pismene v desetinem tvaru, ktere tvori "horni hranici pismene"
	float precision;

	bool isRight;

	bool contourForm;
};

} /* namespace cmp */
#endif /* LEFTRIGHTHULLSKDET_H_ */
