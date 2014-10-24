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
    CentersSkDet(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0, float precision = 0.08, bool recursive = true,bool useMaxMin = false, double zoneOffset = 1.0);

	virtual ~CentersSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contour, bool approximate = false, cv::Mat* debugImage = NULL );

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, double weight, bool approximate = false, cv::Mat* debugImage = NULL);

	//precision urcuje cast pismene v desetinem tvaru, ktere tvori "horni hranici pismene"
	float precision;

	bool recursive;
    
    bool useMaxMin;
	//zone offset urcuje zacatek pasu horni(dolni hranice) topPoint.y' = topPoint.y * zoneOffset
	double zoneOffset;

private:
	double doEstimate( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );
    
    double doEstimate2( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );
};

} /* namespace cmp */
#endif /* CENTERSSKDET_H_ */
