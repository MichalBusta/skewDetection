/*
 * ResultsWriter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */

#ifndef RESULTSWRITER_H_
#define RESULTSWRITER_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include "SkewEvaluator.h"
#include <fstream>
#include <map>

namespace cmp {

struct DetectorResults
{
	std::string letter;
	
	std::string alphabet;

	int detector;

	double biggestAngleDiff;
	double smallestAngleDiff;

	AcumResult acum;

	std::vector<EvaluationResult> results;
	
	DetectorResults() : biggestAngleDiff(0), smallestAngleDiff(M_PI) { };
};

struct LetterResults
{
	std::string letter;
	
	std::string alphabet;

	double biggestAngleDiff;
	double smallestAngleDiff;

	AcumResult acum;

	EvaluationResult faceBiggestDiff;

	std::map<int, DetectorResults> detectors;

	LetterResults() : biggestAngleDiff(0), smallestAngleDiff(M_PI) { };

	static bool SortByWorstDetection(const LetterResults& obj1, const LetterResults& obj2)
	{
		return obj1.biggestAngleDiff < obj2.biggestAngleDiff;
	}
};

struct MeasuresHist
{
	int classificator;
	double histMeasure1[10];
	double histMeasure2[10];

};

class ResultsWriter {
public:
	ResultsWriter();
	virtual ~ResultsWriter();
	
	static void writeWorstDetectorResults(
			std::vector<EvaluationResult>& results,
			int classificator, int maxCount, std::fstream& outStream,
			std::string& outputDir, std::vector<std::string> detctorNames);

	static void writeBestResults(
		std::vector<EvaluationResult>& results, int maxCount,
		std::fstream& outStream, std::vector<std::string> detectorNames);

	static MeasuresHist writeDetectorMeasure(
			std::vector<EvaluationResult>& results,	std::fstream& outStream, int classificator, std::vector<std::string> detectorNames);

	static void writeLettersResults(
			std::vector<EvaluationResult>& results,
			std::string& outputDir, std::vector<std::string> detectorNames,
			double angleTolerance, double angle_min);
};

} /* namespace cmp */
#endif /* RESULTSWRITER_H_ */
