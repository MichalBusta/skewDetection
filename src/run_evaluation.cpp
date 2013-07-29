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

	/**/
	std::map<int, std::string> approx;
	approx[1] = "NONE";
	/*approx[2] = "SIMPLE";
	approx[3] = "TC89_L1";
	approx[4] = "TC89_KCOS";/**/

	/*for (std::map<int, std::string>::iterator it = approx.begin(); it != approx.end(); it++)
	{
		//it->first == approx
		for (double epsilon = 0.014; epsilon <= 0.024; epsilon=epsilon+0.002)
		{
			std::stringstream epsilonToStr;
			epsilonToStr << std::fixed << std::setprecision(3) << epsilon;
			for (int bin_size = 1; bin_size <= 5; bin_size++)
			{
				std::stringstream bin_sizeToStr;
				bin_sizeToStr << bin_size;
				for (int sigma = 1; sigma <= 3; sigma++)
				{
					std::stringstream sigmaToStr;
					sigmaToStr << std::fixed << std::setprecision(1) << sigma;
					for (int range = 2; range <= 11; range=range+3)
					{
						std::stringstream rangeToStr;
						rangeToStr << range;
						evaluator.registerDetector(new VerticalDomSkDet(it->first, epsilon, bin_size, sigma, range, 20), "VerticalDom-"+it->second+"-"+epsilonToStr.str()+"-"+bin_sizeToStr.str()+"-"+sigmaToStr.str()+"-"+rangeToStr.str() );
						rangeToStr.str( "" );
					}
					sigmaToStr.str( "" );
				}
				bin_sizeToStr.str( "" );
			}
			epsilonToStr.str( "" );
		}
	}
	/**/
	/** Centers */
	/**
	for (double epsilon = 0.014; epsilon <= 0.020; epsilon=epsilon+0.002)
	{
		std::stringstream epsilonToStr;
		epsilonToStr << std::fixed << std::setprecision(3) << epsilon;
		for (double precision = 0.06; precision <= 0.26; precision=precision+0.02)
		{
			std::stringstream precisionToStr;
			precisionToStr << precision;
			evaluator.registerDetector(new CentersSkDet(1, epsilon, precision), "Centers-NONE-"+epsilonToStr.str()+"-"+precisionToStr.str() );
			precisionToStr.str( "" );
		}
		epsilonToStr.str( "" );
	}
	/** LR Longest Edge */
	/*/
	for (double epsilon = 0.014; epsilon <= 0.028; epsilon=epsilon+0.002)
	{
		std::stringstream epsilonToStr;
		epsilonToStr << std::fixed << std::setprecision(3) << epsilon;
		for (int ignoreAngle = 9; ignoreAngle <= 21; ignoreAngle=ignoreAngle+3)
		{
			std::stringstream ignoreAngleToStr;
			ignoreAngleToStr << ignoreAngle;
			evaluator.registerDetector(new LRLongestEdge(1, epsilon, ignoreAngle, true), "LeftLongestEdge-NONE-"+epsilonToStr.str()+"-"+ignoreAngleToStr.str() );
			//evaluator.registerDetector(new LRLongestEdge(1, epsilon, ignoreAngle, false), "RightLongestEdge-NONE-"+epsilonToStr.str()+"-"+ignoreAngleToStr.str() );
			ignoreAngleToStr.str( "" );
		}
		epsilonToStr.str( "" );
	}
	/**/
	/**/
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
	
	/*for(double i = 0.1; i <= 1.0; i += 0.1)
	{
		for(double j = 0.1; j <= 1.0; j += 0.1)
		{
			for(double k = 0.1; k <= 1.0; k += 0.1)
			{
				for(double l = 0.1; l <= 1.0; l += 0.1)
				{
					std::ostringstream desc;
					desc << "BG-" << i << "-" << j << "-" << k << "-" << l << std::endl;
					BestGuessSKDetector* det = new  BestGuessSKDetector();
					det->weights[0] = i;
					det->weights[1] = j;
					det->weights[2] = k;
					det->weights[3] = l;
					evaluator.registerDetector( det, desc.str() );
				}
			}
		}
	}*/
	evaluator.evaluate( argv[1] );

	return 0;
}
