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
class LeftRightHullSkDet : public ContourSkewDetector
{
public:
	LeftRightHullSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.01, float precision =0.2, bool isRight = false, int ignoreAngle = IGNORE_ANGLE);

	virtual ~LeftRightHullSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

	//precision urcuje cast pismene v desetinem tvaru, ktere tvori "horni hranici pismene"
	float precision;

	bool isRight;

	/* ve stupnich */
	int ignoreAngle;
};

} /* namespace cmp */
#endif /* LEFTRIGHTHULLSKDET_H_ */
