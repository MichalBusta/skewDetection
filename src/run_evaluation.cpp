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
     
    /* */
	evaluator.registerDetector(new ThinProfileSkDet(), "ThinProfile", "Thinnest Profile" );
	evaluator.registerDetector(new CentersSkDet(), "TopBottomCenter", "Symmetric Glyph" );
	//evaluator.registerDetector(new LeftRightHullSkDet(CV_CHAIN_APPROX_TC89_KCOS ,0.01, 0.2, true), "RightHullLongest" );
	//evaluator.registerDetector(new LeftRightHullSkDet(CV_CHAIN_APPROX_TC89_KCOS ,0.01, 0.2, false), "LeftHullLongest" );
	evaluator.registerDetector(new LongestEdgeSkDetector(), "LongestEdge", "Longest Edge" );
	evaluator.registerDetector(new VerticalDomSkDet(), "VerticalDom", "Vertical Dominant" );
	evaluator.registerDetector(new BestGuessSKDetector(), "BestGuess", "Non-Maximum Suppression" );

	evaluator.registerDetector(new VerticalDomSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.022, 3, 3, IGNORE_ANGLE, 3, true), "VertDomCH", "Vertical Dominant on Convex Hull" );
	//evaluator.registerDetector(new VerticalDomSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.022, 38, 3), "VerticalDom-38" );

	/*
	for( double w1 = 0.1; w1 <= 1; w1+= 0.1 ){
	for( double w2 = 0.1; w2 <= 1; w2+= 0.1 ){
		std::ostringstream os;
		os << "BG-" << w1 << "-" << w2;

		std::vector<cv::Ptr<ContourSkewDetector> > detectors;
		std::vector<std::string> detectorNames;
		std::vector<double> weights;
		weights.push_back(1.0);
		detectorNames.push_back("VertDom");
		detectors.push_back(new VerticalDomSkDet());
		weights.push_back(1.0);
		detectorNames.push_back("VertCH");
		detectors.push_back(new VerticalDomSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.022, 3, 3, IGNORE_ANGLE, 3, true));
		weights.push_back(w1);
		detectorNames.push_back("Centers");
		detectors.push_back(new CentersSkDet());

		weights.push_back(w2);
		detectorNames.push_back("ThinProf");
		detectors.push_back( new ThinProfileSkDet());

		evaluator.registerDetector(new BestGuessSKDetector(detectors, weights, detectorNames), os.str(), os.str() );
	}
	}*/
	/*
     evaluator.registerDetector(new LRLongestEdge(), "LeftLongestEdge" );
     evaluator.registerDetector(new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.026, IGNORE_ANGLE, false), "RightLongestEdge" );
     
     evaluator.registerDetector(new LeftRightHullSkDet(), "LeftTop-BottomEdge" );
     
     evaluator.registerDetector(new BestGuessSKDetector(), "BestGuessSKDetector" );/**/
    
    
	evaluator.evaluate( argv[1] );
    
	return 0;
}
