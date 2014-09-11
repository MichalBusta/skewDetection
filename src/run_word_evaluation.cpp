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
	detectors.push_back(new DiscreteVotingWordSkDet2(detector));
	evaluator.registerDetector( NULL, "CommonVoting", "Common Voting" );
	//evaluator.detectorNames.push_back("CommonVoting");
	//evaluator.detectorCaptions.push_back("Common Voting");
	/*
	for( double w1 = 0.1; w1 <= 1.5; w1+= 0.1 ){
		for( double w2 = 0.1; w2 <= 1; w2+= 0.1 ){
			std::ostringstream os;
			os << "BG-" << w1 << "-" << w2;

			std::vector<cv::Ptr<ContourSkewDetector> > detectorsw;
			std::vector<std::string> detectorNames;
			std::vector<double> weights;
			weights.push_back(1.0);
			detectorNames.push_back("VertDom");
			detectorsw.push_back(new VerticalDomSkDet());
			weights.push_back(1.0);
			detectorNames.push_back("VertCH");
			detectorsw.push_back(new VerticalDomSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.022, 3, 3, IGNORE_ANGLE, 3, true));
			weights.push_back(w1);
			detectorNames.push_back("Centers");
			detectorsw.push_back(new CentersSkDet());

			weights.push_back(w2);
			detectorNames.push_back("ThinProf");
			detectorsw.push_back( new ThinProfileSkDet());

			cv::Ptr<BestGuessSKDetector> det = new BestGuessSKDetector(detectorsw, weights, detectorNames);

			detectors.push_back(new DiscreteVotingWordSkDet2(det));
			evaluator.detectorNames.push_back(os.str());
			evaluator.detectorCaptions.push_back(os.str());
		}
	}
	*/
	/*

	for(double i = 0.1; i < 5; i+= 0.2)
	{
		detectors.push_back(new DiscreteVotingWordSkDet2(detector, i));
		std::stringstream st;
		st << "wd-" << i;
		evaluator.registerDetector(NULL, st.str(), st.str());
		//evaluator.detectorNames.push_back(st.str());
		//evaluator.detectorCaptions.push_back(st.str());

	}*/

    
    
	evaluator.evaluateWords( argv[1], detectors );
    
	return 0;
}
