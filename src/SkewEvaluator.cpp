/*
 * SkewEvaluator.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

#include "SkewEvaluator.h"
#include "IOUtils.h"


namespace cmp
{

SkewEvaluator::SkewEvaluator( )
{

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

	std::vector<std::string> alphabetDir = IOUtils::GetDirectoriesInDirectory(evalDir, "*", true);
	//for all alphabets
	for( size_t i = 0; i < alphabetDir.size(); i++ )
	{
		//process all defined letter
		std::string alphabet = IOUtils::Basename( alphabetDir[i] );

		std::vector<std::string> letterDirs = IOUtils::GetDirectoriesInDirectory(alphabetDir[i], "*", true);
		for(size_t j = 0; j < letterDirs.size(); j++)
		{
			const std::string& letterFile = letterDirs[j];
			cv::Mat img = cv::imread(letterFile, cv::IMREAD_GRAYSCALE);
			evaluateMat( img );
		}
	}
}

/**
 * Evaluates one image
 *
 * @param sourceImage
 */
void SkewEvaluator::evaluateMat(cv::Mat& sourceImage)
{

}

void SkewEvaluator::generateDistortions(cv::Mat& source,
		std::vector<SkewDef>& distortions)
{
	int x;
	float y;
	for(x=-60;x<=60;x=x+10)
	{
		double angleRad = x * M_PI / 180;
		y= tan (angleRad);
		cv::Mat transformed;
		cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
		affineTransform.at<float>(0, 1) = y;
		cv::warpAffine(source, transformed, affineTransform, cv::Size(source.cols * 2, source.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

		distortions.push_back( SkewDef(angleRad, transformed) );
	}
}

} /* namespace cmp */
