//
//  WordSkewDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#include "WordSkewDetector.h"
#include <opencv2/highgui/highgui.hpp>


namespace cmp {
    
    WordSkewDetector::WordSkewDetector()
    {

    }
    WordSkewDetector::~WordSkewDetector(){
        
    }
        
    ContourWordSkewDetector::ContourWordSkewDetector(cv::Ptr<SkewDetector> detector):WordSkewDetector()
    {
        this->localDetector = detector;

    }
    
    ContourWordSkewDetector::~ContourWordSkewDetector(){
        
    }
    
    
    
    double ContourWordSkewDetector::detectSkew(std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage)
    {
        std::vector<double> probs;
        
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
        return computeAngle(angles, probs);
    }
}
