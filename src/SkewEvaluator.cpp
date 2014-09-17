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

bool doBestPossible = true;

namespace cmp
{

SkewEvaluator::SkewEvaluator( std::string outputDirectory, bool debug, bool writeImages, int distAngleMethod ) : outputDirectory(outputDirectory), debug( debug ), nextImageId(0), writeImages( writeImages ), distAngleMethod(distAngleMethod)
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

	//add best possible detector
	if(doBestPossible)
	{
		detectorNames.push_back("BestPossible");
		detectorCaptions.push_back("Best Possible");
	}

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
				copyMakeBorder( tmp, img, 20, 20, 80, 10, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255) );
				evaluateMat( img, alphabet, letterUnicode, k );
			}
		}
	}

	writeResults();
}

static inline void split(std::vector<std::string>& lst, const std::string& input, const std::string& separators, bool remove_empty = true)
{
	std::ostringstream word;
	for (size_t n = 0; n < input.size(); ++n)
	{
		if (std::string::npos == separators.find(input[n]))
			word << input[n];
		else
		{
			if (!word.str().empty() || !remove_empty)
				lst.push_back(word.str());
			word.str("");
		}
	}
	if (!word.str().empty() || !remove_empty)
		lst.push_back(word.str());
}

void readDefinitionFile(std::string& wordDir, std::vector<std::string>& letters){

	std::string dataFilePath;

	dataFilePath = wordDir+"/"+IOUtils::Basename(wordDir)+".txt";
	std::ifstream textdatafile(dataFilePath);

	std::string line;
	while (getline(textdatafile, line)) {

		//if the line contains the line header
		if (line=="LINES:")
			break;

		std::vector<std::string> splitStrings;
		split(splitStrings, line, " ");
		if(splitStrings.size() < 5)
			continue;

		letters.push_back(splitStrings[5]);


	}

}

/**
 * Runs evaluation of words on provided directory
 *
 * @param evalDir the evaluation directory
 */
