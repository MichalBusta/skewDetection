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
        
        Blob(cv::Mat mask, cv::Rect bBox = cv::Rect()){
            this->mask = mask;
            this->bBox = bBox;
            
        };
    };
    
    class WordSkewDetector
    {
    public:
        
        WordSkewDetector();
        virtual ~WordSkewDetector();
        
        virtual double detectSkew( std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage =NULL) = 0;

    };
    class ContourWordSkewDetector : public WordSkewDetector
    {
    public:
        
        ContourWordSkewDetector(cv::Ptr<ContourSkewDetector> detector);
        virtual ~ContourWordSkewDetector();
       
        virtual double detectSkew(std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage =NULL);
        
        virtual double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL);

    protected:
        
        cv::Ptr<ContourSkewDetector> localDetector;
        
        virtual double computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, std::vector<cv::Mat> debugImages, cv::Mat* debugImage = NULL)=0;
    };
}
#endif /* defined(__SkewDetection__WordSkewDetector__) */
