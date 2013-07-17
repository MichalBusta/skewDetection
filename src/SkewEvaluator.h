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
	/** the skew step */
	int step;
	/** the skewed image */
	cv::Mat image;

	SkewDef(double skewAngle, const cv::Mat& image, int step) : skewAngle(skewAngle), image(image), step(step) { }
};

struct EvaluationResult
{
	double angleDiff;

	std::string alphabet;

	std::string letter;

	int classificator;

	EvaluationResult(double angleDiff, std::string alphabet, std::string letter, int classificator) : angleDiff(angleDiff), alphabet(alphabet), letter(letter), classificator(classificator) { };
};

struct AcumResult
{
	//pocet evaluaci
	int count;
	/** soucet stredni kvadraticke chyby */
	double sumDiff;
	//pocet spravnych klasifikaci
	int correctClassCont;

	int classIndex;

	AcumResult() : count(0), sumDiff(0), correctClassCont(0), classIndex(-1) {

	}
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
	SkewEvaluator( std::string outputDirectory, bool debug = false );

	virtual ~SkewEvaluator();

	void evaluate( const std::string& evalDir );

	void evaluateMat( cv::Mat& sourceImage, const std::string& alphabet, const std::string& letter );

	void registerDetector( cv::Ptr<SkewDetector> detector, const std::string detectorName );

	void writeResults();

private:

	void generateDistortions( cv::Mat& source,  std::vector<SkewDef>& distortions );

	std::vector<EvaluationResult> results;
	/** the detectors to evaluate */
	std::vector<cv::Ptr<SkewDetector> > detectors;
	/** the names of detector */
	std::vector<std::string> detectorNames;

	/** if true, results are show during the processing */
	bool debug;
	/** the output directory */
	std::string outputDirectory;

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
