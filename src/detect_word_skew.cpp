//
//  detect_word_skew.cpp
//  SkewDetection
//
//  Created by David Helekal on 26/03/14.
//
//

#include "detect_word_skew.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "VotingWordSkDet.h"
#include "WordSkewDetector.h"
#include "SkewDetection.h"
#include "IOUtils.h"

using namespace cmp;

static void help()
{
	std::cout << "\nRuns skew detection \n Call:\n ./detect_skew input_direcoty [detectorNo]\n\n";
	std::cout << "	Available detectors:";
    
    std::cout << "		0 - Mock Skew Detector\n";
	std::cout << "		1 - Thin Profile Detector\n";
	std::cout << "		2 - Top-Bottom Centers Detector\n";
	std::cout << "		3 - Vertical Dominant Detector\n";
	std::cout << "		4 - Longest vertical edge Detector\n";
	std::cout << "      5 - Left-Right Detector\n";
	std::cout << "      6 - LR Longest Edge Detector\n";
    
    std::cout << "	Available WordDetectors:";
    
	std::cout << "		0 - VotingWordSkDet\n";
    
	std::cout << std::endl;
}

int main( int argc, char **argv)
{    
	if(argc < 2)
	{
		help();
		return -1;
	}
	int detector = 0;
    int wordDetector =0;
    double skew;
    std::vector<Blob> blobs;
    
	if( argc > 2 )
	{
        detector = atoi(argv[2]);
		wordDetector = atoi( argv[3] );
	}
    
    std::vector<cv::Ptr<SkewDetector> > detectors;
    detectors.push_back( new MockSkewDetector() );
	detectors.push_back( new ThinProfileSkDet() );
	detectors.push_back( new CentersSkDet() );
	detectors.push_back( new VerticalDomSkDet() );
	detectors.push_back( new LongestEdgeSkDetector() );
	detectors.push_back( new LeftRightHullSkDet() );
	detectors.push_back( new LRLongestEdge() );
	detectors.push_back( new LRLongestEdge(CV_CHAIN_APPROX_TC89_KCOS, 0.026, IGNORE_ANGLE, false) );
    
    if(detector > detectors.size())
	{
		CV_Error( CV_StsBadArg, "Unsupported detector number" );
	}
    
	std::vector<cv::Ptr<ContourWordSkewDetector> > WordSkewDetectors;
	WordSkewDetectors.push_back(new VotingWordSkDet(detectors[detector]));
    
    if(wordDetector > WordSkewDetectors.size())
    {
		CV_Error( CV_StsBadArg, "Unsupported detector number" );
    }
    
    cv::Mat debugImg = cv::imread(argv[1]);
    std::string filePath = IOUtils::RemoveExtension(argv[1]);
    
    std::vector<std::string> files = IOUtils::GetFilesInDirectory(filePath, ".png", true);
    
	for(size_t i = 0; i < files.size(); i++)
	{
        cv::Mat tmp = cv::imread( files[i], cv::IMREAD_GRAYSCALE );
        cv::Mat img;
		cv::copyMakeBorder( tmp, img, 10, 10, 50, 50, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255) );
        //invert image, so ink is white
		cv::Mat imgRec = ~img;
        blobs.push_back(*new Blob(imgRec));
        
    }
    skew = WordSkewDetectors[wordDetector]->detectSkew(blobs, 0.0, &debugImg);
}
