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
	VerticalDomSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.014, int sigma = 3, int range = 3, int ignoreAngle = IGNORE_ANGLE, int correctAngle = 3, bool doConvexHull = false, bool normalizeLength = true);
	virtual ~VerticalDomSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contour, double lineK, bool approximate = false, cv::Mat* debugImage = NULL);

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate = false, cv::Mat* debugImage = NULL);

private:

	double* hist;
	/** parametr sigma u normaloveho rozdeleni */
	int sigma;
	/** pocet ovlivnenych binu = sigma*range */
	int range;
	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
	/** zobrazi vizualizaci, pokud je uhel v toleranci; ve stupnich */
	int correctAngle;
	/** debug purpose - add border around the letter **/
	int borderForVis;

	bool doConvexHull;

	std::vector<double> probabilities;

	bool normalizeLength;

	double doEstimate( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL);
};

class VertDomChullSkDet : public ContourSkewDetector {
public:
	VertDomChullSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.014, int sigma = 3, int range = 3, int ignoreAngle = IGNORE_ANGLE, int correctAngle = 3, bool doConvexHull = false);
	virtual ~VertDomChullSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contour, bool approximate = false, cv::Mat* debugImage = NULL);

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, double weight, bool approximate = false,  cv::Mat* debugImage = NULL);

private:

	double* hist;
	/** parametr sigma u normaloveho rozdeleni */
	int sigma;
	/** pocet ovlivnenych binu = sigma*range */
	int range;
	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
	/** zobrazi vizualizaci, pokud je uhel v toleranci; ve stupnich */
	int correctAngle;
	/** debug purpose - add border around the letter **/
	int borderForVis;

	bool doConvexHull;

	std::vector<double> probabilities;
};

}
//namespace cmp

#endif /* VERTICALDOMSKDET_H_ */
