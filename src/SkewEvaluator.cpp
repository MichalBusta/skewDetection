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

#include "SkewEvaluator.h"
#include "SkewDetection.h"
#include "IOUtils.h"

#define ANGLE_TOLERANCE M_PI / 60.0


namespace cmp
{

SkewEvaluator::SkewEvaluator( bool debug ) : debug( debug )
{
	registerDetector(new ThinProfileSkDet(), "ThinProfile" );
	registerDetector(new CentersSkDet(), "TopBottomCenters" );
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

			if( debug )
			{
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

	for(size_t i = 0; i < results.size(); i++)
	{
		classMap[ results[i].classificator ].count++;
		classMap[ results[i].classificator ].classIndex = results[i].classificator;
		if( abs(results[i].angleDiff) < ANGLE_TOLERANCE )
		{
			classMap[ results[i].classificator ].correctClassCont++;
		}
	}

	std::sort( classMap.begin(), classMap.end(), &sortResultsByCorrectClsCount );

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

		distortions.push_back( SkewDef( - angleRad, transformed) );
	}
}

} /* namespace cmp */
