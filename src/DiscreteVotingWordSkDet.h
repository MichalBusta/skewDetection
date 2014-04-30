//
//  DiscreteVotingWordSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#ifndef __SkewDetection__DiscreteVotingWordSkDet__
#define __SkewDetection__DiscreteVotingWordSkDet__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "WordSkewDetector.h"
#include <math.h>

namespace cmp {
    
    class DiscreteVotingWordSkDet : public ContourWordSkewDetector
    {
    public:
        
        DiscreteVotingWordSkDet(cv::Ptr<SkewDetector> detector);
        virtual ~DiscreteVotingWordSkDet();
        
    protected:
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, cv::Mat* debugImage);
    };
}

#endif /* defined(__SkewDetection__DiscreteVotingWordSkDet__) */
