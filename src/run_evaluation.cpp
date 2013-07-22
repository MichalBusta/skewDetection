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

	/*
	std::map<int, std::string> approx;
	approx[1] = "NONE";
	approx[2] = "SIMPLE";
	approx[3] = "TC89_L1";
	approx[4] = "TC89_KCOS";
	for (std::map<int, std::string>::iterator it = approx.begin(); it != approx.end(); it++)
	{
		//it->first == approx
		for (double epsilon = 0.008; epsilon <= 0.022; epsilon=epsilon+0.002)
		{
			std::stringstream epsilonToStr;
			epsilonToStr << std::fixed << std::setprecision(3) << epsilon;
			for (int ignoreAngle = 3; ignoreAngle <= 15; ignoreAngle = ignoreAngle + 3)
			{
				std::stringstream ignoreAngleToStr;
				ignoreAngleToStr << ignoreAngle;
				evaluator.registerDetector(new ThinProfileSkDet(it->first, epsilon, ignoreAngle), "ThinProfile-"+it->second+"-"+epsilonToStr.str()+"-"+ignoreAngleToStr.str() );
				ignoreAngleToStr.clear();
			}
			




			epsilonToStr.clear();
		}
	}
	*/
	evaluator.registerDetector(new ThinProfileSkDet(), "ThinProfile" );
	evaluator.registerDetector(new CentersSkDet(), "TopBottomCenters" );
	evaluator.registerDetector(new LeftRightHullSkDet(), "RightHullLongest" );
	evaluator.registerDetector(new LeftRightHullSkDet(CV_CHAIN_APPROX_NONE, 0, 0.1, false), "LeftHullLongest" );
	evaluator.registerDetector(new LongestEdgeSkDetector(), "LongestEdgeSkDetector" );
	evaluator.registerDetector(new VerticalDomSkDet(), "VerticalDomSkDet" );

	evaluator.evaluate( argv[1] );

	return 0;
}
