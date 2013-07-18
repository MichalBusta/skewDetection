/*
 * ResultsWriter.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */
#include <iostream>
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
		std::ofstream& outStream, std::string& outputDir, std::vector<std::string> detctorNames)
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

	std::sort(work.begin(), work.end(), &EvaluationResult::SortByAbsAngleDiff);

	//nejlepsi vysledky
	outStream << "<table border=\"1\">\n";
	outStream << "<th colspan=\"4\"><font color=\"green\">nejlepsi vysledky</font></th>\n";
	outStream << "<tr><td align=\"center\" width=\"150px\">Angle difference</td><td align=\"center\" width=\"150px\">Alphabet</td><td align=\"center\" width=\"150px\">Letter</td><td>picture</td></tr>\n";
	for(size_t i = 0; i < work.size(); i++)
	{
		std::ostringstream picture;
		picture << outputDir << "/" << detctorNames[work[i].classificator] << "/" << work[i].alphabet << "/" << work[i].letter << "/" << work[i].imageId << ".png";

		std::string pictureLink = picture.str();

		//<img src="" />

		//outStream << "<tr><td align=\"center\">" << work[i].angleDiff << "</td><td align=\"center\">" << work[i].alphabet << "</td><td align=\"center\">" << work[i].letter << "</td><td>" <<  << "</td></tr>\n";

		if( i > maxCount)
			break;
	}
	outStream << "</table>\n";
	outStream << "<br>\n";
	//nejhorsi vysledky
	outStream << "<table border=\"1\">\n";
	for(size_t i = work.size()-1; i >= 0; i--)
	{
		outStream << "<th colspan=\"3\"><font color=\"red\">nejhorsi vysledky</font></th>\n";
		outStream << "<tr><td align=\"center\" width=\"150px\">Angle difference</td><td align=\"center\" width=\"150px\">Alphabet</td><td align=\"center\" width=\"150px\">Letter<td><tr>\n";
		outStream << "<tr><td align=\"center\">" << work[i].angleDiff << "</td><td align=\"center\">" << work[i].alphabet << "</td><td align=\"center\">" << work[i].letter << "</td></tr>\n";

		if( i < work.size() - maxCount)
			break;
	}
	outStream << "</table>\n";
}

} /* namespace cmp */
