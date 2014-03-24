//
//  WordSkewDetector.h
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#ifndef __SkewDetection__WordSkewDetector__
#define __SkewDetection__WordSkewDetector__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "SkewDetector.h"
#include <math.h>

namespace cmp{

    class WordSkewDetector
    {
    public:
        
        WordSkewDetector(cv::Ptr<SkewDetector> detector);
        virtual ~WordSkewDetector();
        
        virtual double detectSkew( const std::vector<cv::Mat> masks, double  lineK, const cv::Mat* debugImage = NULL) =0;
        
    protected:
        
        cv::Ptr<ContourSkewDetector> localDetector;
        
    };
    class CountourWordSkewDetector : public WordSkewDetector
    {
    public:
       
        virtual double detectSkew( std::vector<cv::Mat>& masks, double  lineK,cv::Mat* debugImage = NULL) =0;
        
        CountourWordSkewDetector();
        virtual ~CountourWordSkewDetector();
        
        std::vector<double> angles;
        
    protected:
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities);
    };
}
#endif /* defined(__SkewDetection__WordSkewDetector__) */
