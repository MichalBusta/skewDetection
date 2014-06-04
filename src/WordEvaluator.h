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
        
        double probability;
        
        Result(double angle, bool isWrong, cv::Mat debugImg, double probability) : angle(angle), isWrong(isWrong), debugImg(debugImg), probability(probability)
        {
            
        }
        
    };
    
    class WordEvaluator
    {
    public:
        
        WordEvaluator(std::string outputDir, std::string inputDir, std::string *referenceFile = NULL, bool writeData=true);
        
        virtual ~WordEvaluator();
        
        void run();
        
        void evaluateWord(std::string wordDir);
        
        void addWordDetector(cv::Ptr<WordSkewDetector> detector);
    
    private:
        
        std::vector<std::string> splitString(const std::string &inputString, char delim);
        
        void createFileStructure(std::string outputFolder);
        
        void writeResults(std::string outputFolder);
        
        std::string outputDirectory;
        
        int numberOfFiles;
        std::vector<Result> results;
        std::vector<double> reference;
        std::vector<bool> failed;
        std::vector<std::string> wordImages;
        std::vector<std::string> directories;
        
    };
}