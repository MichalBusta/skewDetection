/*
 * SkewEvaluator.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */
#include <opencv2/highgui/highgui.hpp>


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

	std::vector<std::string> facesDir = IOUtils::GetDirectoriesInDirectory(evalDir, "*", true);
	//for all alphabets
	for( size_t i = 0; i < facesDir.size(); i++ )
	{
		//process all defined letter
		std::string alphabet = IOUtils::Basename( facesDir[i] );

		std::vector<std::string> letterDirs = IOUtils::GetDirectoriesInDirectory(facesDir[i], "*", true);
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
	distortions.push_back( SkewDef(0, source) );
	//TODO implement!
}

} /* namespace cmp */
