/*
 * SkewEvaluator.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */
#define _USE_MATH_DEFINES
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <math.h>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "SkewEvaluator.h"
#include "IOUtils.h"
#include "TemplateUtils.h"
#include "ResultsWriter.h"

#ifdef DO_PARALLEL
#	include <omp.h>
#endif


namespace cmp
{

SkewEvaluator::SkewEvaluator( std::string outputDirectory, bool debug, bool writeImages ) : outputDirectory(outputDirectory), debug( debug ), nextImageId(0), writeImages( writeImages )
{
	if(!IOUtils::PathExist(outputDirectory))
	{
		IOUtils::CreateDir( outputDirectory );
	}
	TemplateUtils::CopyIndexTemplates( ".", outputDirectory );

	this->results.reserve(20000);
}

SkewEvaluator::~SkewEvaluator()
{
	// TODO Auto-generated destructor stub
}

/**
 * Runs evaluation on provided directory
 *
 * The evaluation directory should have the structure:
 * evalDir/alphabet/utfLetterCodeNumber/FontName.png ...
 *
 * The font alphabets are Latin|Cyrilic|...
 *
 * @param evalDir the evaluation directory
 */
void SkewEvaluator::evaluate( const std::string& evalDir )
{
	std::vector<std::string> facesDir = IOUtils::GetDirectoriesInDirectory(evalDir, "*", true);
	//for all alphabets
	for( size_t i = 0; i < facesDir.size(); i++ )
	{
		//process all defined letter
		std::string alphabet = IOUtils::Basename( facesDir[i] );

		std::vector<std::string> letterDirs = IOUtils::GetDirectoriesInDirectory(facesDir[i], "*", true);
		for(size_t j = 0; j < letterDirs.size(); j++)
		{
			std::string letterUnicode = IOUtils::Basename( letterDirs[j] );
			std::vector<std::string> letterImages = IOUtils::GetFilesInDirectory(letterDirs[j], "*.png", true);

			for(size_t k = 0; k < letterImages.size(); k++)
			{
				const std::string& letterFile = letterImages[k];
				std::cout << "Processing image: " << letterFile << std::endl;
				cv::Mat tmp = cv::imread(letterFile, cv::IMREAD_GRAYSCALE);
				cv::Mat img;
				copyMakeBorder( tmp, img, 50, 50, 50, 50, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255) );
				evaluateMat( img, alphabet, letterUnicode, k );
			}
		}
	}

	writeResults();
}

/**
 * Create image which is horizontal alignment of imagesToMerge
 *
 * @param imagesToMerge
 * @param spacing space between images
 * @param verticalDisplacement
 *
 * @return the merged image
 */
static cv::Mat mergeHorizontal(std::vector<cv::Mat>& imagesToMerge, int spacing, int verticalDisplacement, std::vector<cv::Point>* imagesCenters )
{
	int sw = 0;
	int sh = 0;
	for( std::vector<cv::Mat>::iterator it =  imagesToMerge.begin(); it < imagesToMerge.end(); it++ )
	{
		sw += it->cols + spacing;
		sh = MAX(it->rows + verticalDisplacement, sh);
	}



	cv::Mat mergedImage = cv::Mat::zeros(sh, sw, imagesToMerge[0].type());
	int wOffset = 0;

	int i = 0;
	for( std::vector<cv::Mat>::iterator it =  imagesToMerge.begin(); it < imagesToMerge.end(); it++ )
	{
		int hoffset = (i % 2 ) * verticalDisplacement;
		if(it->cols == 0)
			continue;
		cv::Rect roi = cv::Rect(wOffset, hoffset, it->cols, it->rows);
		mergedImage(roi) += *it;
		wOffset += it->cols + spacing;
		if(imagesCenters != NULL)
			imagesCenters->push_back( cv::Point(roi.x + roi.width / 2, roi.y + roi.height / 2) );
		i++;
	}

	return mergedImage;
}

/**
 * Evaluates one image
 *
 * @param sourceImage
 * @param alphabet
 * @param letter
 */
