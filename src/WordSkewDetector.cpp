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
        
    ContourWordSkewDetector::ContourWordSkewDetector(cv::Ptr<ContourSkewDetector> detector) : WordSkewDetector()
    {
        this->localDetector = detector;
    }
    
    ContourWordSkewDetector::~ContourWordSkewDetector()
    {
        
    }
    
    
    
    double ContourWordSkewDetector::detectSkew(std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage)
    {
        std::vector<double> probs;
        std::vector<double> angles;
        int noImg = blobs.size();
        for (int i = 0; i<noImg; i++)
        {
        	cv::Mat* tempDebugPtr = NULL;
        	cv::Mat tempDebug;
        	if( debugImage != NULL )
        		tempDebugPtr = &tempDebug;

        	angles.push_back(localDetector->detectSkew(blobs[i].mask, lineK, tempDebugPtr));
#ifdef VERBOSE
        	cv::imshow("temp", *tempDebugPtr);
        	cv::waitKey(0);
#endif
        	probs.push_back(localDetector->lastDetectionProbability);

        }
        double probability = 0;
        return computeAngle(angles, probs, probability);
    }

    double ContourWordSkewDetector::detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage)
    {
    	std::vector<double> probs;
    	std::vector<double> angles;
    	for (size_t i = 0; i < contours.size(); i++)
    	{
#ifdef VERBOSE
    		cv::Mat tempDebug;
    		debugImage = &tempDebug;
#endif
    		angles.push_back(localDetector->detectSkew(*contours[i], debugImage));
    		probs.push_back(localDetector->lastDetectionProbability);
#ifdef VERBOSE
        	cv::imshow("temp", tempDebug);
        	cv::waitKey(0);
#endif
    	}
    	double angle = computeAngle(angles, probs, probability);
#ifdef VERBOSE
    	std::cout << "Detected skew angle is: " << angle << " with prob.: " << probability << std::endl;
#endif
    	return angle;
    }
}
