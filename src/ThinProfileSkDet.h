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
	/**
	 *
	 * @param approximatioMethod
	 * @param epsilon
	 * @param ignoreAngle in degrees
	 */
	ThinProfileSkDet(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0.018, int ignoreAngle = IGNORE_ANGLE, double profilesRange = 0.02);

	virtual ~ThinProfileSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contours, cv::Mat* debugImage = NULL );


	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
	//% of profile width 
	double profilesRange;
    
	std::vector<double> probabilities;


};

} /* namespace cmp */
#endif /* THINPROFILESKDET_H_ */
