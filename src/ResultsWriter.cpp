/*
 * ResultsWriter.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>

#include "ResultsWriter.h"
#include "IOUtils.h"
#include "TemplateUtils.h"

using namespace std;
namespace cmp {

ResultsWriter::ResultsWriter() {
	// TODO Auto-generated constructor stub

}

ResultsWriter::~ResultsWriter() {
	// TODO Auto-generated destructor stub
}

inline static void writeResultsRow(std::fstream& outStream, std::string& outputDir, EvaluationResult& work, std::string& detectorName, int classificator)
{
	std::ostringstream picture;
	if (classificator<0) picture << detectorName << "/";
	picture << work.alphabet << "/" << work.letter << "/" << work.imageId << ".png";
	std::string pictureLink = picture.str();
	outStream << "<tr><td>" << work.angleDiff << "</td><td>" << detectorName << "</td><td>&#" << work.letter << ";</td><td>" << "<img src=\"" << pictureLink << "\"/>" << "</td></tr>\n";
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

		writeResultsRow(outStream, outputDir, work[i], detectorNames[work[i].classificator], classificator);
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

		writeResultsRow(outStream, outputDir, work[i], detectorNames[work[i].classificator], classificator);
		LetterCheck.insert(work[i].letter);

		if( resultsCount++ > maxCount)
			break;
	}
	outStream << "</table></div></div>\n";
}

bool sortResultsByBiggestDiff_subvector(const std::vector<EvaluationResult>& o1, const std::vector<EvaluationResult>& o2)
{
	double biggestDiff1 = 0.0;
	double biggestDiff2 = 0.0;
	for (size_t i=0; i<o1.size(); i++)
	{
		biggestDiff1 = MAX(biggestDiff1, fabs(o1[i].angleDiff));
	}
	for (size_t i=0; i<o2.size(); i++)
	{
		biggestDiff2 = MAX(biggestDiff2, fabs(o2[i].angleDiff));
	}
	return biggestDiff2 < biggestDiff1;
}

bool sortResultsByBiggestDiff(const EvaluationResult& o1, const EvaluationResult& o2)
{
	return fabs(o2.angleDiff) < fabs(o1.angleDiff);
}

void ResultsWriter::writeLettersResults(
			std::vector<EvaluationResult>& results,
			int maxCount,
			std::string& outputDir, std::vector<std::string> detectorNames,
			double angleTolerance)
{
	/* resultsMap[alphabet][detector][letter] */
	//std::map<std::string, std::map<int, std::map<std::string, AcumResult> > > resultsMap;
	//std::vector<std::map<std::string, std::map<int, AcumResult> > > lettersResults;
	//std::set<string> letterCheck;

	/*if( letterCheck.find(work[i].letter) != LetterCheck.end() )
			continue;*/

	std::map<std::string, std::vector<LetterResults> > lettersResults;

	std::map<std::string, LetterResults> letters;

	for(size_t i = 0; i < results.size(); i++)
	{
		//letterCheck.insert(results[i].letter);
		
		letters[ results[i].letter ].letter = results[i].letter;
		letters[ results[i].letter ].alphabet = results[i].alphabet;

		letters[ results[i].letter ].acum.classIndex = results[i].classificator;
		letters[ results[i].letter ].acum.count++;
		letters[ results[i].letter ].acum.sumDiff = letters[ results[i].letter ].acum.sumDiff + results[i].angleDiff*results[i].angleDiff;
		letters[ results[i].letter ].results[results[i].classificator].push_back(results[i]);
		
		letters[ results[i].letter ].biggestAngleDiff = MAX(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].biggestAngleDiff));
		letters[ results[i].letter ].smallestAngleDiff = MIN(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].smallestAngleDiff));


		if( fabs(results[i].angleDiff) < angleTolerance )
		{
			letters[ results[i].letter ].acum.correctClassCont++;
		}
	}
	for (std::map<std::string, LetterResults>::iterator it = letters.begin(); it != letters.end(); it++)
	{
		lettersResults[it->second.alphabet].push_back(it->second);
	}

	for (std::map<std::string, std::vector<LetterResults> >::iterator it = lettersResults.begin(); it != lettersResults.end(); it++)
	{
		std::sort(it->second.begin(), it->second.end(), &LetterResults::SortByWorstDetection);
		std::string outputDirectory = outputDir+"/"+it->first;
		IOUtils::CreateDir( outputDirectory );
		TemplateUtils::CopyAlphabetIndexTemplates( ".", outputDirectory );

		std::fstream report_overview;
		std::stringstream images_table;
		report_overview.open ( (outputDirectory+ "/index.htm").c_str(), std::fstream::out | std::fstream::app );
		report_overview << "\t<table>\n" << "\t\t<tr>\n";
		report_overview << "\t\t\t<th>Char</th>\n";
		images_table << "\t<table class=\"images\">\n" << "\t\t<tr>\n";
		images_table << "\t\t\t<th>Char</th>\n";
		std::map<int, std::vector<EvaluationResult> >::iterator last = it->second[0].results.end();
		--last;
		for(std::map<int, std::vector<EvaluationResult> >::iterator iterator = it->second[0].results.begin(); iterator != it->second[0].results.end(); iterator++)
		{
			report_overview << "\t\t\t<th>" << detectorNames[iterator->first] << "</th>\n";
			if (iterator == it->second[0].results.begin())
			{
				images_table << "\t\t\t<th>Worst detection</th>\n";
			}
			else if (iterator == last)
			{
				images_table << "\t\t\t<th>Best detection</th>\n";
			}
			else
			{
				images_table << "\t\t\t<th>&nbsp;</th>\n";
			}
		}
		images_table << "\t\t\t<th>Smallest angle diff</th>\n";
		images_table << "\t\t\t<th>Biggest angle diff</th>\n";
		report_overview << "\t\t</tr>\n";
		images_table << "\t\t</tr>\n";

		std::ofstream alphabet_json;
		alphabet_json.open ( (outputDirectory+"/object_data.js" ).c_str() );
	
		alphabet_json << "series = [";
		std::vector<std::string> colors;
	
		colors.push_back("#928333");
		colors.push_back("#da0d4d");
		colors.push_back("#bd1fc6");
		colors.push_back("#0950d2");
		colors.push_back("#987e42");
		colors.push_back("#e33154");
		colors.push_back("#0f3313");
		colors.push_back("#f41b15");

		std::stringstream letterNames;
		int colorIndex = 0;
		/********/
		std::map<int, std::string> data;
		for(size_t i = 0; i < it->second.size(); i++)
		{
			report_overview << "\t\t<tr>\n";
			report_overview << "\t\t\t<td>&#" << it->second[i].letter << ";</td>\n";
			
			images_table << "\t\t<tr>\n";
			images_table << "\t\t\t<td>&#" << it->second[i].letter << ";</td>\n";
			std::vector<std::vector<EvaluationResult> > detectors;
			for(std::map<int, std::vector<EvaluationResult> >::iterator iterator = it->second[i].results.begin(); iterator != it->second[i].results.end(); iterator++)
			{
				std::stringstream tmpStr;
				detectors.push_back(iterator->second);
				if(i==0)
				{
					if(iterator != it->second[i].results.begin()) 
					{

						tmpStr << ", \n";
					}
					tmpStr << "{\n";
					tmpStr << "\tname: '" << detectorNames[iterator->first] << "',\n";
					tmpStr << "\tcolor: '" + colors[colorIndex] << "',\n";
					colorIndex = colorIndex==colors.size()-1 ? 0 : colorIndex+1;
					tmpStr << "\ttype: 'column',\n";
					tmpStr << "\tdata: [";
				}
				else
				{
					tmpStr << ", ";
				}
				double detectorTotal = 0.0;
				double detectorCorrect = 0.0;
				for(size_t j = 0; j < iterator->second.size(); j++)
				{
					detectorTotal++;
					if(fabs(iterator->second[j].angleDiff) < angleTolerance) detectorCorrect++;
				}

				if(iterator == it->second[i].results.begin())
				{
					if(i!=0)
					{
						letterNames << ", ";
					}
					letterNames << "'&#" << it->second[i].letter << ";'";
				}

				tmpStr << std::fixed << std::setprecision(2) << 100*detectorCorrect/detectorTotal;
				data[iterator->first] +=  tmpStr.str();
				report_overview << "\t\t\t<td>" << std::fixed << std::setprecision(2) << 100*detectorCorrect/detectorTotal << "%</td>\n";
			}
			std::sort(detectors.begin(), detectors.end(), sortResultsByBiggestDiff_subvector);
			for (size_t j = 0; j < detectors.size(); j++) 
			{
				double detectorTotal = 0.0;
				double detectorCorrect = 0.0;
				for(size_t k = 0; k < detectors[j].size(); k++)
				{
					detectorTotal++;
					if(fabs(detectors[j][k].angleDiff) < angleTolerance) detectorCorrect++;
				}
				std::sort(detectors[j].begin(), detectors[j].end(), sortResultsByBiggestDiff);
				std::stringstream pictureLink;
				pictureLink << "../" << detectorNames[detectors[j][0].classificator] << "/" << detectors[j][0].alphabet << "/" << detectors[j][0].letter << "/" << detectors[j][0].imageId << ".png";
				images_table << "\t\t\t<td><img src=\"" << pictureLink.str() << "\" ";
				images_table << "title=\"" << detectorNames[detectors[j][0].classificator] << " &#10;";
				images_table << "Angle diff " << detectors[j][0].angleDiff*180/M_PI << " &#10;";
				images_table << "Detector accuracy " << std::fixed << std::setprecision(2) << 100*detectorCorrect/detectorTotal << "%\" /></td>\n";
			}
			report_overview << "\t\t</tr>\n";
			images_table << "\t\t\t<td>" << it->second[i].smallestAngleDiff*180/M_PI << "</td>\n";
			images_table << "\t\t\t<td>" << it->second[i].biggestAngleDiff*180/M_PI << "</td>\n";
			images_table << "\t\t</tr>\n";
		}

		for(std::map<int, std::string>::iterator iterator = data.begin(); iterator != data.end(); iterator++)
		{
			alphabet_json << iterator->second;
			alphabet_json << "],\n";
			alphabet_json << "\ttooltip: {\n";
			alphabet_json << "\t\tvalueSuffix: ' %'\n";
			alphabet_json << "\t}\n";
			alphabet_json << "}";
		}

		alphabet_json << "]\n";
		/****/
		alphabet_json << "\n\n" << "categories = [";

		alphabet_json << letterNames.str();
	
		alphabet_json << "];";
		alphabet_json.close();

		report_overview << "\t</table>\n";

		images_table << "\t</table>\n";
		report_overview << images_table.str();
		
		report_overview << "</body>\n</html>";

		report_overview.close();

	}

}

} /* namespace cmp */