void SkewEvaluator::evaluateWords( const std::string& evalDir, std::vector<cv::Ptr<ContourWordSkewDetector> >& wordSkewDetectors )
{
	std::vector<std::string> wordImages = IOUtils::GetFilesInDirectory(evalDir,"*.png", true);
	for(size_t i = 0; i < wordImages.size(); i++)
	{
		std::string wordDir = IOUtils::RemoveExtension(wordImages[i]);
		std::string baseName = IOUtils::Basename(wordDir);
		std::vector<std::string> letters;
		readDefinitionFile(wordDir, letters);
		std::vector<cv::Mat> letterImages;
		for(size_t j = 0; j < letters.size(); j++)
		{
			cv::Mat tempImg = cv::imread(wordDir+"/" + letters[j], CV_LOAD_IMAGE_GRAYSCALE);
			cv::copyMakeBorder(tempImg, tempImg, 20, 20, 80, 10, cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
			letterImages.push_back(tempImg);
		}
		std::cout << "Processing " <<  wordImages[i] << std::endl;
		evaluateWordsMat( letterImages, "Latin", baseName, i, wordSkewDetectors, wordImages[i]);


	}
	writeResults();
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
		double bestAngleDiff = M_PI;
		double bestAngle = M_PI;
		EvaluationResult bestResult;
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
			int isWorst = 0;
			if( fabs(angleDiff) >  fabs(def.skewAngle) && fabs(angleDiff) > ANGLE_TOLERANCE )
			{
				isWorst = 1;
			}
			results.push_back( EvaluationResult(angleDiff, alphabet, letter, i, def.imageId, faceIndex, isWorst, def.skewAngle, detectedAngle) );

			results.back().measure1 = detectors[i]->probMeasure1;
			results.back().measure2 = detectors[i]->probMeasure2;
			results.back().probability = detectors[i]->lastDetectionProbability;

			if( fabs(angleDiff) <  fabs(bestAngleDiff) )
			{
				bestAngleDiff = angleDiff;
				bestAngle = detectedAngle;
				bestResult = results.back();
			}

#ifdef DO_PARALLEL
			omp_unset_lock(&lock);
#endif
			//for correct angles, do not write image
			if( fabs(angleDiff) < ANGLE_MIN)
				continue;

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
		int isBestWorst = 0;
		if( fabs(bestAngleDiff) >  fabs(def.skewAngle) )
		{
			isBestWorst = 1;
		}
		if(doBestPossible)
		{
			results.push_back( EvaluationResult(bestAngleDiff, alphabet, letter, detectorNames.size() - 1, def.imageId, faceIndex, isBestWorst, def.skewAngle, bestAngle) );
			if( fabs(bestAngleDiff) > ANGLE_MIN)
				bestResults.push_back(bestResult);
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
 * Evaluates one image
 *
 * @param sourceImage
 * @param alphabet
 * @param letter
 */
void SkewEvaluator::evaluateWordsMat( std::vector<cv::Mat>& letterImages, const std::string& alphabet, const std::string& letter, size_t faceIndex,
		std::vector<cv::Ptr<ContourWordSkewDetector> >& wordSkewDetectors, std::string& wordImage )
{
	//generate modifications

	std::vector<std::vector<SkewDef> > wordDistortions;
	for(size_t i = 0; i < letterImages.size(); i++)
	{
		std::vector<SkewDef> distortions;

		cv::Mat negative = letterImages[i];
		generateDistortions(negative, distortions);
		wordDistortions.push_back(distortions);
	}

	for(size_t j = 0; j < wordDistortions[0].size(); j++)
	{
		EvaluationResult bestResult;

		std::vector<std::vector<cv::Point>* > contoursWord;
		std::vector<std::vector<cv::Point> > contoursWall;
		contoursWall.reserve(wordDistortions.size());
		for( size_t i = 0; i < wordDistortions.size(); i++ )
		{
			cv::Mat workImage = wordDistortions[i][j].image;
			std::vector<std::vector<cv::Point> > contours;
			std::vector<cv::Vec4i> hierarchy;

			/** find the contour */
			findContours( workImage.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

			if( contours.size() == 0)
				return;

			if( contours.size() > 1)
				ContourSkewDetector::getBigestContour( contours, hierarchy );
			contoursWall.push_back(contours[0]);
			contoursWord.push_back( &contoursWall.back() );
		}

		for(size_t k = 0; k < wordSkewDetectors.size(); k++)
		{
			cv::Ptr<ContourWordSkewDetector> wordSkewDetector = wordSkewDetectors[k];
			SkewDef def = wordDistortions[0][j];
			double lineK = 0;
			double probability;
			cv::Mat debugImage;
			double detectedAngle = wordSkewDetector->detectContoursSkew(contoursWord, 0, probability, &debugImage );
			double angleDiff = detectedAngle - def.skewAngle;

			int isWorst = 0;
			if( fabs(angleDiff) >  fabs(def.skewAngle) )
			{
				isWorst = 1;
			}
			results.push_back( EvaluationResult(angleDiff, alphabet, letter, k, def.imageId, faceIndex, isWorst, def.skewAngle, detectedAngle) );

			results.back().measure1 = 0;
			results.back().measure2 = 0;
			results.back().probability = probability;

			//for correct angles, do not write image
			if( fabs(angleDiff) < ANGLE_MIN)
				continue;

			if (writeImages)
			{
				cv::Mat src = cv::imread(wordImage);

				float y= tan (-def.skewAngle);
				cv::Mat transformed;
				cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
				affineTransform.at<float>(0, 1) = y;
				cv::warpAffine(src, transformed, affineTransform, cv::Size(src.cols, src.rows), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
				src = transformed;

				//draw the lines
				cv::Point origin = cv::Point( src.cols / 2.0, 0 );
				cv::Point end = cv::Point( origin.x + src.rows * cos(detectedAngle + M_PI / 2.0),  origin.y + src.rows * sin(detectedAngle + M_PI / 2.0));

				cv::line( src, origin, end, cv::Scalar(0, 0, 255), 1 );
				end = cv::Point( origin.x + debugImage.rows * cos(def.skewAngle + M_PI / 2.0),  origin.y + debugImage.rows * sin(def.skewAngle + M_PI / 2.0));
				cv::line( src, origin, end, cv::Scalar(0, 255, 0), 1 );


				std::vector<cv::Mat> toMerge;
				toMerge.push_back(src);
				toMerge.push_back(debugImage);
				cv::Mat dispImage = mergeHorizontal(toMerge, 1, 0, NULL, cv::Scalar(255, 255, 255));

				//cv::imshow("ts2", dispImage);
				//cv::waitKey(0);

				//write image to output directory structure
				std::string detectorDir = this->outputDirectory;
				detectorDir += "/" + detectorNames[k];
				IOUtils::CreateDir( detectorDir );
				std::string alphabetDir = detectorDir;
				alphabetDir += "/" + alphabet;
				IOUtils::CreateDir( alphabetDir );
				std::string letterDir = alphabetDir;
				letterDir += "/" + letter;
				IOUtils::CreateDir( letterDir );

				std::ostringstream os;
				os << letterDir << "/" << def.imageId << ".png";

				cv::imwrite( os.str(), dispImage );
			}
		}
	}

}

/**
 * Registers detector
 *
 * @param detector
 * @param detectorName
 */
void SkewEvaluator::registerDetector(cv::Ptr<SkewDetector> detector,
		const std::string& detectorName,  const std::string& detectorCaption)
{
	detectors.push_back(detector);
	detectorNames.push_back(detectorName);
	detectorCaptions.push_back(detectorCaption);

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
	classMap.resize(detectorNames.size());
	std::map<int, std::map<std::string, std::map<std::string, AcumResult> > > resMap;
	std::map<int, std::map<std::string, AcumResult> > alphabetMap;
	std::map<std::string, bool> letters;

	std::map<int, double> sumCorrectAlphabetPercent;
	std::map<int, double> sumCorrectLetterPercent;
	std::vector<MeasuresHist> detectorMeasures;

	std::map<std::string, int> alphabetNamesMap;
	int alphabetIdx = 0;

	if(results.size() == 0)
	{
		//TODO write somethin to out html
		return;
	}

	std::fstream out_csv;
	out_csv.open( (outputDirectory+ "/results.csv").c_str(), std::fstream::out );
	for(size_t i = 0; i < results.size(); i++)
	{
		classMap[ results[i].classificator ].count++;
		classMap[ results[i].classificator ].classIndex = results[i].classificator;
		classMap[ results[i].classificator ].sumDiff = classMap[ results[i].classificator ].sumDiff + results[i].angleDiff*results[i].angleDiff;
		classMap[ results[i].classificator ].isWorst += results[i].isWorst;

		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].classIndex = results[i].classificator;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].count++;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff = resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff + results[i].angleDiff*results[i].angleDiff;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].isWorst += results[i].isWorst;

		alphabetMap[ results[i].classificator ][ results[i].alphabet ].count++;
		alphabetMap[ results[i].classificator ][ results[i].alphabet ].classIndex = results[i].classificator;
		alphabetMap[ results[i].classificator ][ results[i].alphabet ].isWorst += results[i].isWorst;

		letters[ results[i].letter ] = true;
		if( fabs(results[i].angleDiff) <= ANGLE_TOLERANCE )
		{
			classMap[ results[i].classificator ].correctClassCont++;
			resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].correctClassCont++;
			alphabetMap[ results[i].classificator ][ results[i].alphabet ].correctClassCont++;
		}

		if(alphabetNamesMap.find(results[i].alphabet) == alphabetNamesMap.end())
		{
			alphabetNamesMap[results[i].alphabet] = alphabetIdx++;
		}
		int alphaIdx = alphabetNamesMap[results[i].alphabet];

		out_csv << results[i].classificator << "," << results[i].angleDiff << ","<< results[i].measure1 << "," << results[i].measure2 << "," << results[i].probability << "," << results[i].isWorst << ',' << results[i].letter << "," << results[i].faceIndex << ',' << results[i].estimAngle << ',' << results[i].gtAngle << "," << alphaIdx << std::endl;
	}
	out_csv.close();
	//Writing the report
	std::fstream report_overview;

	report_overview.open ( (outputDirectory+ "/index.htm").c_str(), std::fstream::out | std::fstream::app );
	report_overview << std::fixed << std::setprecision(2);
	std::stringstream table_overview;
	table_overview << std::fixed << std::setprecision(2);
	table_overview << "<div id=\"container2\"></div>";
	table_overview << "\t<table id=\"detectors_overview\">\n" << "\t\t<tr>\n";
	table_overview << "\t\t\t<th rowspan=\"2\">Detector</th>\n";

	report_overview << "\t<table id=\"detectors_overview_sum\">\n" << "\t\t<tr>\n";
	report_overview << "\t\t\t<th rowspan=\"2\">Detector</th>\n";
	report_overview << "\t\t\t<th class=\"border_left\" colspan=\"6\">Sum</th>\n";
	report_overview << "\t\t</tr>\n";
	report_overview << "\t\t<tr>\n" << "\t\t\t<th class=\"border_left\">Total</th>\n" << "\t\t\t<th>Correct</th>\n" << "\t\t\t<th>% Correct</th>\n" << "\t\t\t<th>StdDev</th>\n" << "\t\t\t<th>avgL%</th>\n" << "\t\t\t<th>avgA%</th>\n" << "\t\t</tr>\n";

	std::vector<double> jsonData_correct, jsonData_deviation;

	std::string subtitle = "";

	for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[0].begin(); it != resMap[0].end(); it++)
	{
		table_overview << "\t\t\t<th class=\"border_left\" colspan=\"5\"><a href=\"" << it->first << "/index.htm\">" << it->first << "</a></th>\n";
		subtitle = subtitle + "\t\t\t<th class=\"border_left\">Total</th>\n" + "\t\t\t<th>Correct</th>\n" + "\t\t\t<th>% Correct</th>\n" + "\t\t\t<th>StdDev</th>\n" + "\t\t\t<th>avgL%</th>\n";
	}

	table_overview << "\t\t</tr>\n";

	table_overview << "\t\t<tr>\n" << subtitle << "\t\t</tr>\n";

	std::sort( classMap.begin(), classMap.end(), &sortResultsByCorrectClsCount );

	for(size_t i = 0; i < classMap.size(); i++)
	{
		std::ofstream json_data;
		std::fstream json_data_measure;
		std::stringstream json_incorrect;
		json_incorrect << std::fixed << std::setprecision(2);

		json_data.open ( (outputDirectory+"/"+detectorNames[classMap[i].classIndex]+"/json_data.js" ).c_str() );
		std::string outMeasure = (outputDirectory+"/"+detectorNames[classMap[i].classIndex]+"/json_data_measure.js" ).c_str();
		json_data_measure.open ( outMeasure.c_str(), std::fstream::out );
		json_data << std::fixed << std::setprecision(2);
		json_data << "var json = {\n" << "\t\"children\": [\n";

		table_overview << "\t\t<tr>\n";
		report_overview << "\t\t<tr>\n";

		table_overview << "\t\t\t<td><a href=\"" << detectorNames[classMap[i].classIndex] << "/index.htm\" title=\"" << detectorNames[classMap[i].classIndex] << "\">" << detectorNames[classMap[i].classIndex] << "</a></td>\n";
		report_overview << "\t\t\t<td><a href=\"" << detectorNames[classMap[i].classIndex] << "/index.htm\" title=\"" << detectorNames[classMap[i].classIndex] << "\">" << detectorNames[classMap[i].classIndex] << "</a></td>\n";

		json_data << "\t\t{\n" << "\t\t\t\"children\": [\n";
		json_incorrect << "\t\t{\n" << "\t\t\t\"children\": [\n";


		std::fstream report_detector;
		report_detector.open ( (outputDirectory+"/"+detectorNames[classMap[i].classIndex]+"/index.htm" ).c_str(), std::fstream::out | std::fstream::app );
		report_detector << std::fixed << std::setprecision(2);
		report_detector << "\t<table id=\"detector_detail_overview\">\n" << "\t\t<tr>\n";
		subtitle = "";

		for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[classMap[i].classIndex].begin(); it != resMap[classMap[i].classIndex].end(); it++)
		{
			report_detector << "\t\t\t<th class=\"border_right\" colspan=\"5\">" << it->first << "</th>\n";
			subtitle = subtitle + "\t\t\t<th>Total</th>\n" + "\t\t\t<th>Correct</th>\n" + "\t\t\t<th>% Correct</th>\n" + "\t\t\t<th>Variance</th>\n" + "\t\t\t<th class=\"border_right\">avg % Correct Letters</th>\n";
		}

		report_detector << "\t\t\t<th colspan=\"6\">Sum</th>\n";
		report_detector << "\t\t</tr>\n";
		report_detector << "\t\t<tr>\n" << subtitle << "\t\t\t<th>Total</th>\n" << "\t\t\t<th>Correct</th>\n" << "\t\t\t<th>% Correct</th>\n" << "\t\t\t<th>Variance</th>\n" << "\t\t\t<th>avg % Correct Letters</th>\n" << "\t\t\t<th>avg % Correct Alphabet</th> <th>Worst</th><th>Worst%</th>\n" << "\t\t</tr>\n";
		report_detector << "\t\t<tr>\n";

		int total = 0;
		int correct = 0;
		double variance = 0.0;
		double worst = 0;
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
			int alphabetWorst = 0;
			int letterIndex = 0;
			for(std::map<std::string, AcumResult>::iterator iterator = it->second.begin(); iterator != it->second.end(); iterator++)
			{
				alphabetTotal = alphabetTotal + iterator->second.count;
				alphabetCorrect = alphabetCorrect + iterator->second.correctClassCont;
				alphabetVariance = alphabetVariance + iterator->second.sumDiff;
				alphabetWorst += iterator->second.isWorst;
				letterIndex++;
				sumCorrectPercent = sumCorrectPercent + double(iterator->second.correctClassCont)/double(iterator->second.count)*100;
			}
			total = total + alphabetTotal;
			correct = correct + alphabetCorrect;
			variance = variance + alphabetVariance;
			worst += alphabetWorst;
			if(alphabetVariance > 0)
			{
				alphabetVariance = sqrt(alphabetVariance / alphabetTotal);
			}

			sumCorrectAlphabetPercent[classMap[i].classIndex] = sumCorrectAlphabetPercent[classMap[i].classIndex] + double(alphabetCorrect)/double(alphabetTotal)*100;
			sumCorrectLetterPercent[classMap[i].classIndex] = sumCorrectLetterPercent[classMap[i].classIndex] + sumCorrectPercent;

			letterTotal = letterTotal + it->second.size();

			table_overview << std::fixed << std::setprecision(2) << "\t\t\t<td class=\"border_left\">" << alphabetTotal << "</td>\n" << "\t\t\t<td>" << alphabetCorrect << "</td>\n" << "\t\t\t<td>" << double(alphabetCorrect)/double(alphabetTotal)*100 << "</td>\n" << "\t\t\t<td>" << alphabetVariance << "</td>\n" << "\t\t\t<td>" << sumCorrectPercent/double(it->second.size()) << "</td>\n";
			report_detector << std::fixed << std::setprecision(2)<< "\t\t\t<td>" << alphabetTotal << "</td>\n" << "\t\t\t<td>" << alphabetCorrect << "</td>\n" << "\t\t\t<td>" << double(alphabetCorrect)/double(alphabetTotal)*100 << "</td>\n" << "\t\t\t<td>" << alphabetVariance << "</td>\n" << "\t\t\t<td class=\"border_right\">" << sumCorrectPercent/double(it->second.size()) << "</td>\n";
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

		if(variance > 0)
		{
			variance = sqrt(variance / total);
		}

		table_overview << "\t\t</tr>\n";
		report_overview << "\t\t\t<td class=\"border_left\">" << total << "</td>\n" << "\t\t\t<td>" << correct << "</td>\n" << "\t\t\t<td>" << double(correct)/double(total)*100 << "</td>\n" << "\t\t\t<td>" << variance << "</td>\n" << "\t\t\t<td>" << sumCorrectLetterPercent[classMap[i].classIndex] << "</td>\n" << "\t\t\t<td>" << sumCorrectAlphabetPercent[classMap[i].classIndex] << "</td>\n" << "\t\t</tr>\n";

		report_detector << "\t\t\t<td>" << total << "</td>\n" << "\t\t\t<td>" << correct << "</td>\n" << "\t\t\t<td>" << double(correct)/double(total)*100 << "</td>\n" << "\t\t\t<td>" << variance << "</td>\n" << "\t\t\t<td>" << sumCorrectLetterPercent[classMap[i].classIndex] << "</td>\n" << "\t\t\t<td>" << sumCorrectAlphabetPercent[classMap[i].classIndex] << "</td><td>" << worst << "</td><td>" << worst / (double) total << "</td>\n" << "\t\t</tr>\n";
		report_detector << "\t</table>\n";

		ResultsWriter::writeWorstDetectorResults( results,  classMap[i].classIndex, 100, report_detector, outputDirectory, detectorNames );

		report_detector << "</body>\n</html>";
		report_detector.close();

		MeasuresHist histValue = ResultsWriter::writeDetectorMeasure( results, json_data_measure, classMap[i].classIndex, detectorNames );
		detectorMeasures.push_back( histValue );
		json_data_measure.close();
	}

	table_overview << "\t</table>\n";

	report_overview << "\t</table>\n";

	report_overview << table_overview.str();

	report_overview << "<div class=\"preview\">\n";
	report_overview << "<h2>Results Preview</h2>\n";
	ResultsWriter::writeWorstDetectorResults( results,  -1, 100, report_overview, outputDirectory, detectorNames );
	ResultsWriter::writeBestResults( bestResults, 300, report_overview, detectorNames );
	report_overview << "</div>\n";

	ResultsWriter::writeDetectorMeasuresTable(detectorMeasures, report_overview, detectorNames);

	report_overview << "</body>\n</html>";

	report_overview.close();

	std::ofstream overview_json;
	overview_json.open ( (outputDirectory+"/object_data.js" ).c_str() );
	overview_json << std::fixed << std::setprecision(2);

	overview_json << "series = [{\n";
	overview_json << "\tname: 'Correct Classifications',\n";
	overview_json << "\tcolor: '#4572A7',\n";
	overview_json << "\ttype: 'column',\n";
	overview_json << "\tdata: [";

	std::fstream overview_csv;
	overview_csv.open( (outputDirectory+ "/overview.csv").c_str(), std::fstream::out );
	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i != 0) overview_csv << ",";
		overview_csv << detectorCaptions[classMap[i].classIndex];
	}
	overview_csv << std::endl;
	for(size_t i = 0; i < classMap.size(); i++)
	{
		if(i!=0){
			overview_json << ", ";
			overview_csv << ",";
		}
		overview_json << 100*double(classMap[i].correctClassCont)/double(classMap[i].count);
		overview_csv << 100*double(classMap[i].correctClassCont)/double(classMap[i].count);
	}
	overview_csv << std::endl;
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
		if(i!=0){
			overview_json << ", ";
			overview_csv << ",";
		}
		overview_json << sumCorrectLetterPercent[classMap[i].classIndex];
		overview_csv << sumCorrectLetterPercent[classMap[i].classIndex];
	}
	overview_csv << std::endl;
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
		if(i!=0)
		{
			overview_json << ", ";
			overview_csv << ",";
		}
		overview_json << sumCorrectAlphabetPercent[classMap[i].classIndex];
		overview_csv << sumCorrectAlphabetPercent[classMap[i].classIndex];
	}
	overview_csv << std::endl;
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
		if(i!=0)
		{
			overview_json << ", ";
			overview_csv << ",";
		}
		overview_json << classMap[i].sumDiff;
		overview_csv << classMap[i].sumDiff;
	}
	overview_csv.close();
	overview_json << "],\n";
	overview_json << "\ttooltip: {\n";
	overview_json << "\t\tvalueSuffix: ''\n";
	overview_json << "\t}\n";
	/***************/
	overview_json << "}]\n";

	overview_json << "\n\n" << "categories = [ ";

	for(size_t i = 0; i < classMap.size(); i++)
	{
		overview_json << "'" << detectorCaptions[classMap[i].classIndex] << "', ";
	}

	overview_json << "'Sum' ];";
	overview_json.close();
	/**************************************************************************/
	std::ofstream alphabet_json;
	alphabet_json.open ( (outputDirectory+"/alphabet_object_data.js" ).c_str() );
	alphabet_json << std::fixed << std::setprecision(2);

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
			alphabet_json << 100 * double(iterator->second.correctClassCont)/double(iterator->second.count);
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

	ResultsWriter::writeLettersResults( results, outputDirectory, detectorNames, ANGLE_TOLERANCE, ANGLE_MIN );

	/*
            EXPORTING RESULTS TO CSV
	 */

	//exporting detector correlation data
	std::vector<std::pair<int, int>> detPairs;
	int pairedDetectors;
	std::vector<std::vector<EvaluationResult> > detResults;
	detResults.resize(detectorNames.size());

	//create result vector for each detector

	for (int iter =0; iter<results.size(); iter++) {

		EvaluationResult& er = results[iter];
		assert(er.classificator < detResults.size());
		detResults[er.classificator].push_back(er);

	}
	//create detector pairs
	pairedDetectors = 0;

	for (int i = 0; i<classMap.size(); i++) {
		pairedDetectors++;
		for (int i2 = pairedDetectors; i2 <classMap.size(); i2++) {

			std::pair<int, int> tempPair;
			tempPair.first = i;
			tempPair.second = i2;
			detPairs.push_back(tempPair);
		}
	}

	//writing the table as a csv file

	for (int i=0; i<detPairs.size(); i++) {
		//open&create the file
		std::fstream correlationTable;
		std::ostringstream os;
		os << outputDirectory << "/correlationTable"<<"_" << detectorNames[detPairs[i].first] << "_" << detectorNames[detPairs[i].second] <<".csv";
		correlationTable.open(os.str().c_str(), std::fstream::out);
		//write column titles
		correlationTable << detectorNames[detPairs[i].first] <<"," <<detectorNames[detPairs[i].second]<< "\n";
		//writing the data
		for (int idx =0; idx < detResults[detPairs[i].first].size(); idx++) {

			correlationTable << fabs(detResults[detPairs[i].first][idx].angleDiff)<< ",";
			correlationTable << fabs(detResults[detPairs[i].second][idx].angleDiff)<< "\n";

		}

		correlationTable.close();
	}
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

	if(distAngleMethod == 0)
	{
		float angleD = -20;
		double angleRad = angleD * M_PI / 180;
		double y= tan (angleRad);
		cv::Mat transformed;
		cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
		affineTransform.at<float>(0, 1) = y;
		cv::warpAffine(source, transformed, affineTransform, cv::Size(source.cols * 2, source.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

		distortions.push_back( SkewDef( - angleRad, transformed, nextImageId++) );
		return;
	}
	if(distAngleMethod == 1)
	{
	int x;
	float y;
	float min = -40;
	float max = 40;
	for(int i = 0; i < 10; i++ )
	{
		float r = (float)rand() / (float)RAND_MAX;
		float angleD =  min + r * (max - min);


		double angleRad = angleD * M_PI / 180;
		y= tan (angleRad);
		cv::Mat transformed;
		cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
		affineTransform.at<float>(0, 1) = y;
		cv::warpAffine(source, transformed, affineTransform, cv::Size(source.cols * 2, source.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

		distortions.push_back( SkewDef( - angleRad, transformed, nextImageId++) );
	}
	}

	if(distAngleMethod == 2)
	{
		for(int x=-40;x<=40;x=x+10)
		{
			double angleRad = x * M_PI / 180;
			float y= tan (angleRad);
			cv::Mat transformed;
			cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
			affineTransform.at<float>(0, 1) = y;
			cv::warpAffine(source, transformed, affineTransform, cv::Size(source.cols * 2, source.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

			distortions.push_back( SkewDef( - angleRad, transformed, nextImageId++) );
		}
	}
}

} /* namespace cmp */
