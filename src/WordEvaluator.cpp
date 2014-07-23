//
//  WordEvaluator.cpp
//  SkewDetection
//
//  Created by David Helekal on 30/05/14.
//
//

#include "WordEvaluator.h"
#include <opencv2/highgui/highgui.hpp>
namespace cmp {
    
    WordEvaluator::WordEvaluator(std::string outputDir, std::string inputDir, bool writeData, std::string *referenceFile) : outputDirectory(outputDir){
        
        wordImages = IOUtils::GetFilesInDirectory(inputDir,"*.png", true);
        
        //if the reference file is supplied, set the reference vector the given data
        if (referenceFile!=NULL)
        {
            //do something
        }
        
        //else assume skew = 0
        else
        {
            for (size_t i=0; i<wordImages.size(); i++) {
                reference.push_back(0.0);
            }
        }
        
        //get the directories containing the different words
        for (size_t i=0; i<wordImages.size(); i++) {
            directories.push_back(IOUtils::RemoveExtension(wordImages[i]));
        }
    }
    
    WordEvaluator::~WordEvaluator()
    {
        
    }
    
    void WordEvaluator::run()
    {
        createFileStructure(outputDirectory);
        
        for (size_t i=0; i<directories.size(); i++) {
            evaluateWord(directories[i], i);
        }
        writeResults(outputDirectory);
    }
    
    void WordEvaluator::evaluateWord(std::string wordDir, int idx)
    {
        std::cout<<wordDir;
        //vector containing lines of image data from the suplied text file
        std::vector<std::vector<std::string> > imageData;
        //the output vector
        std::vector<std::vector<std::string> > words;
        //vector containing the path to the text file (is a vector only due to return type)
        std::string dataFilePath;
        //vector containing line data
        std::vector<std::string> lines;
        //vector containing the length of word lines
        std::vector<int> lineIndices;
        //temp string for parsing purposes
        std::string line;
        //the text file instance
        std::ifstream textdatafile;
        //index for iterating through lines
        int index=0;
        
        dataFilePath = wordDir+"/"+IOUtils::Basename(wordDir)+".txt";
        textdatafile.open(dataFilePath);
        
        while (getline(textdatafile, line)) {
            lines.push_back(line);
        }
        
        //parsing the text file
        bool isLine=false;
        for (size_t i=0; i<lines.size(); i++) {
            
            //if the line contains line data do
            if (isLine)
            {
                lineIndices.push_back(splitString(lines[i], ' ').size());
                continue;
            }
            
            //if the line contains the line header
            if (lines[i]=="LINES:")
            {
                isLine=true;
                continue;
            }
            
            //if the line contains image data
            if (!isLine && i!=0) {
                imageData.push_back(splitString(lines[i], ' '));
            }
        }
        
        //create a words vector, containing vectors with each letter
        
        for (size_t i=0; i<lineIndices.size(); i++) {
            std::vector<std::string> temp;
            for (size_t i1 =0; i1 <lineIndices[i]; i1++) {
                temp.push_back(imageData[index][5]);
                index++;
            }
            words.push_back(temp);
        }
        
        
        for (size_t i=0; i<detectors.size(); i++) {
            
            std::string dirPath=outputDirectory+"/"+detectorIDs[i]+"/"+IOUtils::Basename(wordDir);
            IOUtils::CreateDir(dirPath);
            
            for (size_t i1=0; i1<words.size(); i1++) {
                std::vector<Blob> imgs;
                double angle;
                cv::Mat debugImage;
                double tolerance = 0.06;
                bool isWrong =true;
                
                for (size_t i2=0; i2<words[i1].size(); i2++)
                {
                    cv::Mat tempImg = cv::imread(wordDir+"/"+words[i1][i2], CV_LOAD_IMAGE_GRAYSCALE);
                    tempImg = tempImg>128;
                    cv::copyMakeBorder(tempImg, tempImg, 5, 5,5,5, cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
                    //cv::imshow("test", tempImg);
                    //cv::waitKey(0);
                    imgs.push_back(Blob(tempImg));
                }
                angle=detectors[i]->detectSkew(imgs, 0.0, &debugImage);
                
                if (tolerance > fabs(angle-reference[idx])) {
                    isWrong = false;
                }
                Result tempResult(angle, isWrong, debugImage, IOUtils::Basename(wordDir));
                results.push_back(tempResult);
                
                std::stringstream imageName;
                imageName << dirPath<< "/" << IOUtils::Basename(wordDir) << i1 <<".jpeg";
                
                saveResult(imageName.str(), tempResult);
                
            }
        }
    }
    
    //string splitting method
    
    std::vector<std::string> WordEvaluator::splitString(const std::string &inputString, char delim)
    {
        std::stringstream ss(inputString);
        std::string item;
        std::vector<std::string> outputString;
        while (std::getline(ss, item, delim)) {
            outputString.push_back(item);
        }
        return outputString;
    }
    
    void WordEvaluator::addWordDetector(cv::Ptr<WordSkewDetector> detector, std::string detectorID)
    {
        detectors.push_back(detector);
        detectorIDs.push_back(detectorID);
        
    }
    
    //function for creating the output file structure in the output folder
    
    void WordEvaluator::createFileStructure(std::string outputFolder)
    {
        for (size_t i=0; i<detectorIDs.size(); i++) {
            std::string directoryPath;
            directoryPath=outputFolder;
            directoryPath += "/"+detectorIDs[i];
            IOUtils::CreateDir(directoryPath);
            
        }
    }
    
    //write results to the output folder & json
    
    void WordEvaluator::writeResults(std::string outputFolder)
    {
        std::cout << "writing results...";
        std::ofstream outputFile;
        outputFolder+="/Output.txt";
        outputFile.open(outputFolder);
        if (outputFile.is_open()) {
            for (size_t t=0; t<results.size(); t++) {
                if (results[t].isWrong) {
                    outputFile << results[t].imgName <<" "<< results[t].angle << " incorrect skew.\n";
                }
                else{
                    /*outputFile  << results[t].imgName <<" " << results[t].angle << " correct skew.\n";*/
                }
            }
        }
    }
    
    void WordEvaluator::saveResult(std::string outputDir, cmp::Result result)
    {
        cv::imwrite(outputDir, result.debugImg);
    }
    
    
}
