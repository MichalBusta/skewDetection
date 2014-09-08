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
class CentersSkDet : public ContourSkewDetector
{
public:
	CentersSkDet(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0.01, float precision = 0.08);

	virtual ~CentersSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, cv::Mat* debugImage);

	//precision urcuje cast pismene v desetinem tvaru, ktere tvori "horni hranici pismene"
	float precision;
};

} /* namespace cmp */
#endif /* CENTERSSKDET_H_ */
