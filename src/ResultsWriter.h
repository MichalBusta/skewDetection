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

struct LetterResults
{
	std::string letter;
	
	std::string alphabet;

	double biggestAngleDiff;
	double smallestAngleDiff;

	AcumResult acum;

	std::map<int, std::vector<EvaluationResult> > results;
	
	LetterResults() : biggestAngleDiff(0), smallestAngleDiff(M_PI) { };

	static bool SortByWorstDetection(const LetterResults& obj1, const LetterResults& obj2)
	{
		return obj1.biggestAngleDiff < obj2.biggestAngleDiff;
	}
};

class ResultsWriter {
public:
	ResultsWriter();
	virtual ~ResultsWriter();
	
	static void writeWorstDetectorResults(
			std::vector<EvaluationResult>& results,
			int classificator, int maxCount, std::fstream& outStream,
			std::string& outputDir, std::vector<std::string> detctorNames);

	static void writeLettersResults(
			std::vector<EvaluationResult>& results,
			int maxCount,
			std::string& outputDir, std::vector<std::string> detectorNames,
			double angleTolerance);
};

} /* namespace cmp */
#endif /* RESULTSWRITER_H_ */
