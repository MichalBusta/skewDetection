/*
 * LRLongestEdge.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: Michal Busta
 */

#include "LRLongestEdge.h"

namespace cmp
{


LRLongestEdge::LRLongestEdge(int approximatioMethod, double epsilon) : ContourSkewDetector(approximatioMethod, epsilon)
{
}

LRLongestEdge::~LRLongestEdge()
{
	// TODO Auto-generated destructor stub
}

double LRLongestEdge::detectSkew(const cv::Mat& mask,
		std::vector<std::vector<cv::Point> >& contours,
		std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage)
{
	return 0;
}

} /* namespace cmp */
