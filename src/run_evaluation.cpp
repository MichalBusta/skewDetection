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
#include "SkewEvaluator.h"
#define PI 3.14159265

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

	SkewEvaluator evaluator( argv[2], false );

	evaluator.registerDetector(new ThinProfileSkDet(), "ThinProfile" );
	evaluator.registerDetector(new CentersSkDet(), "TopBottomCenters" );
	evaluator.registerDetector(new LeftRightHullSkDet(), "RightHullLongest" );
	evaluator.registerDetector(new LeftRightHullSkDet(CV_CHAIN_APPROX_NONE, 0, 0.1, false), "LeftHullLongest" );
	evaluator.registerDetector(new LongestEdgeSkDetector(), "LongestEdgeSkDetector" );
	evaluator.registerDetector(new VerticalDomSkDet(), "VerticalDomSkDet" );

	evaluator.evaluate( argv[1] );

	return 0;
}
