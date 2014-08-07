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
        
        double angleDifference;
        
        bool isWrong;
        
        cv::Mat debugImg;
        
        std::string imgName;
        
        std::vector<std::string> letters;
        
        std::string fontName;
        
        Result(double angle, double angleDifference, bool isWrong, cv::Mat debugImg, std::string imgName,std::vector<std::string> letters, std::string fontName) : angle(angle), angleDifference(angleDifference), isWrong(isWrong), debugImg(debugImg), imgName(imgName), letters(letters), fontName(fontName)
        {
            
        }
        Result()
        {
            
        }
        
    };
    struct ImageData
    {
        std::string imgName;
        std::string letter;
        
    };
    struct statisticalResult
    {
        //result struct used for displaying and sorting statistics. the first entry in the statMap will be used by the sorting algorithm
        //map containing all image results for select sub-category
        std::vector<Result> resultVector;        //map containing all statistical measuremnts for select sub-category
        std::map<std::string, double> statMap;
        
        
        statisticalResult(std::vector<Result> resultVector, std::map<std::string, double> statMap) : resultVector(resultVector),statMap(statMap)
        {
            
        }
        
       statisticalResult()
        {
            
        }
    };
    
    class WordEvaluator
    {
    public:
        
        WordEvaluator(std::string outputDir, std::string inputDir, bool writeData=true,std::string *referenceFile = NULL);
        
        virtual ~WordEvaluator();
        
        void run();
        
        void addWordDetector(cv::Ptr<DiscreteVotingWordSkDet> detector, std::string detectorID);
    
    private:
        
        std::vector<std::string> splitString(const std::string &inputString, char delim);
        
        void evaluateWord(std::string wordDir, int idx);
        
        void createFileStructure(std::string outputFolder);
        
        void saveResults(std::string outputFolder);
        
        void createLayout(std::ofstream& outputFile);
        
        void saveDebugImg(std::string outputDir, Result result);
        std::string outputDirectory;
        
        std::vector<cv::Ptr<DiscreteVotingWordSkDet> > detectors;
        std::vector<std::string> detectorIDs;
        std::vector<Result> results;
        std::vector<double> reference;
        std::vector<bool> failed;
        std::vector<std::string> wordImages;
        std::vector<std::string> directories;
        
        
        
    };
}