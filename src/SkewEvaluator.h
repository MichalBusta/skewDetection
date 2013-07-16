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
#include <map>

#include "SkewDetector.h"

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

struct EvaluationResult
{
	double angleDiff;

	std::string alphabet;

	std::string letter;

	EvaluationResult(double angleDiff, std::string alphabet, std::string letter) : angleDiff(angleDiff), alphabet(alphabet), letter(letter) { };
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
	SkewEvaluator( bool debug = false );

	virtual ~SkewEvaluator();

	void evaluate( const std::string& evalDir );

	void evaluateMat( cv::Mat& sourceImage, const std::string& alphabet, const std::string& letter );

	void registerDetector( cv::Ptr<SkewDetector> detector, const std::string detectorName );

private:

	void generateDistortions( cv::Mat& source,  std::vector<SkewDef>& distortions );

	std::vector<EvaluationResult> results;
	/** the detectors to evaluate */
	std::vector<cv::Ptr<SkewDetector> > detectors;
	/** the names of detector */
	std::vector<std::string> detectorNames;

	/** if true, results are show during the processing */
	bool debug;

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
