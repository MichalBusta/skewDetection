/*

 * SkewEvaluator.h
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */

#ifndef SKEWEVALUATOR_H_
#define SKEWEVALUATOR_H_

#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>

#include <string>
#include <map>
#include <math.h>
#include <cmath>

#include "SkewDetection.h"

#define ANGLE_MIN M_PI / 120.0

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
	/* rozdil v radianech oproti spravnemu vysledku */
	double angleDiff;

	std::string alphabet;

	std::string letter;
	/* identifikace detektoru */
	int classificator;

	size_t imageId;

	size_t faceIndex;

	/** measure for detector to estimate probability */
	double measure1;

	/** measure for detector to estimate probability */
	double measure2;

	double probability;

	double gtAngle;

	double estimAngle;

	int isWorst;

	EvaluationResult(double angleDiff, std::string alphabet, std::string letter, int classificator, size_t imageId, size_t faceIndex, int isWorst, double gtAngle, double estimAngle)
    :
			angleDiff(angleDiff), alphabet(alphabet), letter(letter),classificator(classificator), imageId(imageId), faceIndex(faceIndex),
			measure1(0.0), measure2(0.0), probability(0.0), isWorst(isWorst),  gtAngle(gtAngle), estimAngle(estimAngle)
    {
        
	}
	;
	
	EvaluationResult() : angleDiff(0), measure1(0.0), measure2(0.0), classificator(-1), faceIndex(0), imageId(0), probability(0), isWorst(0), gtAngle(0), estimAngle(0.0) { };

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

	int isWorst;

	AcumResult() : count(0), sumDiff(0), correctClassCont(0), classIndex(-1), isWorst(0) {

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
	SkewEvaluator( std::string outputDirectory, bool debug = false, bool writeImages = false, int distAngleMethod = 0 );

	virtual ~SkewEvaluator();

	void evaluate( const std::string& evalDir );


	void evaluateWords( const std::string& evalDir, std::vector<cv::Ptr<ContourWordSkewDetector> >& wordSkewDetectors );

	void evaluateMat( cv::Mat& sourceImage, const std::string& alphabet, const std::string& letter, size_t faceIndex );

    void evaluateWordsMat( std::vector<cv::Mat>& letterImages, std::vector<cv::Rect>& bounds ,const std::string& alphabet, const std::string& letter, size_t faceIndex, std::vector<cv::Ptr<ContourWordSkewDetector> >& wordSkewDetectors, std::string& wordImage);

	void registerDetector( cv::Ptr<SkewDetector> detector, const std::string& detectorName,  const std::string& detectorCaption);

	void writeResults();

	/** the names of detector */
	std::vector<std::string> detectorNames;
	/** the long detector names*/
	std::vector<std::string> detectorCaptions;

	int distAngleMethod;

private:

	void generateDistortions( cv::Mat& source,  std::vector<SkewDef>& distortions );

	std::vector<EvaluationResult> results;
	std::vector<EvaluationResult> bestResults;
	/** the detectors to evaluate */
	std::vector<cv::Ptr<SkewDetector> > detectors;

	/** if true, results are show during the processing */
	bool debug;
	/** the output directory */
	std::string outputDirectory;

	size_t nextImageId;

	/** if true, result images are saved */
	bool writeImages;

};

} /* namespace cmp */
#endif /* SKEWEVALUATOR_H_ */
