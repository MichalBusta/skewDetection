//
//  WordEvaluator.h
//  SkewDetection
//
//  Created by David Helekal on 30/05/14.
//
//

#ifndef __SkewDetection__WordEvaluator__
#define __SkewDetection__WordEvaluator__
#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <iostream>
#include "SkewDetection.h"
#include "WordSkewDetection.h"

#endif /* defined(__SkewDetection__WordEvaluator__) */
namespace cmp
{
    class WordEvaluator
    {
    public:
        
        WordEvaluator(std::string outputDir, std::string inputDir, std::string *referenceFile = NULL, bool writeData=true);
        
        virtual ~WordEvaluator();
        
        void run();
        
        void addWordDetector(cv::Ptr<WordSkewDetector> detector);
    
    private:
        
        void createFileStructure(std::string outputDir);
        
        void writeResults(std::string outputFolder);
        
        void getContours();
    };
}