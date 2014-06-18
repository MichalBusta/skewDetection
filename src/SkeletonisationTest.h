//
//  SkeletonisationTest.h
//  SkewDetection
//
//  Created by David Helekal on 14/06/14.
//
//

#ifndef __SkewDetection__SkeletonisationTest__
#define __SkewDetection__SkeletonisationTest__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "math.h"

namespace cmp {
    
    class SkeletonisationTest
    {
    public:
        
        SkeletonisationTest();
        
        virtual ~SkeletonisationTest();
        
        static void run(cv::Mat* inputImage, cv::Mat* output);
        
    };
    
}
#endif /* defined(__SkewDetection__SkeletonisationTest__) */
