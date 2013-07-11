/*

 * SkewEvaluator.h
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */

#ifndef SKEWEVALUATOR_H_
#define SKEWEVALUATOR_H_

#include <opencv2/core/core.hpp>

#include <string>

namespace cmp
{

struct SkewDef
{
	/** the ground-truth skew angle */
	double skewAngle;
	/** the skewed image */
	cv::Mat image;

	SkewDef(double skewAngle, const cv::Mat& image) : skewAngle(skewAngle), image(image) { }
};

/**
 * @class cmp::SkewEvaluator
 * 
 * @brief Evaluates dataset
 *
 * Processes all dataset directory
 */
class SkewEvaluator
{
public:
	SkewEvaluator();

	virtual ~SkewEvaluator();

	void evaluate( const std::string& evalDir );

	void evaluateMat( cv::Mat& sourceImage );

private:

	void generateDistortions( cv::Mat& source,  std::vector<SkewDef>& distortions );

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
