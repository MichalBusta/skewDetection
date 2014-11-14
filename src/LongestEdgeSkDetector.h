/*
 * LongestEdgeSkDetector.h
 *
 *  Created on: Jul 12, 2013
 *      Author: cidlijak
 */

#ifndef LONGESTEDGESKDETECTOR_H_
#define LONGESTEDGESKDETECTOR_H_

#include "SkewDetector.h"

namespace cmp {

class LongestEdgeSkDetector : public ContourSkewDetector {
public:

	LongestEdgeSkDetector(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0.02, double ignoreAngle = IGNORE_ANGLE, double edgeRatio = 0.2, bool normalizeLength = false);

	virtual ~LongestEdgeSkDetector();

	virtual double detectSkew( std::vector<cv::Point>& contour, double lineK, bool approximate = false, cv::Mat* debugImage = NULL );

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate = false, cv::Mat* debugImage = NULL);
	/* ve stupnich */
	double ignoreAngle;
	/* udava % rozsah pro odhad pravdepodobnosti (kolik dalsich hran je s delkou longestEdge +- edgeRatio * longestEdge ) */
	double edgeRatio;

	std::vector<double> probabilities;
private:
	double doEstimate( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

	bool normalizeLength;

};

class LongestBitgEstimator : public ContourSkewDetector {
public:

	LongestBitgEstimator(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0.02, double ignoreAngle = IGNORE_ANGLE, double edgeRatio = 0.2);

	virtual ~LongestBitgEstimator();

	virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, double weight, cv::Mat* debugImage);
	/* ve stupnich */
	double ignoreAngle;
	/* udava % rozsah pro odhad pravdepodobnosti (kolik dalsich hran je s delkou longestEdge +- edgeRatio * longestEdge ) */
	double edgeRatio;

	std::vector<double> probabilities;
};

} /* namespace cmp */
#endif /* LONGESTEDGESKDETECTOR_H_ */
