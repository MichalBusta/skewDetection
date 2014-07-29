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
#include <fstream>
#include "IOUtils.h"

#endif /* defined(__SkewDetection__WordEvaluator__) */
namespace cmp
{
    struct Result
    {
        double angle;
        
        bool isWrong;
        
        cv::Mat debugImg;
        
        std::string imgName;
        
        Result(double angle, bool isWrong, cv::Mat debugImg, std::string imgName) : angle(angle), isWrong(isWrong), debugImg(debugImg), imgName(imgName)
        {
            
        }
        
    };
    
    class WordEvaluator
    {
    public:
        
        WordEvaluator(std::string outputDir, std::string inputDir, bool writeData=true,std::string *referenceFile = NULL);
        
        virtual ~WordEvaluator();
        
        void run();
        
        void addWordDetector(cv::Ptr<WordSkewDetector> detector, std::string detectorID);
    
    private:
        
        std::vector<std::string> splitString(const std::string &inputString, char delim);
        
        void evaluateWord(std::string wordDir, int idx);
        
        void createFileStructure(std::string outputFolder);
        
        void writeResults(std::string outputFolder);
        
        void saveResult(std::string outputDir, Result result);
        std::string outputDirectory;
        
        std::vector<cv::Ptr<WordSkewDetector> > detectors;
        std::vector<std::string> detectorIDs;
        std::vector<Result> results;
        std::vector<double> reference;
        std::vector<bool> failed;
        std::vector<std::string> wordImages;
        std::vector<std::string> directories;
        
        
    };
}