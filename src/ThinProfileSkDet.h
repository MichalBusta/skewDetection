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
class ThinProfileSkDet : public ContourSkewDetector
{
public:
	ThinProfileSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.1, int ignoreAngle = 10);
	virtual ~ThinProfileSkDet();

	virtual double detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage = NULL );

	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
};

} /* namespace cmp */
#endif /* THINPROFILESKDET_H_ */
