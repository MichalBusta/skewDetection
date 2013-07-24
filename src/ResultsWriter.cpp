/*
 * ResultsWriter.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: cidlijak
 */
#include <iostream>
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

/*inline static void writeResultsRow(std::fstream& outStream, std::string& outputDir, EvaluationResult& work, std::string& detectorName, int classificator)
{
	std::ostringstream picture;
	if (classificator<0) picture << detectorName << "/";
	picture << work.alphabet << "/" << work.letter << "/" << work.imageId << ".png";
	std::string pictureLink = picture.str();
	outStream << "<tr><td>" << work.angleDiff << "</td><td>" << detectorName << "</td><td>&#" << work.letter << ";</td><td>" << "<img src=\"" << pictureLink << "\"/>" << "</td></tr>\n";
}*/

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

		letters[ results[i].letter ].biggestAngleDiff = MAX(fabs(results[i].angleDiff*results[i].angleDiff), fabs(letters[ results[i].letter ].biggestAngleDiff));


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
		std::map<int, std::stringstream> data;
		for(size_t i = 0; i < it->second.size(); i++)
		{
			for(std::map<int, std::vector<EvaluationResult> >::iterator iterator = it->second[i].results.begin(); iterator != it->second[i].results.end(); iterator++)
			{
				if(i==0)
				{
					if(iterator != it->second[i].results.begin()) 
					{
						data[iterator->first] << ", \n";
					}
					data[iterator->first] << "{\n";
					data[iterator->first] << "\tname: '" << detectorNames[iterator->first] << "',\n";
					data[iterator->first] << "\tcolor: '" << colors[colorIndex] << "',\n";
					colorIndex = colorIndex==colors.size()-1 ? 0 : colorIndex+1;
					data[iterator->first] << "\ttype: 'column',\n";
					data[iterator->first] << "\tdata: [";
				}
				else
				{
					data[iterator->first] << ", ";
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

				data[iterator->first] << std::fixed << std::setprecision(2) << 100*detectorCorrect/detectorTotal;
			}
			/*alphabet_json << "],\n";
			alphabet_json << "\ttooltip: {\n";
			alphabet_json << "\t\tvalueSuffix: ' %'\n";
			alphabet_json << "\t}\n";
			alphabet_json << "}";*/
		}

		for(std::map<int, std::stringstream>::iterator iterator = data.begin(); iterator != data.end(); iterator++)
		{
			alphabet_json << iterator->second.str();
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
	}
	/*
	std::stringstream detectors;
		int colorIndex = 0;

		for(size_t i = 0; i < it->second.size(); i++)
		{
			if(i > 0)
			{
				alphabet_json << ", \n";
			}
			alphabet_json << "{\n";
			alphabet_json << "\tname: '" << it->second[i].letter << "',\n";
			alphabet_json << "\tcolor: '" << colors[colorIndex] << "',\n";
			colorIndex = colorIndex==colors.size()-1 ? 0 : colorIndex+1;
			alphabet_json << "\ttype: 'column',\n";
			alphabet_json << "\tdata: [";
			for(std::map<int, std::vector<EvaluationResult> >::iterator iterator = it->second[i].results.begin(); iterator != it->second[i].results.end(); iterator++)
			{
				double detectorTotal = 0;
				double detectorCorrect = 0;

				for (size_t j = 0; j < iterator->second.size(); j++)
				{
					detectorTotal++;
					if (fabs(results[i].angleDiff) < angleTolerance )
					{
						detectorCorrect++;
					}
				}

				if(iterator != it->second[i].results.begin()) alphabet_json << ", ";


				if(i == 0)
				{
					if(iterator != it->second[i].results.begin())
					{
						detectors << ", ";
					}
					detectors << "'" << detectorNames[iterator->first] << "'";
				}
				alphabet_json << std::fixed << std::setprecision(2) << 100*detectorCorrect/detectorTotal;
			}
			alphabet_json << "],\n";
			alphabet_json << "\ttooltip: {\n";
			alphabet_json << "\t\tvalueSuffix: ' %'\n";
			alphabet_json << "\t}\n";
			alphabet_json << "}";
		}
		alphabet_json << "]\n";

		alphabet_json << "\n\n" << "categories = [";

		alphabet_json << detectors.str();*/










	/**
	std::ofstream alphabet_json;
	alphabet_json.open ( (outputDirectory+"/alphabet_object_data.js" ).c_str() );
	
	alphabet_json << "alphabet_series = [";
	std::vector<std::string> colors;
	
	colors.push_back("#928333");
	colors.push_back("#da0d4d");
	colors.push_back("#bd1fc6");
	colors.push_back("#0950d2");
	colors.push_back("#987e42");
	colors.push_back("#e33154");
	colors.push_back("#0f3313");
	colors.push_back("#f41b15");

	std::stringstream alphabetNames;
	int colorIndex = 0;
	for(std::map<int, std::map<std::string, AcumResult> >::iterator it = alphabetMap.begin(); it != alphabetMap.end(); it++)
	{
		if(it != alphabetMap.begin())
		{
			alphabet_json << ", ";
		}
		alphabet_json << "{\n";
		alphabet_json << "\tname: '" << detectorNames[it->first] << "',\n";
		alphabet_json << "\tcolor: '" << colors[colorIndex] << "',\n";
		colorIndex = colorIndex==colors.size()-1 ? 0 : colorIndex+1;
		alphabet_json << "\ttype: 'column',\n";
		alphabet_json << "\tdata: [";
		for(std::map<std::string, AcumResult>::iterator iterator = it->second.begin(); iterator != it->second.end(); iterator++)
		{
			if(iterator != it->second.begin()) alphabet_json << ", ";
			if(it == alphabetMap.begin())
			{
				if(iterator != it->second.begin())
				{
					alphabetNames << ", ";
				}
				alphabetNames << "'" << iterator->first << "'";
			}
			alphabet_json << std::fixed << std::setprecision(2) << 100*double(iterator->second.correctClassCont)/double(iterator->second.count);
		}
		alphabet_json << "],\n";
		alphabet_json << "\ttooltip: {\n";
		alphabet_json << "\t\tvalueSuffix: ' %'\n";
		alphabet_json << "\t}\n";
		alphabet_json << "}";
	}
	alphabet_json << "]\n";

	alphabet_json << "\n\n" << "alphabet_categories = [";

	alphabet_json << alphabetNames.str();
	
	alphabet_json << "];";
	alphabet_json.close();
	*/
}

} /* namespace cmp */
