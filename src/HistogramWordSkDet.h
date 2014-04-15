//
//  HistogramWordSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#ifndef __SkewDetection__HistogramWordSkDet__
#define __SkewDetection__HistogramWordSkDet__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "WordSkewDetector.h"
#include <math.h>

namespace cmp {
    
    class HistogramWordSkDet : public ContourWordSkewDetector
    {
    public:
        
        HistogramWordSkDet(cv::Ptr<SkewDetector> detector);
        virtual ~HistogramWordSkDet();
        
    protected:
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities);
        
    };
}

#endif /* defined(__SkewDetection__HistogramWordSkDet__) */