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
	VerticalDomSkDet(int histColWidth = 1, int sigma = 2, int range = 10, int ignoreAngle = 10);
	virtual ~VerticalDomSkDet();

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );

	float* hist;
	/** sirka binu histogramu ve stupnich */
	int histColWidth;
	/** parametr sigma u normalového rozdeleni */
	int sigma;
	/** pocet ovlivnenych binu = sigma*range */
	int range;
	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
};

}//namespace cmp

#endif /* VERTICALDOMSKDET_H_ */
