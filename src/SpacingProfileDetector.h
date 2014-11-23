//
//  SpacingProfileDetector.h
//  SkewDetection
//
//  Created by David Helekal on 15/11/14.
//
//

#ifndef __SkewDetection__SpacingProfileDetector__
#define __SkewDetection__SpacingProfileDetector__

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "SkewDetector.h"
#include <math.h>
#include "WordSkewDetector.h"

#endif /* defined(__SkewDetection__SpacingProfileDetector__) */

namespace cmp {
    
    class SpacingProfileDetector : public ContourWordSkewDetector
    {
        public:
        
            SpacingProfileDetector();
        
            ~SpacingProfileDetector();
        
            double detectSkew( std::vector<Blob>& blobs, double lineK, double& probability, cv::Mat* debugImage =NULL);
        
            double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL);
        
        private:
        
            void createMat(std::vector<cv::Point> rightChar,std::vector<cv::Point> leftChar, std::vector<cv::Point>& outputCont, double additionalSpacing = 5);
        
            void invertMerge(std::vector<cv::Point>& firstFace,std::vector<cv::Point>& secondFace, std::vector<cv::Point>& outputCont, double spacing);
        
            size_t spaceCount;
        
            void getFace(std::vector<cv::Point> input, std::vector<cv::Point>& output, bool getLeft = false);
        
            void deOffset(std::vector<cv::Point>& cont, int xOrigin = 0);
    };
}