/*
* word_skew_evaluator.cpp
*
*  Created on: May 5, 2014
*      Author: David Helekal
*/

#include <iostream>
#include "SkewDetection.h"
#include "WordSkewDetection.h"
#include "WordEvaluator.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


static void help()
{
	std::cout << "\nRuns evaluation \n Call:\n ./word_skew_evaluator input_direcory output_directory \n\n";
}

int main( int argc, char **argv)
{
	if( argc < 3)
	{
		help();
		return -1;
    }
    
    cmp::WordEvaluator eval(argv[2],argv[1],true);
    std::vector<cv::Ptr<cmp::SkewDetector> > detectors;
    std::vector<std::string> detectorNames;
    std::vector<double> weights;
    std::map< std::string ,cv::Scalar> detectorIdentifierColours;
    
    //adding detectors
    detectors.push_back( new cmp::VerticalDomSkDet());
	weights.push_back(1.0);
    detectorNames.push_back("VerticalDomSkDet");
    detectorIdentifierColours["VerticalDomSkDet"] = cv::Scalar();
    
	detectors.push_back( new cmp::ThinProfileSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.1));
	weights.push_back(1.0);
    detectorNames.push_back("ThinProfileSkDet");
    detectorIdentifierColours["ThinProfileSkDet"] = cv::Scalar();
    
	detectors.push_back( new cmp::LongestEdgeSkDetector(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.4));
    weights.push_back(1.0);
    detectorNames.push_back("LongestEdgeSkDetector");
    detectorIdentifierColours["LongestEdgeSkDetector"] = cv::Scalar();
    
	detectors.push_back( new cmp::LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.014, IGNORE_ANGLE, true) );
	weights.push_back(0.25);
    detectorNames.push_back("LRLongestEdge");
    detectorIdentifierColours["LRLongestEdge"] = cv::Scalar();
    
    eval.addWordDetector(new cmp::DiscreteVotingWordSkDet(detectors,detectorNames,weights,detectorIdentifierColours), "DiscreteVoting");
    eval.run();
    
}
