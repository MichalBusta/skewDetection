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


	MeasuresHist ResultsWriter::writeDetectorMeasure(std::vector<EvaluationResult>& results,
		std::fstream& outStream, int classificator,
		std::vector<std::string> detectorNames)
	{
		MeasuresHist ret;
		ret.classificator = classificator;


		double goodTriesCount[10];
		for(int g = 0; g < 10; g++) goodTriesCount[g] = 0;

		int histMeasure1Count[10];
		int histMeasure2Count[10];
		double totalCountInBin[10];

		for(int i = 0; i < 10; i++)
		{
			ret.histMeasure1[i] = 0;
			ret.histMeasure2[i] = 0;
			histMeasure1Count[i] = 0;
			histMeasure2Count[i] = 0;
			totalCountInBin[i] = 0;
		}

		double maxValue = 0;
		double minValue = DBL_MAX;

		//max and min values in range
		for(size_t i = 0; i < results.size(); i++)
		{
			if(results[i].classificator != classificator)
				continue;

			maxValue = MAX( results[i].measure2, maxValue);
			minValue = MIN( results[i].measure2, minValue);
		}

		double binSize = 0;
		binSize = (maxValue - minValue)  / 10;

		int index = 0;
		int totalCount = 0;

		for(size_t i = 0; i < results.size(); i++)
		{

			if(results[i].classificator != classificator)
				continue;

			int boxNo = (int) ( results[i].measure1 - 1 );
			boxNo = MIN(boxNo, 9);
			boxNo = MAX(boxNo, 0);
			histMeasure1Count[boxNo]++;

			index = ( (int) ((results[i].measure2 - minValue) / binSize) );
			index = MIN(index, 9);
			index = MAX(index, 0);
			histMeasure2Count[index]++;
			totalCount++;

			if( fabs(results[i].angleDiff) < ANGLE_TOLERANCE )
			{
				goodTriesCount[boxNo]++;
				totalCountInBin[index] += 1;
			}
		}

		for(int t = 0; t < 10; t++)
		{
			if(histMeasure1Count[t] > 0)
				ret.histMeasure1[t] = goodTriesCount[t] / histMeasure1Count[t] * 100;
			else 
				ret.histMeasure1[t] = 0;
		}

		for(int i = 0; i < 10; i++)
			if(histMeasure2Count[i] > 0)
				ret.histMeasure2[i] = totalCountInBin[i] / histMeasure2Count[i] * 100;


		//if( fabs(results[i].angleDiff) < ANGLE_TOLERANCE )
		//{

		//example:

		outStream << "seriesMeasure = [{ \n";
		outStream << "             name: 'Measure1', \n";
		outStream << "             color: '#4572A7',\n";
		outStream << "             type: 'column',\n";
		outStream << "             data: [";
		string delim = "";
		for(int i = 0; i < 10; i++)
		{
			outStream << delim << ret.histMeasure1[i] << " ";
			delim = ",";
		}
		outStream << "],\n";


		outStream << "             tooltip: {\n";
		outStream << "                 valueSuffix: ' %'\n";
		outStream << "             }\n";
		outStream << "            }, {\n";
		outStream << "                name: 'Measure2',\n";
		outStream << "                color: '#89A54E',\n";
		outStream << "                type: 'column',\n";
		outStream << "                data: [";

		delim = "";
		for(int i = 0; i < 10; i++)
		{
			outStream << delim << ret.histMeasure2[i] << " ";
			delim = ",";
		}
		outStream << "],\n";

		outStream << "                tooltip: {\n";
		outStream << "                    valueSuffix: ' %'\n";
		outStream << "                }\n";

		outStream << "            }, {\n";
		outStream << "                name: 'Measure1: % of samples in bin ',\n";
		outStream << "                color: '#FF0000',\n";
		outStream << "                type: 'column',\n";
		outStream << "                data: [";

		delim = "";
		for(int i = 0; i < 10; i++)
		{
			outStream << delim << histMeasure1Count[i] / (double) totalCount * 100 << " ";
			delim = ",";
		}
		outStream << "],\n";

		outStream << "                tooltip: {\n";
		outStream << "                    valueSuffix: ''\n";
		outStream << "                }\n";
		outStream << "            }, {\n";
		outStream << "                name: 'Measure2: % of samples in bin " << minValue << "," << maxValue << " ',\n";
		outStream << "                color: '#FFFF00',\n";
		outStream << "                type: 'column',\n";
		outStream << "                data: [";

		delim = "";
		for(int i = 0; i < 10; i++)
		{
			outStream << delim << histMeasure2Count[i] / (double) totalCount * 100 << " ";
			delim = ",";
		}
		outStream << "],\n";

		outStream << "                tooltip: {\n";
		outStream << "                    valueSuffix: ''\n";
		outStream << "                }\n";

		outStream << "            }]\n";

		outStream << "categoriesMeasure = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10']" << std::endl;

		return ret;
	}

	inline static void writeResultsRow(std::fstream& outStream, std::string& outputDir, EvaluationResult& work, std::string& detectorName, int classificator)
	{
		std::ostringstream picture;
		if (classificator<0) picture << detectorName << "/";
		picture << work.alphabet << "/" << work.letter << "/" << work.imageId << ".png";
		std::string pictureLink = picture.str();
		outStream << "<tr><td>" << work.angleDiff << "</td><td>" << detectorName << "</td><td>&#" << work.letter << ";</td><td>" << work.measure1 << "</td><td>" << work.measure2 << "</td><td>" << "<img src=\"" << pictureLink << "\"/>" << "</td></tr>\n";
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


		//outStream << "<div class=\"preview\">\n";
		//outStream << "<h2>Results Preview</h2>\n";
		outStream << "<div class=\"column\">\n";

		outStream << "<h3>Worst Results</h3>\n";
		//nejhorsi vsledky
		outStream << "<table id=\"detectors_wrong_images\">\n";
		outStream << "<tr><td>Angle Difference</td><td>Detector</td><td>Letter</td><td>Measure1</td><td>Confidence</td><td>Preview</td></tr>\n";
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

		outStream << "<table id=\"detectors_right_images\">\n";
		outStream << "<tr><td>Angle Difference</td><td>Detector</td><td>Letter</td><td>Measure1</td><td>Confidence</td><td align=\"center\">Preview</td></tr>\n";
		resultsCount = 0;
		LetterCheck.clear();
		for(int i = 0; i < work.size(); i++)
		{
			//skip the best possible detector
			if(work[i].classificator == detectorNames.size() - 1)
				continue;

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
		outStream << "</table></div>";
		//outStream << "</div>\n";
	}

	/**
	* @param classificator - if >= 0 zapise vysledky pouze od daneho detektoru
	* @param maxCount - maximalni pocet radku v tabulce
	*/
	void ResultsWriter::writeBestResults(
		std::vector<EvaluationResult>& results, int maxCount,
		std::fstream& outStream, std::vector<std::string> detectorNames)
	{
		std::vector<EvaluationResult> work;

		work = results;

		std::set<string> LetterCheck;
		std::sort(work.begin(), work.end(), &EvaluationResult::SortByAbsAngleDiff);

		outStream << "<div class=\"column\">\n";
		outStream << "<h3>Best result for letter (worst->best)</h3>\n";

		outStream << "<table id=\"detectors_right_images\">\n";
		outStream << "<tr><td>Angle Difference</td><td>Detector</td><td>Letter</td><td>Measure1</td><td>Preview</td></tr>\n";
		int resultsCount = 0;
		LetterCheck.clear();
		for(int i = (int) work.size() -1; i >= 0; i--)
		{
			if( LetterCheck.find(work[i].letter) != LetterCheck.end() )
				continue;

			std::ostringstream pictureLink;
			pictureLink << detectorNames[work[i].classificator] << "/" << work[i].alphabet << "/" << work[i].letter << "/" << work[i].imageId << ".png";
			outStream << "<tr><td>" << work[i].angleDiff << "</td><td>" << detectorNames[work[i].classificator] << "</td><td>&#" << work[i].letter << ";</td><td>" <<  work[i].measure1 << "</td><td>" << "<img src=\"" << pictureLink.str() << "\"/>" << "</td></tr>\n";

			LetterCheck.insert(work[i].letter);

			if( resultsCount++ > maxCount)
				break;
		}
		outStream << "</table></div>";
	}

	bool sortResultsByBiggestDiff_subvector(const DetectorResults& o1, const DetectorResults& o2)
	{
		return fabs(o2.biggestAngleDiff) < fabs(o1.biggestAngleDiff);
	}

	bool sortResultsByBiggestDiff(const EvaluationResult& o1, const EvaluationResult& o2)
	{
		return fabs(o2.angleDiff) < fabs(o1.angleDiff);
	}

	void ResultsWriter::writeDetectorMeasuresTable(
		std::vector<MeasuresHist>& results, std::fstream& outStream,
		std::vector<std::string> detctorNames)
	{
		std::sort( results.begin(), results.end(), &MeasuresHist::SortHistMeasure1FirstBin );

		outStream << "<div class=\"DPM\"><h2>" << "Detector Probability Measures" << "</h2>";

		outStream << "<table>\n";
		outStream << "<tr>";
		outStream << "<th>Detector</th><th>Prob. H1</th><th>Prob. H2</th><th>Prob. H3</th><th>Prob. H4</th><th>Prob. M1</th><th>Prob. M2</th>\n";
		outStream << "</tr>";
		for(size_t i = 0; i < results.size(); i++)
		{
			outStream << "<tr>";
			outStream << "<td><a href=\"" <<  detctorNames[results[i].classificator] << "/index.htm\">" <<  detctorNames[results[i].classificator] << "</a></td><td>" << results[i].histMeasure1[0] << "</td><td>" << results[i].histMeasure1[1] <<  "</td><td>" << results[i].histMeasure1[2] << "</td><td>" << results[i].histMeasure1[3] << "</td>";
			outStream << "<td>" << results[i].histMeasure2[0] << "</td><td>" << results[i].histMeasure2[1] << "</td>";
			outStream << "</tr>\n";
		}

		outStream << "</table></div>\n";
	}

	void ResultsWriter::writeLettersResults(
		std::vector<EvaluationResult>& results,
		std::string& outputDir, std::vector<std::string> detectorNames,
		double angleTolerance, double angle_min)
	{
		std::map<std::string, std::vector<LetterResults> > lettersResults;

		std::map<std::string, LetterResults> letters;

		for(size_t i = 0; i < results.size(); i++)
		{

			letters[ results[i].letter ].letter = results[i].letter;
			letters[ results[i].letter ].alphabet = results[i].alphabet;

			letters[ results[i].letter ].acum.classIndex = results[i].classificator;
			letters[ results[i].letter ].acum.count++;
			letters[ results[i].letter ].acum.sumDiff = letters[ results[i].letter ].acum.sumDiff + results[i].angleDiff*results[i].angleDiff;
			letters[ results[i].letter ].detectors[results[i].classificator].results.push_back(results[i]);

			letters[ results[i].letter ].biggestAngleDiff = MAX(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].biggestAngleDiff));
			letters[ results[i].letter ].smallestAngleDiff = MIN(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].smallestAngleDiff));

			letters[ results[i].letter ].detectors[results[i].classificator].acum.classIndex = results[i].classificator;
			letters[ results[i].letter ].detectors[results[i].classificator].acum.count++;
			letters[ results[i].letter ].detectors[results[i].classificator].acum.sumDiff = letters[ results[i].letter ].detectors[results[i].classificator].acum.sumDiff + results[i].angleDiff*results[i].angleDiff;
			letters[ results[i].letter ].detectors[results[i].classificator].detector = results[i].classificator;
			letters[ results[i].letter ].detectors[results[i].classificator].alphabet = results[i].alphabet;
			letters[ results[i].letter ].detectors[results[i].classificator].letter = results[i].letter;

			letters[ results[i].letter ].detectors[results[i].classificator].biggestAngleDiff = MAX(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].detectors[results[i].classificator].biggestAngleDiff));
			letters[ results[i].letter ].detectors[results[i].classificator].smallestAngleDiff = MIN(fabs(results[i].angleDiff), fabs(letters[ results[i].letter ].detectors[results[i].classificator].smallestAngleDiff));

			if(fabs(letters[ results[i].letter ].faceBiggestDiff.angleDiff) <= fabs(results[i].angleDiff))
			{
				letters[ results[i].letter ].faceBiggestDiff = results[i];
			}

			if( fabs(results[i].angleDiff) < angleTolerance )
			{
				letters[ results[i].letter ].acum.correctClassCont++;
				letters[ results[i].letter ].detectors[results[i].classificator].acum.correctClassCont++;
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
			report_overview << std::fixed << std::setprecision(2);
			images_table << std::fixed << std::setprecision(2);
			report_overview << "\t<table id=\"alphabet_detail_overview\">\n" << "\t\t<tr>\n";
			report_overview << "\t\t\t<th rowspan=\"2\">Char</th>\n";
			images_table << "\t<table id=\"alphabet_detail_images\" class=\"images\">\n" << "\t\t<tr>\n";
			images_table << "\t\t\t<th>Char</th>\n";
			std::map<int, DetectorResults>::iterator last = it->second[0].detectors.end();
			--last;--last;
			std::stringstream subtitle;
			subtitle << std::fixed << std::setprecision(2);
			for(std::map<int, DetectorResults>::iterator iterator = it->second[0].detectors.begin(); iterator != --(it->second[0].detectors.end()); iterator++)
			{
				report_overview << "\t\t\t<th class=\"border_left\" colspan=\"2\">" << detectorNames[iterator->first] << "</th>\n";
				subtitle << "\t\t\t<th class=\"border_left\">% Correct</th>\n";
				subtitle << "\t\t\t<th class=\"border_right\">StdDev</th>\n";
				if (iterator == it->second[0].detectors.begin())
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
			report_overview << "\t\t\t<th class=\"border_left\" colspan=\"2\">" << detectorNames[detectorNames.size() - 1] << "</th>\n";
			subtitle << "\t\t\t<th class=\"border_left\">% Correct</th>\n";
			subtitle << "\t\t\t<th class=\"border_right\">StdDev</th>\n";

			images_table << "\t\t\t<th>Smallest angle diff</th>\n";
			images_table << "\t\t\t<th>Biggest angle diff</th>\n";
			images_table << "\t\t\t<th>Best detection for worst font</th>\n";
			report_overview << "\t\t</tr>\n";
			report_overview << "\t\t<tr>\n";
			report_overview << subtitle.str();
			report_overview << "\t\t</tr>\n";
			images_table << "\t\t</tr>\n";

			std::ofstream alphabet_json;
			alphabet_json.open ( (outputDirectory+"/object_data.js" ).c_str() );
			alphabet_json << std::fixed << std::setprecision(2);
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
			letterNames << std::fixed << std::setprecision(2);
			int colorIndex = 0;
			/** iterate through letters; it->second[i] --> LetterResults */
			std::map<int, std::string> data;
			for(size_t i = 0; i < it->second.size(); i++)
			{
				report_overview << "\t\t<tr>\n";
				report_overview << "\t\t\t<td>&#" << it->second[i].letter << ";</td>\n";

				images_table << "\t\t<tr>\n";
				images_table << "\t\t\t<td>&#" << it->second[i].letter << ";</td>\n";
				std::vector<DetectorResults> detectors;
				size_t worstFaceIndex = 0;
				double worstFaceAngle = 0.0;

				for(std::map<int, DetectorResults>::iterator iterator = it->second[i].detectors.begin(); iterator != it->second[i].detectors.end(); iterator++)
				{
					std::stringstream tmpStr;
					tmpStr << std::fixed << std::setprecision(2);
					detectors.push_back(iterator->second);

					if(i==0)
					{
						if(iterator != it->second[i].detectors.begin()) 
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

					if(iterator == it->second[i].detectors.begin())
					{
						if(i!=0)
						{
							letterNames << ", ";
						}
						letterNames << "'&#" << it->second[i].letter << ";'";
					}

					tmpStr << 100*iterator->second.acum.correctClassCont/iterator->second.acum.count;
					data[iterator->first] +=  tmpStr.str();
					report_overview << "\t\t\t<td class=\"border_left\">" << 100*iterator->second.acum.correctClassCont/iterator->second.acum.count << "%</td>\n";
					report_overview << "\t\t\t<td class=\"border_right\">" << iterator->second.acum.sumDiff << "</td>\n";
				}
				std::sort(detectors.begin(), detectors.end(), sortResultsByBiggestDiff_subvector);

				double bestDiff = M_PI;
				std::stringstream bestDet;
				bestDet << std::fixed << std::setprecision(2);

				int tooGoodRes = 0;

				for (size_t j = 0; j < detectors.size(); j++)
				{
					std::sort(detectors[j].results.begin(), detectors[j].results.end(), sortResultsByBiggestDiff);
					for(size_t k = 0; k < detectors[j].results.size(); k++) 
					{
						if (detectors[j].results[k].faceIndex == it->second[i].faceBiggestDiff.faceIndex && fabs(detectors[j].results[k].angleDiff) < fabs(bestDiff))
						{

							if (fabs(detectors[j].results[k].angleDiff) < fabs(angle_min))
							{
								tooGoodRes++;
							}
							else
							{
								//skip virtual detector "Best of all"
								if( detectors[j].results[k].classificator ==  detectorNames.size() - 1)
									continue;

								bestDet.str( "" );
								bestDet << "<img src=\"";
								bestDet << "../" << detectorNames[detectors[j].results[k].classificator] << "/" << detectors[j].results[k].alphabet << "/" << detectors[j].results[k].letter << "/" << detectors[j].results[k].imageId << ".png";
								bestDet << "\" ";
								bestDet << "title=\"" << detectorNames[detectors[j].results[k].classificator] << " &#10;";
								bestDet << "Angle diff " << detectors[j].results[k].angleDiff*180/M_PI << "&deg; &#10;";
								bestDet << "Detector accuracy " << 100*detectors[j].acum.correctClassCont/detectors[j].acum.count << "%\" />";
								bestDiff = detectors[j].results[k].angleDiff;
							}
						}
					}

					//skip virtual detector "Best of all"
					if( detectors[j].detector ==  detectorNames.size() - 1)
						continue;

					std::stringstream pictureLink;
					pictureLink << "../" << detectorNames[detectors[j].detector] << "/" << detectors[j].alphabet << "/" << detectors[j].letter << "/" << detectors[j].results[0].imageId << ".png";
					images_table << "\t\t\t<td><img src=\"" << pictureLink.str() << "\" ";
					images_table << "title=\"" << detectorNames[detectors[j].detector] << " &#10;";
					images_table << "Angle diff " << detectors[j].results[0].angleDiff*180/M_PI << "&deg; &#10;";
					images_table << "Detector accuracy " << 100*detectors[j].acum.correctClassCont/detectors[j].acum.count << "%\" /></td>\n";
				}
				report_overview << "\t\t</tr>\n";
				images_table << "\t\t\t<td>" << it->second[i].smallestAngleDiff*180/M_PI << "&deg;</td>\n";
				images_table << "\t\t\t<td>" << it->second[i].biggestAngleDiff*180/M_PI << "&deg;</td>\n";
				images_table << "\t\t\t<td>" << tooGoodRes << bestDet.str() << "</td>\n";

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
