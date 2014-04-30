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

	class VerticalDomSkDet : public ContourSkewDetector {
	public:
		VerticalDomSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.022, int histColWidth = 1, int sigma = 3, int range = 3, int ignoreAngle = IGNORE_ANGLE, int correctAngle = 3);
		virtual ~VerticalDomSkDet();

		virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL);

		double* hist;
		/** sirka binu histogramu ve stupnich */
		int histColWidth;
		/** parametr sigma u normaloveho rozdeleni */
		int sigma;
		/** pocet ovlivnenych binu = sigma*range */
		int range;
		/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
		int ignoreAngle;
		/** zobrazi vizualizaci, pokud je uhel v toleranci; ve stupnich */
		int correctAngle;

		std::vector<double> probabilities;
	};

}
//namespace cmp

#endif /* VERTICALDOMSKDET_H_ */
