//
//  VotingWordSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 21/03/14.
//
//

#ifndef __SkewDetection__VotingWordSkDet__
#define __SkewDetection__VotingWordSkDet__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "WordSkewDetector.h"
#include <math.h>

namespace cmp {
    
    class VotingWordSkDet : public ContourWordSkewDetector
    {
    public:
        
        VotingWordSkDet(cv::Ptr<SkewDetector> detector);
        virtual ~VotingWordSkDet();
        
    protected:
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities);
    };
}
#endif /* defined(__SkewDetection__VotingWordSkDet__) */