void SkewEvaluator::evaluateMat( cv::Mat& sourceImage, const std::string& alphabet, const std::string& letter, size_t faceIndex )
{
	//generate modifications
	std::vector<SkewDef> distortions;

	cv::Mat negative = ~sourceImage;
	generateDistortions(negative, distortions);

#ifdef DO_PARALLEL
	omp_lock_t lock;
	omp_init_lock(&lock);
#endif

	for(size_t j = 0; j < distortions.size(); j++)
	{
		SkewDef& def = distortions[j];
		
		#pragma omp parallel for
		for(int i = 0; i < (int) detectors.size(); i++ )
		{
			cv::Mat debugImage;
			cv::Mat workImage = def.image.clone();

			double detectedAngle = detectors[i]->detectSkew( workImage, 0, &debugImage );
			double angleDiff = detectedAngle - def.skewAngle;
#ifdef DO_PARALLEL
			omp_set_lock(&lock);
#endif
			results.push_back( EvaluationResult(angleDiff, alphabet, letter, i, def.imageId, faceIndex) );

#ifdef DO_PARALLEL
			omp_unset_lock(&lock);
#endif
			//for correct angles, do not write image
			//if( fabs(angleDiff) < ANGLE_MIN)
				//continue;
			
			if (writeImages)
			{
				//write image to output directory structure
				std::string detectorDir = this->outputDirectory;
				detectorDir += "/" + this->detectorNames[i];
				std::string alphabetDir = detectorDir;
				alphabetDir += "/" + alphabet;
				IOUtils::CreateDir( alphabetDir );
				std::string letterDir = alphabetDir;
				letterDir += "/" + letter;
				IOUtils::CreateDir( letterDir );

				std::ostringstream os;
				os << letterDir << "/" << def.imageId << ".png";

				//cut display image
				std::vector<std::vector<cv::Point> > contours;
				std::vector<cv::Vec4i> hierarchy;
				cv::Mat temp = def.image.clone();
				findContours( temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
				if (contours.size() == 0) continue;
				ContourSkewDetector::getBigestContour(contours, hierarchy);
				cv::Rect r = cv::boundingRect(contours[0]);
				r.x=r.x-10;
				r.x = MAX(r.x, 0);
				r.y=r.y-10;
				r.y = MAX(r.y, 0);
				r.height=r.height+20;
				r.height = MIN(r.height, temp.rows - r.y - 1);
				r.width=r.width+20;
				r.width = MIN(r.width, temp.cols - r.x - 1);

				cv::Mat draw;
				cv::cvtColor( ~def.image, draw, cv::COLOR_GRAY2BGR);
				draw = draw(r);
				if(  debugImage.cols == workImage.cols && debugImage.rows == workImage.rows)
					debugImage = debugImage(r);

				//create display image
				cv::Point origin = cv::Point( draw.cols / 2.0, 0 );
				cv::Point end = cv::Point( origin.x + draw.rows * cos(detectedAngle + M_PI / 2.0),  origin.y + draw.rows * sin(detectedAngle + M_PI / 2.0));

				cv::line( draw, origin, end, cv::Scalar(0, 0, 255), 1 );
				end = cv::Point( origin.x + debugImage.rows * cos(def.skewAngle + M_PI / 2.0),  origin.y + debugImage.rows * sin(def.skewAngle + M_PI / 2.0));
				cv::line( draw, origin, end, cv::Scalar(0, 255, 0), 1 );

				std::vector<cv::Mat> toMerge;
				toMerge.push_back(draw);
				toMerge.push_back(debugImage);
				cv::Mat dispImage = mergeHorizontal(toMerge, 1, 0, NULL);

				cv::imwrite( os.str(), dispImage );

				if( debug )
				{
					cv::imshow(detectorNames[i], dispImage);
				}
			}
		}

		if( debug )
		{
			int key = cv::waitKey(0);
			if(key == 'w')
			{
				imwrite("/tmp/debugImage.png", def.image);
			}
		}
	}
#ifdef DO_PARALLEL
	omp_destroy_lock(&lock);
#endif
}

/**
 * Registers detector
 *
 * @param detector
 * @param detectorName
 */
void SkewEvaluator::registerDetector(cv::Ptr<SkewDetector> detector,
		const std::string detectorName)
{
	detectors.push_back(detector);
	detectorNames.push_back(detectorName);

	std::string detectorDir = outputDirectory;
	detectorDir += "/" + detectorName;
	IOUtils::CreateDir( detectorDir );
	TemplateUtils::CopyDetectorTemplates( ".", detectorDir );

}

bool sortResultsByCorrectClsCount(const AcumResult& o1, const AcumResult& o2)
{
	return o1.correctClassCont > o2.correctClassCont;
}

void SkewEvaluator::writeResults()
{
	std::vector<AcumResult> classMap;
	classMap.resize(detectors.size());
	std::map<int, std::map<std::string, std::map<std::string, AcumResult> > > resMap;
	std::map<int, std::map<std::string, AcumResult> > alphabetMap;
	std::map<std::string, bool> letters;
	
	std::map<int, double> sumCorrectAlphabetPercent;
	std::map<int, double> sumCorrectLetterPercent;

	if(results.size() == 0)
	{
		//TODO write somethin to out html
		return;
	}

	for(size_t i = 0; i < results.size(); i++)
	{
		classMap[ results[i].classificator ].count++;
		classMap[ results[i].classificator ].classIndex = results[i].classificator;
		classMap[ results[i].classificator ].sumDiff = classMap[ results[i].classificator ].sumDiff + results[i].angleDiff*results[i].angleDiff;

		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].classIndex = results[i].classificator;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].count++;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff = resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff + results[i].angleDiff*results[i].angleDiff;

		alphabetMap[ results[i].classificator ][ results[i].alphabet ].count++;
		alphabetMap[ results[i].classificator ][ results[i].alphabet ].classIndex = results[i].classificator;

		letters[ results[i].letter ] = true;
		if( fabs(results[i].angleDiff) < ANGLE_TOLERANCE )
		{
			classMap[ results[i].classificator ].correctClassCont++;
			resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].correctClassCont++;
			alphabetMap[ results[i].classificator ][ results[i].alphabet ].correctClassCont++;
		}
	}

	std::fstream report_overview;
	report_overview.open ( (outputDirectory+ "/index.htm").c_str(), std::fstream::out | std::fstream::app );
	report_overview << "\t<table>\n" << "\t\t<tr>\n";
	report_overview << "\t\t\t<th rowspan=\"2\">Detector</th>\n";

	std::vector<double> jsonData_correct, jsonData_deviation;

	std::string subtitle = "";

	for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[0].begin(); it != resMap[0].end(); it++)
	{
		report_overview << "\t\t\t<th colspan=\"5\">" << it->first << "</th>\n";
		subtitle = subtitle + "\t\t\t<th>Total</th>\n" + "\t\t\t<th>Correct</th>\n" + "\t\t\t<th>% Correct</th>\n" + "\t\t\t<th>StdDev</th>\n" + "\t\t\t<th>avgL%</th>\n";
	}

	report_overview << "\t\t\t<th colspan=\"6\">Sum</th>\n";
	report_overview << "\t\t</tr>\n";
	
	report_overview << "\t\t<tr>\n" << subtitle << "\t\t\t<th>Total</th>\n" << "\t\t\t<th>Correct</th>\n" << "\t\t\t<th>% Correct</th>\n" << "\t\t\t<th>StdDev</th>\n" << "\t\t\t<th>avgL%</th>\n" << "\t\t\t<th>avgA%</th>\n" << "\t\t</tr>\n";

	std::sort( classMap.begin(), classMap.end(), &sortResultsByCorrectClsCount );

	for(size_t i = 0; i < classMap.size(); i++)
	{
		std::ofstream json_data;
		std::stringstream json_incorrect;

		json_data.open ( (outputDirectory+"/"+detectorNames[classMap[i].classIndex]+"/json_data.js" ).c_str() );
		json_data << "var json = {\n" << "\t\"children\": [\n";
		
		report_overview << "\t\t<tr>\n";
		
		report_overview << "\t\t\t<td><a href=\"" << detectorNames[classMap[i].classIndex] << "/index.htm\" title=\"" << detectorNames[classMap[i].classIndex] << "\">" << detectorNames[classMap[i].classIndex] << "</a></td>\n";
		
		json_data << "\t\t{\n" << "\t\t\t\"children\": [\n";
		json_incorrect << "\t\t{\n" << "\t\t\t\"children\": [\n";


		std::fstream report_detector;
		report_detector.open ( (outputDirectory+"/"+detectorNames[classMap[i].classIndex]+"/index.htm" ).c_str(), std::fstream::out | std::fstream::app );
		report_detector << "\t<table>\n" << "\t\t<tr>\n";
		subtitle = "";

		for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[classMap[i].classIndex].begin(); it != resMap[classMap[i].classIndex].end(); it++)
		{
			report_detector << "\t\t\t<th colspan=\"5\">" << it->first << "</th>\n";
			subtitle = subtitle + "\t\t\t<th>Total</th>\n" + "\t\t\t<th>Correct</th>\n" + "\t\t\t<th>% Correct</th>\n" + "\t\t\t<th>Variance</th>\n" + "\t\t\t<th>avg % Correct Letters</th>\n";
		}

		report_detector << "\t\t\t<th colspan=\"6\">Sum</th>\n";
		report_detector << "\t\t</tr>\n";
		report_detector << "\t\t<tr>\n" << subtitle << "\t\t\t<th>Total</th>\n" << "\t\t\t<th>Correct</th>\n" << "\t\t\t<th>% Correct</th>\n" << "\t\t\t<th>Variance</th>\n" << "\t\t\t<th>avg % Correct Letters</th>\n" << "\t\t\t<th>avg % Correct Alphabet</th>\n" << "\t\t</tr>\n";
		report_detector << "\t\t<tr>\n";

		int total = 0;
		int correct = 0;
		double variance = 0.0;
		int alphabetIndex = 0;
		sumCorrectAlphabetPercent[classMap[i].classIndex] = 0.0;
		sumCorrectLetterPercent[classMap[i].classIndex] = 0.0;
		int letterTotal = 0;

		for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[classMap[i].classIndex].begin(); it != resMap[classMap[i].classIndex].end(); it++)
		{
			json_data << "\t\t\t\t{\n" << "\t\t\t\t\t\"children\": [\n";
			json_incorrect << "\t\t\t\t{\n" << "\t\t\t\t\t\"children\": [\n";
			int alphabetTotal = 0;
			int alphabetCorrect = 0;
			double alphabetVariance = 0.0;
			double sumCorrectPercent = 0.0;
			int letterIndex = 0;
			for(std::map<std::string, AcumResult>::iterator iterator = it->second.begin(); iterator != it->second.end(); iterator++)
			{
				alphabetTotal = alphabetTotal + iterator->second.count;
				alphabetCorrect = alphabetCorrect + iterator->second.correctClassCont;
				alphabetVariance = alphabetVariance + iterator->second.sumDiff;
				letterIndex++;
				sumCorrectPercent = sumCorrectPercent + double(iterator->second.correctClassCont)/double(iterator->second.count)*100;
			}
			total = total + alphabetTotal;
			correct = correct + alphabetCorrect;
			variance = variance + alphabetVariance;

			sumCorrectAlphabetPercent[classMap[i].classIndex] = sumCorrectAlphabetPercent[classMap[i].classIndex] + double(alphabetCorrect)/double(alphabetTotal)*100;
			sumCorrectLetterPercent[classMap[i].classIndex] = sumCorrectLetterPercent[classMap[i].classIndex] + sumCorrectPercent;

			letterTotal = letterTotal + it->second.size();

			report_overview << std::fixed << std::setprecision(2) << "\t\t\t<td>" << alphabetTotal << "</td>\n" << "\t\t\t<td>" << alphabetCorrect << "</td>\n" << "\t\t\t<td>" << double(alphabetCorrect)/double(alphabetTotal)*100 << "</td>\n" << "\t\t\t<td>" << alphabetVariance << "</td>\n" << "\t\t\t<td>" << sumCorrectPercent/double(it->second.size()) << "</td>\n";
			report_detector << std::fixed << std::setprecision(2)<< "\t\t\t<td>" << alphabetTotal << "</td>\n" << "\t\t\t<td>" << alphabetCorrect << "</td>\n" << "\t\t\t<td>" << double(alphabetCorrect)/double(alphabetTotal)*100 << "</td>\n" << "\t\t\t<td>" << alphabetVariance << "</td>\n" << "\t\t\t<td>" << sumCorrectPercent/double(it->second.size()) << "</td>\n";
			json_data << "\t\t\t\t\t],\n" << "\t\t\t\t\t\"data\": {\n";
			json_data << "\t\t\t\t\t\t\"$angularWidth\": " << double(alphabetCorrect)/double(alphabetTotal)*100*it->second.size() << ",\n";
			json_data << "\t\t\t\t\t\t\"index\": " << alphabetIndex << ",\n";
			json_data << "\t\t\t\t\t\t\"correct\": " << alphabetCorrect << ",\n";
			json_data << "\t\t\t\t\t\t\"count\": " << alphabetTotal << ",\n";
			json_data << "\t\t\t\t\t\t\"variance\": " << alphabetVariance << ",\n";
			json_data << "\t\t\t\t\t\t\"percent\": " << double(alphabetCorrect)/double(alphabetTotal)*100 << ",\n";
			json_data << "\t\t\t\t\t\t\"$color\": \"#00FF55\"\n";
			json_data << "\t\t\t\t\t},\n" << "\t\t\t\t\t\"id\": \"Correct_" << it->first << "\",\n" << "\t\t\t\t\t\"name\": \"" << it->first << "\"\n" << "\t\t\t\t},\n";

			json_incorrect << "\t\t\t\t\t],\n" << "\t\t\t\t\t\"data\": {\n";
			json_incorrect << "\t\t\t\t\t\t\"$angularWidth\": " << (1.0-(double(alphabetCorrect)/double(alphabetTotal)))*100*it->second.size() << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"index\": " << alphabetIndex << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"correct\": " << alphabetCorrect << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"count\": " << alphabetTotal << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"variance\": " << alphabetVariance << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"percent\": " << 100.0-(double(alphabetCorrect)/double(alphabetTotal)*100) << ",\n";
			json_incorrect << "\t\t\t\t\t\t\"$color\": \"#FF0055\"\n";
			json_incorrect << "\t\t\t\t\t},\n" << "\t\t\t\t\t\"id\": \"Incorrect_" << it->first << "\",\n" << "\t\t\t\t\t\"name\": \"" << it->first << "\"\n" << "\t\t\t\t},\n";
			alphabetIndex++;
		}
		json_data << "\t\t\t],\n" << "\t\t\t\"data\": {\n";
		json_data << "\t\t\t\t\"$angularWidth\": " << letters.size()*double(correct)/double(total)*100 << ",\n";
		json_data << "\t\t\t\t\"percent\": " << double(correct)/double(total)*100 << ",\n";
		json_data << "\t\t\t\t\"$color\": \"#00FF00\"\n";
		json_data << "\t\t\t},\n" << "\t\t\t\"id\": \"Correct\",\n" << "\t\t\t\"name\": \"Correct\"\n" << "\t\t},\n";

		json_incorrect << "\t\t\t],\n" << "\t\t\t\"data\": {\n";
		json_incorrect << "\t\t\t\t\"$angularWidth\": " << letters.size()*(100.0-(double(correct)/double(total)*100)) << ",\n";
		json_incorrect << "\t\t\t\t\"percent\": " << 100.0-(double(correct)/double(total)*100) << ",\n";
		json_incorrect << "\t\t\t\t\"$color\": \"#FF0000\"\n";
		json_incorrect << "\t\t\t},\n" << "\t\t\t\"id\": \"Incorrect\",\n" << "\t\t\t\"name\": \"Incorrect\"\n" << "\t\t}\n";

		
		json_data << json_incorrect.str();

		json_data << "\t],\n" << "\t\"data\": {\n";
		json_data << "\t\t\"$type\": \"none\"\n";
		json_data << "\t},\n" << "\t\"id\": \"" << detectorNames[classMap[i].classIndex] << "\",\n" << "\t\"name\": \"" << detectorNames[classMap[i].classIndex] << "\"\n}";

		json_data.close();

		sumCorrectAlphabetPercent[classMap[i].classIndex] = sumCorrectAlphabetPercent[classMap[i].classIndex]/double(alphabetIndex);
		sumCorrectLetterPercent[classMap[i].classIndex] = sumCorrectLetterPercent[classMap[i].classIndex]/double(letterTotal);

		report_overview << std::fixed << std::setprecision(2) << "\t\t\t<td>" << total << "</td>\n" << "\t\t\t<td>" << correct << "</td>\n" << "\t\t\t<td>" << double(correct)/double(total)*100 << "</td>\n" << "\t\t\t<td>" << variance << "</td>\n" << "\t\t\t<td>" << sumCorrectLetterPercent[classMap[i].classIndex] << "</td>\n" << "\t\t\t<td>" << sumCorrectAlphabetPercent[classMap[i].classIndex] << "</td>\n" << "\t\t</tr>\n";

		report_detector << std::fixed << std::setprecision(2) << "\t\t\t<td>" << total << "</td>\n" << "\t\t\t<td>" << correct << "</td>\n" << "\t\t\t<td>" << double(correct)/double(total)*100 << "</td>\n" << "\t\t\t<td>" << variance << "</td>\n" << "\t\t\t<td>" << sumCorrectLetterPercent[classMap[i].classIndex] << "</td>\n" << "\t\t\t<td>" << sumCorrectAlphabetPercent[classMap[i].classIndex] << "</td>\n" << "\t\t</tr>\n";
		report_detector << "\t</table>\n";

		ResultsWriter::writeWorstDetectorResults( results,  classMap[i].classIndex, 100, report_detector, outputDirectory, detectorNames );

		report_detector << "</body>\n</html>";
		report_detector.close();
	}

	report_overview << "\t</table>\n";

	ResultsWriter::writeWorstDetectorResults( results,  -1, 100, report_overview, outputDirectory, detectorNames );

	report_overview << "</body>\n</html>";

	report_overview.close();

	std::ofstream overview_json;
	overview_json.open ( (outputDirectory+"/object_data.js" ).c_str() );

	overview_json << "series = [{\n";
	overview_json << "\tname: 'Correct Classifications',\n";
    overview_json << "\tcolor: '#4572A7',\n";
    overview_json << "\ttype: 'column',\n";
    overview_json << "\tdata: [";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i!=0) overview_json << ", ";
		overview_json << std::fixed << std::setprecision(2) << 100*double(classMap[i].correctClassCont)/double(classMap[i].count);
	}

	overview_json << "],\n";
    overview_json << "\ttooltip: {\n";
    overview_json << "\t\tvalueSuffix: ' %'\n";
    overview_json << "\t}\n";
	/*****************/
	overview_json << "}, {\n";
	overview_json << "\tname: 'avg Correct Letters',\n";
    overview_json << "\tcolor: '#42ca2f',\n";
    overview_json << "\ttype: 'column',\n";
    overview_json << "\tdata: [";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i!=0) overview_json << ", ";
		overview_json << std::fixed << std::setprecision(2) << sumCorrectLetterPercent[classMap[i].classIndex];
	}

    overview_json << "],\n";
    overview_json << "\ttooltip: {\n";
    overview_json << "\t\tvalueSuffix: ' %'\n";
    overview_json << "\t}\n";
	/***************/
	overview_json << "}, {\n";
	overview_json << "\tname: 'avg Correct Alphabet',\n";
    overview_json << "\tcolor: '#cf2229',\n";
    overview_json << "\ttype: 'column',\n";
    overview_json << "\tdata: [";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i!=0) overview_json << ", ";
		overview_json << std::fixed << std::setprecision(2) << sumCorrectAlphabetPercent[classMap[i].classIndex];
	}

    overview_json << "],\n";
    overview_json << "\ttooltip: {\n";
    overview_json << "\t\tvalueSuffix: ' %'\n";
    overview_json << "\t}\n";
	/***************/
	overview_json << "}, {\n";
	overview_json << "\tname: 'Standard Deviation',\n";
    overview_json << "\tcolor: '#89A54E',\n";
    overview_json << "\ttype: 'spline',\n";
	overview_json << "\tyAxis: 1,\n";
    overview_json << "\tdata: [";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i!=0) overview_json << ", ";
		overview_json << std::fixed << std::setprecision(2) << classMap[i].sumDiff;
	}

    overview_json << "],\n";
    overview_json << "\ttooltip: {\n";
    overview_json << "\t\tvalueSuffix: ''\n";
    overview_json << "\t}\n";
	/***************/
	overview_json << "}]\n";

	overview_json << "\n\n" << "categories = [ ";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		overview_json << "'" << detectorNames[classMap[i].classIndex] << "', ";
	}
	
	overview_json << "'Sum' ];";
	overview_json.close();
	/**************************************************************************/
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
			alphabet_json << ", \n";
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


	
	ResultsWriter::writeLettersResults( results, 100, outputDirectory, detectorNames, ANGLE_TOLERANCE );
}

/**
 * Generates skew image distortions
 *
 * | 1  angle |
 * | 0    0   |
 *
 * @param source
 * @param distortions
 */
void SkewEvaluator::generateDistortions(cv::Mat& source,
		std::vector<SkewDef>& distortions)
{
	int x;
	float y;
	for(x=-40;x<=40;x=x+10)
	{
		double angleRad = x * M_PI / 180;
		y= tan (angleRad);
		cv::Mat transformed;
		cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
		affineTransform.at<float>(0, 1) = y;
		cv::warpAffine(source, transformed, affineTransform, cv::Size(source.cols * 2, source.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

		distortions.push_back( SkewDef( - angleRad, transformed, nextImageId++) );
	}
}

} /* namespace cmp */
