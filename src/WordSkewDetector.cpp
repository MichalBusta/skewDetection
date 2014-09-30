//
//  WordSkewDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#include "WordSkewDetector.h"
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


namespace cmp {

WordSkewDetector::WordSkewDetector()
{

}
WordSkewDetector::~WordSkewDetector(){

}

ContourWordSkewDetector::ContourWordSkewDetector() : WordSkewDetector()
{

}

ContourWordSkewDetector::~ContourWordSkewDetector()
{

}



double ContourWordSkewDetector::detectSkew(std::vector<Blob>& blobs, double lineK, double& probability, cv::Mat* debugImage)
{

	std::vector<std::vector<cv::Point>* > contours;
	std::vector<std::vector<std::vector<cv::Point> > > contoursFound;
	contoursFound.resize(blobs.size());
	for (size_t i = 0; i< blobs.size(); i++)
	{

		/** find the contour */
		std::vector<cv::Vec4i> hierarchy;
		findContours( blobs[i].mask, contoursFound[i], hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
		contours.push_back( &contoursFound[i][0] );
	}
	return detectContoursSkew( contours, lineK, probability, debugImage );
}

}//namespace cmp
