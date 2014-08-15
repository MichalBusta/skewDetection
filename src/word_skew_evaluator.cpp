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
    std::map< std::string ,cv::Scalar> detectorIdentifierColors;
    
    //adding detectors
    detectors.push_back( new cmp::VerticalDomSkDet());
	weights.push_back(1.0);
    detectorNames.push_back("VerticalDomSkDet");
    detectorIdentifierColors["VerticalDomSkDet"] = cv::Scalar(90,200,220);
    
	detectors.push_back( new cmp::ThinProfileSkDet(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.1));
	weights.push_back(1.0);
    detectorNames.push_back("ThinProfileSkDet");
    detectorIdentifierColors["ThinProfileSkDet"] = cv::Scalar(210,105,30);
    
	detectors.push_back( new cmp::LongestEdgeSkDetector(CV_CHAIN_APPROX_TC89_KCOS, 0.028, IGNORE_ANGLE, 0.4));
    weights.push_back(1.0);
    detectorNames.push_back("LongestEdgeSkDetector");
    detectorIdentifierColors["LongestEdgeSkDetector"] = cv::Scalar(40,250,0);
    
	detectors.push_back( new cmp::LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.014, IGNORE_ANGLE, true) );
	weights.push_back(0.25);
    detectorNames.push_back("LRLongestEdge");
    detectorIdentifierColors["LRLongestEdge"] = cv::Scalar(0,50,200);
    
    detectors.push_back(new cmp::CentersSkDet());
    weights.push_back(0.25);
    detectorNames.push_back("CenterSkDet");
    detectorIdentifierColors["CenterSkDet"] = cv::Scalar(200,200,150);
    
    eval.addWordDetector(new cmp::DiscreteVotingWordSkDet(detectors,detectorNames,weights,detectorIdentifierColors), "DiscreteVoting");
    eval.run();
    
}
