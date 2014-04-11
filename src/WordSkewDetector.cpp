//
//  WordSkewDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#include "WordSkewDetector.h"

namespace cmp {
    
    WordSkewDetector::WordSkewDetector(cv::Ptr<SkewDetector> detector)
    {
        this->localDetector = detector;
    }
    
    ContourWordSkewDetector::ContourWordSkewDetector(cv::Ptr<SkewDetector> detector):WordSkewDetector(detector){
        
    }
    
    double ContourWordSkewDetector::detectSkew(std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage)
    {
        std::vector<double> probs;
        
        int noImg = blobs.size();
        for (int i = 0; i<noImg; i++){
            
            if(blobs[i].bBox.width != 0 && blobs[i].bBox.width != 0){
                cv::Mat tempDebug = debugImage->operator()(blobs[i].bBox);
                angles.push_back(localDetector->detectSkew(blobs[i].mask, lineK,&tempDebug));
                probs.push_back(localDetector->lastDetectionProbability);
            }
            else{
                angles.push_back(localDetector->detectSkew(blobs[i].mask, lineK));
                probs.push_back(localDetector->lastDetectionProbability);
            }

        }
        return computeAngle(angles, probs);
    }
}