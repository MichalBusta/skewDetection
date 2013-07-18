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

#include "SkewDetection.h"

#define ANGLE_TOLERANCE M_PI / 60.0
#define ANGLE_MIN M_PI / 180.0

namespace cmp
{

struct SkewDef
{
	/** the ground-truth skew angle */
	double skewAngle;
	/** the skew step */
	size_t imageId;
	/** the skewed image */
	cv::Mat image;

	SkewDef(double skewAngle, const cv::Mat& image, size_t imageId) : skewAngle(skewAngle), image(image), imageId(imageId) { }
};

struct EvaluationResult
{
	double angleDiff;

	std::string alphabet;

	std::string letter;

	int classificator;

	size_t imageId;

	EvaluationResult(double angleDiff, std::string alphabet, std::string letter, int classificator, size_t imageId) : angleDiff(angleDiff), alphabet(alphabet), letter(letter), classificator(classificator), imageId(imageId) { };

	static bool SortByAbsAngleDiff(const EvaluationResult& obj1, const EvaluationResult& obj2)
	{
		return fabs(obj1.angleDiff) < fabs(obj2.angleDiff);
	}
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

	size_t nextImageId;

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
