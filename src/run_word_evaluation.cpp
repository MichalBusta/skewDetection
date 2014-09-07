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
#include "DiscreteVotingWordSkDet.h"

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
	/**
     //evaluator.registerDetector(new ThinProfileSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.005, false), "LF-0.005" );
     /** LR Longest Edge */
	/**
     for (double precision = 0.005; precision <= 0.4; precision += 0.005)
     {
     std::stringstream ignoreAngleToStr;
     ignoreAngleToStr << precision;
     evaluator.registerDetector(new VerticalDomSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, 1, precision), "L-"+ignoreAngleToStr.str() );
     }
     
     /**/

	cv::Ptr<BestGuessSKDetector> detector = new BestGuessSKDetector();
	std::vector<cv::Ptr<ContourWordSkewDetector> > detectors;
	for(double i = 0.1; i < 5; i+= 0.2)
	{
		detectors.push_back(new DiscreteVotingWordSkDet2(detector, i));
		std::stringstream st;
		st << "wd-" << i;
		evaluator.registerDetector(NULL, st.str(), st.str());
		//evaluator.detectorNames.push_back(st.str());
		//evaluator.detectorCaptions.push_back(st.str());

	}
    
	/*
     evaluator.registerDetector(new LRLongestEdge(), "LeftLongestEdge" );
     evaluator.registerDetector(new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.026, IGNORE_ANGLE, false), "RightLongestEdge" );
     
     evaluator.registerDetector(new LeftRightHullSkDet(), "LeftTop-BottomEdge" );
     
     evaluator.registerDetector(new BestGuessSKDetector(), "BestGuessSKDetector" );/**/
    
    
	evaluator.evaluateWords( argv[1], detectors );
    
	return 0;
}
