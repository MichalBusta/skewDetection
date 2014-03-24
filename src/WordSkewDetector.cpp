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
    
    double CountourWordSkewDetector::detectSkew(std::vector<cv::Mat>& masks, double lineK, cv::Mat* debugImage)
    {
        std::vector<double> probs;
        
        int noImg = masks.size();
        for (int i = 0; i<noImg; i++)
        {
            angles.push_back(localDetector->detectSkew(masks[i], lineK, debugImage));
            probs.push_back(localDetector->lastDetectionProbability);
        }
        return computeAngle(angles, probs);
    }
}