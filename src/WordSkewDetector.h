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

    struct Blob{
        cv::Mat mask;
        cv::Rect bBox;
    };
    
    class WordSkewDetector
    {
    public:
        
        WordSkewDetector(cv::Ptr<SkewDetector> detector);
        virtual ~WordSkewDetector();
        
        
    protected:
        
        cv::Ptr<ContourSkewDetector> localDetector;
        
    };
    class ContourWordSkewDetector : public WordSkewDetector
    {
    public:
        
        ContourWordSkewDetector(cv::Ptr<SkewDetector> detector);
        virtual ~ContourWordSkewDetector();
       
        virtual double detectSkew( std::vector<Blob>& blobs, double lineK, cv::Rect bBox, cv::Mat* debugImage = NULL);
        
        std::vector<double> angles;
        
    protected:
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities)=0;
    };
}
#endif /* defined(__SkewDetection__WordSkewDetector__) */
