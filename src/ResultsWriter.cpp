/*
 * ResultsWriter.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */
#include <iostream>
#include <set>
#include "ResultsWriter.h"

using namespace std;
namespace cmp {

ResultsWriter::ResultsWriter() {
	// TODO Auto-generated constructor stub

}

ResultsWriter::~ResultsWriter() {
	// TODO Auto-generated destructor stub
}

/**
 * @param classificator - if >= 0 zapise vysledky pouze od daneho detektoru
 * @param maxCount - maximalni pocet radku v tabulce
 */

void ResultsWriter::writeWorstDetectorResults(
		std::vector<EvaluationResult>& results, int classificator, int maxCount,
		std::fstream& outStream, std::string& outputDir, std::vector<std::string> detectorNames)
{
	std::vector<EvaluationResult> work;
	std::vector<EvaluationResult> clsResults;
	if( classificator >= 0 )
	{
		for(size_t i = 0; i < results.size(); i++)
		{
			if(results[i].classificator == classificator)
				clsResults.push_back(results[i]);
		}
		work = clsResults;
	}else
		work = results;

	std::set<string> LetterCheck;
	std::sort(work.begin(), work.end(), &EvaluationResult::SortByAbsAngleDiff);

	
	outStream << "<div class=\"preview\">\n";
	outStream << "<h2>Results Preview</h2>\n";
	outStream << "<div class=\"column\">\n";

	outStream << "<h3>Worst Results</h3>\n";
	//nejhorsi vsledky
	outStream << "<table>\n";
	outStream << "<tr><td>Angle Difference</td><td>Detector</td><td>Letter</td><td>Preview</td></tr>\n";
	int resultsCount = 0;
	for(int i = (int) work.size() -1; i >= 0; i--)
	{

		if( LetterCheck.find(work[i].letter) != LetterCheck.end() )
			continue;

		std::ostringstream picture;
		picture << outputDir << "/" << detectorNames[work[i].classificator] << "/" << work[i].alphabet << "/" << work[i].letter << "/" << work[i].imageId << ".png";
		std::string pictureLink = picture.str();
		outStream << "<tr><td align=\"center\">" << work[i].angleDiff << "</td><td>" << detectorNames[work[i].classificator] << "</td><td>" << work[i].letter << "</td><td>" << "<img src=\"" << pictureLink << "\"/>" << "</td></tr>\n";

		LetterCheck.insert(work[i].letter);


		if( resultsCount++ > maxCount)
			break;

	}
	outStream << "</table></div>\n";
	
	outStream << "<div class=\"column\">\n";
	//nejlepsi vysledky
	outStream << "<h3>Best Results</h3>\n";

	outStream << "<table>\n";
	outStream << "<tr><td>Angle Difference</td><td>Detector</td><td>Letter</td><td align=\"center\">Preview</td></tr>\n";
	resultsCount = 0;
	LetterCheck.clear();
	for(size_t i = 0; i < work.size(); i++)
	{
		//skip the almost correct angles ...
		if( fabs(work[i].angleDiff) < ANGLE_MIN)
			continue;

		if( LetterCheck.find(work[i].letter) != LetterCheck.end() )
			continue;

		std::ostringstream picture;
		picture << outputDir << "/" << detectorNames[work[i].classificator] << "/" << work[i].alphabet << "/" << work[i].letter << "/" << work[i].imageId << ".png";

		std::string pictureLink = picture.str();

		LetterCheck.insert(work[i].letter);

		outStream << "<tr><td>" << work[i].angleDiff << "</td><td>" << detectorNames[work[i].classificator] << "</td><td>&#" << work[i].letter << ";</td><td>" << "<img src=\"" << pictureLink << "\"/>" << "</td></tr>\n";

		if( resultsCount++ > maxCount)
			break;
	}
	outStream << "</table></div></div>\n";





}

} /* namespace cmp */
