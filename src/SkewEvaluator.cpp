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

#include "ImageFilter.h"
#include "SkewEvaluator.h"
#include "SkewDetection.h"
#include "IOUtils.h"
#include "ImageFilter.h"
#include "TemplateUtils.h"

#define ANGLE_TOLERANCE M_PI / 60.0


namespace cmp
{

SkewEvaluator::SkewEvaluator( std::string outputDirectory, bool debug ) : outputDirectory(outputDirectory), debug( debug ), nextImageId(0)
{
	registerDetector(new ThinProfileSkDet(), "ThinProfile" );
	registerDetector(new CentersSkDet(), "TopBottomCenters" );

	if(!IOUtils::PathExist(outputDirectory))
	{
		IOUtils::CreateDir( outputDirectory );
	}
	TemplateUtils::CopyIndexTemplates( ".", outputDirectory );
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
				copyMakeBorder( tmp, img, 10, 10, 50, 50, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255) );
				evaluateMat( img, alphabet, letterUnicode );
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
void SkewEvaluator::evaluateMat( cv::Mat& sourceImage, const std::string& alphabet, const std::string& letter )
{
	//generate modifications
	std::vector<SkewDef> distortions;

	ImageFilter imageFilter;
	sourceImage = imageFilter.filterImage( sourceImage );

	cv::Mat negative = ~sourceImage;
	generateDistortions(negative, distortions);

	for(size_t j = 0; j < distortions.size(); j++)
	{
		SkewDef& def = distortions[j];
		for(size_t i = 0; i < detectors.size(); i++ )
		{
			cv::Mat debugImage;
			cv::Mat workImage = def.image.clone();
			double detectedAngle = detectors[i]->detectSkew( workImage, 0, &debugImage );
			double angleDiff = detectedAngle - def.skewAngle;
			results.push_back( EvaluationResult(angleDiff, alphabet, letter, i, def.imageId) );

			//write image to output directory structure
			std::string detectorDir = this->outputDirectory;
			detectorDir += "/" + this->detectorNames[i];
			IOUtils::CreateDir( detectorDir );
			std::string alphabetDir = detectorDir;
			alphabetDir += "/" + alphabet;
			IOUtils::CreateDir( alphabetDir );
			std::string letterDir = alphabetDir;
			letterDir += "/" + letter;
			IOUtils::CreateDir( letterDir );

			std::ostringstream os;
			os << letterDir << "/" << def.imageId << ".png";

			//create display image
			cv::Point origin = cv::Point( debugImage.cols / 2.0, 0 );
			cv::Point end = cv::Point( origin.x + debugImage.rows * cos(detectedAngle + M_PI / 2.0),  origin.y + debugImage.rows * sin(detectedAngle + M_PI / 2.0));

			cv::Mat draw;
			cv::cvtColor( ~def.image, draw, cv::COLOR_GRAY2BGR);
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

		if( debug )
		{
			int key = cv::waitKey(0);
			if(key == 'w')
			{
				imwrite("/tmp/debugImage.png", def.image);
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
		const std::string detectorName)
{
	detectors.push_back(detector);
	detectorNames.push_back(detectorName);
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
	std::map<int, AcumResult> detectorMap;

	for(size_t i = 0; i < results.size(); i++)
	{
		classMap[ results[i].classificator ].count++;
		classMap[ results[i].classificator ].classIndex = results[i].classificator;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].classIndex = results[i].classificator;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].count++;
		resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff = resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].sumDiff + results[i].angleDiff*results[i].angleDiff;
		alphabetMap[ results[i].classificator ][ results[i].alphabet ].count++;
		detectorMap[ results[i].classificator ].count++;
		if( abs(results[i].angleDiff) < ANGLE_TOLERANCE )
		{
			classMap[ results[i].classificator ].correctClassCont++;
			resMap[ results[i].classificator ][ results[i].alphabet ][ results[i].letter ].correctClassCont++;
			alphabetMap[ results[i].classificator ][ results[i].alphabet ].correctClassCont++;
			detectorMap[ results[i].classificator ].correctClassCont++;
		}
	}
	
	//std::string outputDir = "/tmp";
	std::string outputDir = "C:/SkewDetection/reports";
	std::string htmlHeader1 = "<!DOCTYPE HTML>\n<html>\n<head>\n\t<title>";
	std::string htmlHeader2 = "</title>\n</head>\n<body>\n\t<table>\n";
	std::string htmlFooter = "\t</table>\n</body>\n</html>";

	std::ofstream report_overview;
	report_overview.open ( (outputDir+ "/report_overview.html").c_str() );
	report_overview << htmlHeader1 << "Report - Overview" << htmlHeader2;
	report_overview << "\t\t<tr>\n";
	report_overview << "\t\t\t<th rowspan=\"2\">Detector</th>\n";

	bool foo = report_overview.is_open();

	std::string subtitle = "";

	for(std::map<std::string, std::map<std::string, AcumResult> >::iterator it = resMap[0].begin(); it != resMap[0].end(); it++)
	{
		report_overview << "\t\t\t<th colspan=\"5\">" << it->first << "</th>\n";
		subtitle = subtitle + "\t\t\t<th>Total</th>\n" + "\t\t\t<th>Correct</th>\n" + "\t\t\t<th>% Correct</th>\n" + "\t\t\t<th>Variance</th>\n" + "\t\t\t<th>avg % Correct</th>\n";
	}

	report_overview << "\t\t\t<th colspan=\"6\">Sum</th>\n";
	report_overview << "\t\t</tr>\n";
	
	report_overview << "\t\t<tr>\n" << subtitle << "\t\t\t<th>Total</th>\n" << "\t\t\t<th>Correct</th>\n" << "\t\t\t<th>% Correct</th>\n" << "\t\t\t<th>Variance</th>\n" << "\t\t\t<th>avg % Correct Letters</th>\n" << "\t\t\t<th>avg % Correct Alphabet</th>\n" << "\t\t</tr>\n";


	std::sort( classMap.begin(), classMap.end(), &sortResultsByCorrectClsCount );

	for(size_t i = 0; i < classMap.size(); i++)
	{
		std::ofstream json_data;
		std::stringstream json_incorrect;

		json_data.open ( (outputDir+"/"+detectorNames[classMap[i].classIndex]+"/json_data.js" ).c_str() );// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		json_data << "var json = {\n" << "\t\"children\": [\n";
		
		report_overview << "\t\t<tr>\n";
		
		report_overview << "\t\t\t<td>" << detectorNames[classMap[i].classIndex] << "</td>\n";
		
		json_data << "\t\t{\n" << "\t\t\t\"children\": [\n";
		json_incorrect << "\t\t{\n" << "\t\t\t\"children\": [\n";

		int total = 0;
		int correct = 0;
		double variance = 0.0;
		int alphabetIndex = 0;
		double sumSumCorrectPercent = 0.0;
		double sumCorrectAlphabetPercent = 0.0;
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
				json_data << "\t\t\t\t\t\t{\n" << "\t\t\t\t\t\t\t\"children\": [\n";
				json_incorrect << "\t\t\t\t\t\t{\n" << "\t\t\t\t\t\t\t\"children\": [\n";
				alphabetTotal = alphabetTotal + iterator->second.count;
				alphabetCorrect = alphabetCorrect + iterator->second.correctClassCont;
				alphabetVariance = alphabetVariance + iterator->second.sumDiff;
				json_data << "\t\t\t\t\t\t\t],\n" << "\t\t\t\t\t\t\t\"data\": {\n";
				json_data << "\t\t\t\t\t\t\t\t\"$angularWidth\": " << (double(detectorMap[classMap[i].classIndex].correctClassCont)/double(detectorMap[classMap[i].classIndex].count))*(double(alphabetMap[classMap[i].classIndex][it->first].correctClassCont)/double(alphabetMap[classMap[i].classIndex][it->first].count))*(double(iterator->second.correctClassCont)/double(iterator->second.count))*100 << ",\n";
				json_data << "\t\t\t\t\t\t\t\t\"index\": " << letterIndex << ",\n";
				json_data << "\t\t\t\t\t\t\t\t\"correct\": " << iterator->second.correctClassCont << ",\n";
				json_data << "\t\t\t\t\t\t\t\t\"count\": " << iterator->second.count << ",\n";
				json_data << "\t\t\t\t\t\t\t\t\"variance\": " << iterator->second.sumDiff << ",\n"; 
				json_data << "\t\t\t\t\t\t\t\t\"percent\": " << double(iterator->second.correctClassCont)/double(iterator->second.count)*100 << ",\n"; 
				json_data << "\t\t\t\t\t\t\t\t\"$color\": \"#00FF55\"\n"; 
				json_data << "\t\t\t\t\t\t\t},\n" << "\t\t\t\t\t\t\t\"id\": \"Correct_" << it->first << "_" << iterator->first << "\",\n" << "\t\t\t\t\t\t\t\"name\": \"" << iterator->first << "\"\n" << "\t\t\t\t\t\t},\n";

				json_incorrect << "\t\t\t\t\t\t\t],\n" << "\t\t\t\t\t\t\t\"data\": {\n";
				json_incorrect << "\t\t\t\t\t\t\t\t\"$angularWidth\": " << (1.0-(double(detectorMap[classMap[i].classIndex].correctClassCont)/double(detectorMap[classMap[i].classIndex].count)))*(1.0-(double(alphabetMap[classMap[i].classIndex][it->first].correctClassCont)/double(alphabetMap[classMap[i].classIndex][it->first].count)))*(1.0-(double(iterator->second.correctClassCont)/double(iterator->second.count)))*100 << ",\n"; 
				json_incorrect << "\t\t\t\t\t\t\t\t\"index\": " << letterIndex << ",\n";
				json_incorrect << "\t\t\t\t\t\t\t\t\"correct\": " << iterator->second.correctClassCont << ",\n";
				json_incorrect << "\t\t\t\t\t\t\t\t\"count\": " << iterator->second.count << ",\n";
				json_incorrect << "\t\t\t\t\t\t\t\t\"variance\": " << iterator->second.sumDiff << ",\n"; 
				json_incorrect << "\t\t\t\t\t\t\t\t\"percent\": " << 100.0-(double(iterator->second.correctClassCont)/double(iterator->second.count)*100) << ",\n"; 
				json_incorrect << "\t\t\t\t\t\t\t\t\"$color\": \"#FF0055\"\n"; 
				json_incorrect << "\t\t\t\t\t\t\t},\n" << "\t\t\t\t\t\t\t\"id\": \"Incorrect_" << it->first << "_" << iterator->first << "\",\n" << "\t\t\t\t\t\t\t\"name\": \"" << iterator->first << "\"\n" << "\t\t\t\t\t\t},\n";
				letterIndex++;
				sumCorrectPercent = sumCorrectPercent + double(iterator->second.correctClassCont)/double(iterator->second.count)*100;
			}
			total = total + alphabetTotal;
			correct = correct + alphabetCorrect;
			variance = variance + alphabetVariance;
			
			sumSumCorrectPercent = sumSumCorrectPercent + sumCorrectPercent;
			sumCorrectAlphabetPercent = sumCorrectAlphabetPercent + double(alphabetCorrect)/double(alphabetTotal)*100;

			report_overview << std::fixed << std::setprecision(2) << "\t\t\t<td>" << alphabetTotal << "</td>\n" << "\t\t\t<td>" << alphabetCorrect << "</td>\n" << "\t\t\t<td>" << double(alphabetCorrect)/double(alphabetTotal)*100 << "</td>\n" << "\t\t\t<td>" << alphabetVariance << "</td>\n" << "\t\t\t<td>" << sumCorrectPercent/double(alphabetTotal) << "</td>\n";
			json_data << "\t\t\t\t\t],\n" << "\t\t\t\t\t\"data\": {\n";
			json_data << "\t\t\t\t\t\t\"$angularWidth\": " << (double(alphabetCorrect)/double(alphabetTotal))*(double(detectorMap[classMap[i].classIndex].correctClassCont)/double(detectorMap[classMap[i].classIndex].count))*100 << ",\n";
			json_data << "\t\t\t\t\t\t\"index\": " << alphabetIndex << ",\n";
			json_data << "\t\t\t\t\t\t\"correct\": " << alphabetCorrect << ",\n";
			json_data << "\t\t\t\t\t\t\"count\": " << alphabetTotal << ",\n";
			json_data << "\t\t\t\t\t\t\"variance\": " << alphabetVariance << ",\n";
			json_data << "\t\t\t\t\t\t\"percent\": " << double(alphabetCorrect)/double(alphabetTotal)*100 << ",\n";
			json_data << "\t\t\t\t\t\t\"$color\": \"#00FF55\"\n";
			json_data << "\t\t\t\t\t},\n" << "\t\t\t\t\t\"id\": \"Correct_" << it->first << "\",\n" << "\t\t\t\t\t\"name\": \"" << it->first << "\"\n" << "\t\t\t\t},\n";

			json_incorrect << "\t\t\t\t\t],\n" << "\t\t\t\t\t\"data\": {\n";
			json_incorrect << "\t\t\t\t\t\t\"$angularWidth\": " << (1.0-(double(alphabetCorrect)/double(alphabetTotal)))*(1.0-(double(detectorMap[classMap[i].classIndex].correctClassCont)/double(detectorMap[classMap[i].classIndex].count)))*100 << ",\n";
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
		json_data << "\t\t\t\t\"$angularWidth\": " << double(correct)/double(total)*100 << ",\n";
		json_data << "\t\t\t\t\"percent\": " << double(correct)/double(total)*100 << ",\n";
		json_data << "\t\t\t\t\"$color\": \"#00FF00\"\n";
		json_data << "\t\t\t},\n" << "\t\t\t\"id\": \"Correct\",\n" << "\t\t\t\"name\": \"Correct\"\n" << "\t\t},\n";

		json_incorrect << "\t\t\t],\n" << "\t\t\t\"data\": {\n";
		json_incorrect << "\t\t\t\t\"$angularWidth\": " << 100.0-(double(correct)/double(total)*100) << ",\n";
		json_incorrect << "\t\t\t\t\"percent\": " << 100.0-(double(correct)/double(total)*100) << ",\n";
		json_incorrect << "\t\t\t\t\"$color\": \"#FF0000\"\n";
		json_incorrect << "\t\t\t},\n" << "\t\t\t\"id\": \"Incorrect\",\n" << "\t\t\t\"name\": \"Incorrect\"\n" << "\t\t}\n";

		
		json_data << json_incorrect.str();

		json_data << "\t],\n" << "\t\"data\": {\n";
		json_data << "\t\t\"$type\": \"none\"\n";
		json_data << "\t},\n" << "\t\"id\": \"" << detectorNames[classMap[i].classIndex] << "\",\n" << "\t\"name\": \"" << detectorNames[classMap[i].classIndex] << "\"\n}";

		json_data.close();

		report_overview << std::fixed << std::setprecision(2) << "\t\t\t<td>" << total << "</td>\n" << "\t\t\t<td>" << correct << "</td>\n" << "\t\t\t<td>" << double(correct)/double(total)*100 << "</td>\n" << "\t\t\t<td>" << variance << "</td>\n" << "\t\t\t<td>" << sumSumCorrectPercent/double(total) << "</td>\n" << "\t\t\t<td>" << sumCorrectAlphabetPercent/double(alphabetIndex) << "</td>\n" << "\t\t</tr>\n";
	}

	report_overview << htmlFooter;
	report_overview.close();



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
