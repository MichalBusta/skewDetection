/*
 * detect_skew.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

#include <iostream>

#include <opencv2/highgui/highgui.hpp>

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
	if( argc > 2 )
	{
		detector = atoi( argv[2] );
	}

	std::vector<cv::Ptr<SkewDetector> > detectors;
	detectors.push_back( new MockSkewDetector() );
	detectors.push_back( new ThinProfileSkDet() );
	detectors.push_back( new CentersSkDet() );

	if(detector > detectors.size())
	{
		CV_Error( CV_StsBadArg, "Unsupported detector number" );
	}

	std::vector<std::string> files = IOUtils::GetFilesInDirectory(argv[1], "*.png", true );

	for(size_t i = 0; i < files.size(); i++)
	{
		cv::Mat img = cv::imread( files[i], cv::IMREAD_GRAYSCALE );
		//invert image, so ink is white
		cv::Mat imgRec = ~img;
		cv::Mat debugImage;
		double skew = detectors[i]->detectSkew( imgRec,  0, &debugImage );

		std::cout << "Detected skew is: " << skew << std::endl;

		cv::Point origin = cv::Point( img.cols / 2.0, 0 );
		cv::Point end = cv::Point( origin.x + img.rows * cos(skew + M_PI / 2.0),  origin.y + img.rows * sin(skew + M_PI / 2.0));

		cv::Mat draw;
		cv::cvtColor( img, draw, cv::COLOR_GRAY2BGR);
		cv::line( draw, origin, end, cv::Scalar(0, 0, 255), 2 );

		cv::imshow("Original & Skew", draw);

		cv::imshow("Debug", debugImage);

		cv::waitKey(0);
	}


}

