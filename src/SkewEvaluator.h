/*
 * SkewEvaluator.h
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */

#ifndef SKEWEVALUATOR_H_
#define SKEWEVALUATOR_H_

#include <string>

namespace cmp
{

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

	void evaluateMat( cv::Mat sourceImage );

private:

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
