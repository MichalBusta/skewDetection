//
//  WordEvaluator.cpp
//  SkewDetection
//
//  Created by David Helekal on 30/05/14.
//
//

#include "WordEvaluator.h"
namespace cmp {
    
    WordEvaluator::WordEvaluator(std::string outputDir, std::string inputDir, std::string *referenceFile, bool writeData) : outputDirectory(outputDir){
        
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
            evaluateWord(directories[i]);
        }
    }
    
    void WordEvaluator::evaluateWord(std::string wordDir)
    {
        //vector containing lines of image data from the suplied text file
        std::vector<std::vector<std::string> > imageData;
        //the output vector
        std::vector<std::vector<std::string> > words;
        //vector containing the path to the text file (is a vector only due to return type)
        std::vector<std::string> dataFilePath;
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
        
        dataFilePath = IOUtils::GetFilesInDirectory(wordDir, "*.txt",true);
        textdatafile.open(dataFilePath[0]);
        
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
            if (lines[i]=="LINES")
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
    
    void WordEvaluator::addWordDetector(cv::Ptr<WordSkewDetector> detector)
    {
        
    }
    
    //function for creating the output file structure in the output folder
    
    void WordEvaluator::createFileStructure(std::string outputFolder)
    {
        for (size_t i=0; i<directories.size(); i++) {
            
        }
    }
    
    //write results to the output folder & json
    
    void WordEvaluator::writeResults(std::string outputFolder)
    {
        
    }
    
    
}