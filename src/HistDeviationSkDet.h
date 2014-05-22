//
//  HistDeviationSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 11/05/14.
//
//

#ifndef __SkewDetection__HistDeviationSkDet__
#define __SkewDetection__HistDeviationSkDet__

#include <iostream>
#include "SkewDetector.h"

namespace cmp {
    class HistDeviationSkDet : public SkewDetector
    {
    public:
    
        HistDeviationSkDet();
        
        virtual ~HistDeviationSkDet();
        
        virtual double detectSkew(cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL);
        
    protected:
        
        std::vector<double> getSkeleton(cv::Mat& src);
        
        double getDeviance(std::vector<double>& input);
        
        cv::Mat transformMask(cv::Mat& src, double angle);
        
    };
    
}
#endif /* defined(__SkewDetection__HistDeviationSkDet__) */
