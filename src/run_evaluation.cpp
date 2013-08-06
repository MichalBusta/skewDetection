/*
* run_evaluation.cpp
*
*  Created on: Jul 10, 2013
*      Author: Michal Busta
*/

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <map>
#include <iomanip>
#include <sstream>

#include "SkewEvaluator.h"

using namespace cmp;

static void help()
{
	std::cout << "\nRuns evaluation \n Call:\n ./run_evaluation input_direcoty output_directory \n\n";
}

int main( int argc, char **argv)
{
	if( argc < 3)
	{
		help();
		return -1;
	}

	SkewEvaluator evaluator( argv[2], false, true );

	/** LR Longest Edge */
	/**/
	for (double precision = 0.005; precision <= 0.4; precision += 0.005)
	{
		std::stringstream ignoreAngleToStr;
		ignoreAngleToStr << precision;
		evaluator.registerDetector(new ThinProfileSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, precision), "L-"+ignoreAngleToStr.str() );
	}

	/**/
	/**
	evaluator.registerDetector(new ThinProfileSkDet(), "ThinProfile" );
	evaluator.registerDetector(new CentersSkDet(), "TopBottomCenters" );
	//evaluator.registerDetector(new LeftRightHullSkDet(), "RightHullLongest" );
	//evaluator.registerDetector(new LeftRightHullSkDet(CV_CHAIN_APPROX_NONE, 0, 0.1, false), "LeftHullLongest" );
	evaluator.registerDetector(new LongestEdgeSkDetector(), "LongestEdgeSkDetector" );
	evaluator.registerDetector(new VerticalDomSkDet(), "VerticalDomSkDet" );
	evaluator.registerDetector(new LRLongestEdge(), "LeftLongestEdge" );
	evaluator.registerDetector(new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.026, IGNORE_ANGLE, false), "RightLongestEdge" );

	evaluator.registerDetector(new LeftRightHullSkDet(), "LeftTop-BottomEdge" );

	evaluator.registerDetector(new BestGuessSKDetector(), "BestGuessSKDetector" );/**/


	evaluator.evaluate( argv[1] );

	return 0;
}
