/*
* LRLongestEdge.h
*
*  Created on: Jul 23, 2013
*      Author: Michal Busta
*/

#ifndef LRLONGESTEDGE_H_
#define LRLONGESTEDGE_H_

#include "SkewDetector.h"

namespace cmp
{

	/**
	* @class cmp::LRLongestEdge
	* 
	* @brief TODO brief description
	*
	* TODO type description
	*/
	class LRLongestEdge : public ContourSkewDetector
	{
	public:
		LRLongestEdge(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.026, int ignoreAngle = IGNORE_ANGLE, bool left = true);
		virtual ~LRLongestEdge();

		virtual double detectSkew( std::vector<cv::Point>& contour, cv::Mat* debugImage = NULL );

		/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
		int ignoreAngle;

		/** pri true bude hledat levou nejdelsi hranu, pri false pravou */
		bool left;

		virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double *histogram, double weight, cv::Mat* debugImage){

		}
	};

} /* namespace cmp */
#endif /* LRLONGESTEDGE_H_ */
