/*
 * ResultsWriter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */

#ifndef RESULTSWRITER_H_
#define RESULTSWRITER_H_

#include "SkewEvaluator.h"
#include <fstream>

namespace cmp {


class ResultsWriter {
public:
	ResultsWriter();
	virtual ~ResultsWriter();

	static void writeWorstDetectorResults(
			std::vector<EvaluationResult>& results,
			int classificator, int maxCount, std::fstream& outStream,
			std::string& outputDir, std::vector<std::string> detctorNames);
};

} /* namespace cmp */
#endif /* RESULTSWRITER_H_ */
