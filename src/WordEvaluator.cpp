//
//  WordEvaluator.cpp
//  SkewDetection
//
//  Created by David Helekal on 30/05/14.
//
//

#include "WordEvaluator.h"
#include <opencv2/highgui/highgui.hpp>
#define sortingConstant "mean"

namespace cmp {
    
    typedef std::map<std::string, statisticalResult>::iterator statMapIterator;
    static bool sortStatMap(const statMapIterator it1, const statMapIterator it2){ return(it1->second.statMap[sortingConstant]> it2->second.statMap[sortingConstant] );};
    static bool sortResultVector (Result res1, Result res2) {return (res1.angleDifference>res2.angleDifference);};
    
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
        saveResults(outputDirectory);
    }
    
    
    void WordEvaluator::evaluateWord(std::string wordDir, int idx)
    {
        //std::cout<<wordDir;
        //vector containing lines of image data from the suplied text file
        std::vector<std::vector<std::string> > imageData;
        //the output vector
        std::vector<std::vector<ImageData> > words;
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
            std::vector<ImageData> temp;
            ImageData tempData;
            for (size_t i1 =0; i1 <lineIndices[i]; i1++) {

                tempData.imgName = imageData[index][5];
                tempData.letter = imageData[index][6];
                temp.push_back(tempData);
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
                std::vector<std::string> letters;
                std::string font;
                std::string imageName;
                double angleDifference;
                
                for (size_t i2=0; i2<words[i1].size(); i2++)
                {
                    cv::Mat tempImg = cv::imread(wordDir+"/"+words[i1][i2].imgName, CV_LOAD_IMAGE_GRAYSCALE);
                    //tempImg = tempImg>128;
                    cv::copyMakeBorder(tempImg, tempImg, 5, 5,5,5, cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
                    //cv::imshow("test", tempImg);
                    //cv::waitKey(0);
                    imgs.push_back(Blob(tempImg));
                    letters.push_back(words[i1][i2].letter);
                }
                assert(letters.size()==imgs.size());
                double probability = 0;
                angle= detectors[i]->detectSkew( imgs, 0.0, probability, &debugImage );
                
                angleDifference = angle-reference[idx];
                
                if (tolerance > fabs(angleDifference)) {
                    isWrong = false;
                    
                }
                
                imageName =IOUtils::Basename(wordDir);
                
                font = splitString(imageName, '-')[0];
                
                Result tempResult(angle, std::fabs(angleDifference), isWrong, debugImage, imageName,letters,font);
                std::stringstream imageFileName;
                imageFileName << dirPath<< "/" << imageName <<".png";
                
                saveDebugImg(imageFileName.str(), tempResult);
                tempResult.debugImg.release();
                results.push_back(tempResult);

                
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
    
    void WordEvaluator::addWordDetector(cv::Ptr<DiscreteVotingWordSkDet> detector, std::string detectorID)
    {
        detectors.push_back(detector);
        detectorIDs.push_back(detectorID);
        
    }
    
    //function for creating the output file structure in the output folder
    
    void WordEvaluator::createFileStructure(std::string outputFolder)
    {
        //create a separate directory for storage of debug images returned by detectors with incorrect result
        std::string directoryPath;
        directoryPath=outputFolder;
        directoryPath+= "/";
        directoryPath+="Failed";
        IOUtils::CreateDir(directoryPath);
        
        //create a directory for each word
        for (size_t i=0; i<detectorIDs.size(); i++) {
            std::string directoryPath;
            directoryPath=outputFolder;
            directoryPath += "/"+detectorIDs[i];
            IOUtils::CreateDir(directoryPath);
            
        }
    }
    
    
    void WordEvaluator::createLayout(std::ofstream& outputFile){
        outputFile << "<!DOCTYPE html>" << "\n"
        <<"<html>" << "\n"
        <<"<head>" << "\n"
        <<"<style>" << "\n"
        <<"table,th,td" << "\n"
        <<"{"<< "\n"
        <<"border:"<<"1px"<<" "<< "solid"<<" "<< "black" << ";"<< "\n"
        <<"border-collapse:collapse"<< ";" << "\n"
        <<"}"<< "\n"
        <<"th,td"<< "\n"
        <<"{"<< "\n"
        <<"padding:"<<"5px"<<";"<< "\n"
        <<"}"<<"\n"
        <<"</style>"<< "\n"
        <<"</head>"<< "\n";
        
    }
    //write results to the output folder & json

    void WordEvaluator::saveResults(std::string outputFolder)
    {
        std::cout << "writing results...";
        std::ofstream outputFile;
        
        size_t numberOfDisplayedResults = 5;
        //preparing the file strucuture...
        
        std::map<std::string,std::map<std::string,std::vector<Result> > > iteratingMap;
        std::map<std::string,std::vector<Result>> fontMap;
        std::map<std::string,statisticalResult> sortedFontMap;
        
        std::map<std::string, std::vector<std::string>> keyOrders;
        
        std::map<std::string,std::vector<Result>> letterMap;
        std::map<std::string,statisticalResult> sortedLetterMap;
        
        std::map<std::string, std::map<std::string, statisticalResult > > sortedResultMap;
        //load up the category vectors
        
        for (Result r : results) {
            
            std::vector<std::string> letterVect;
            bool containsLetter=false;
            for (std::string letter : r.letters) {
                
                for (std::string ltr : letterVect) {
                    if (ltr == letter) {
                        containsLetter = true;
                    }
                }
                letterVect.push_back(letter);
                if (containsLetter == false) {
                    letterMap[letter].push_back(r);
                }
            }
            fontMap[r.fontName].push_back(r);
        }
        
        //load up the interatingMap
        
        iteratingMap["letters"] = letterMap;
        iteratingMap["fonts"] = fontMap;
        
        //sort the vectors
        
        for (auto iterator1 = iteratingMap.begin(); iterator1!=iteratingMap.end(); ++iterator1) {
            
            std::map<std::string,statisticalResult> tempStatMap;
            std::vector<statMapIterator> iterators;
            for (auto iterator2 = iterator1->second.begin(); iterator2 != iterator1->second.end(); ++iterator2) {
                //sort the vector
                std::sort(iterator2->second.begin(),iterator2->second.end(), sortResultVector);
                //and calculate the statistics
                double mean=0;
                std::map<std::string,double> statistics;
                double sum = 0.0;
                int count= 0;
                for (Result r : iterator2->second ) {
                    sum +=r.angleDifference;
                    count++;
                }
                if (count != 0) {
                    mean=sum/count;
                }
                else mean =0;
                statistics["mean"] = mean;
                statisticalResult tempResult(iterator2->second, statistics);
                tempStatMap[iterator2->first] = tempResult;
            }
            
            for (auto iterator3 = tempStatMap.begin(); iterator3!=tempStatMap.end(); ++iterator3) {
                iterators.push_back(iterator3);
            }
            std::sort(iterators.begin(), iterators.end(), sortStatMap);
            
            for (statMapIterator iter: iterators) {
                keyOrders[iterator1->first].push_back(iter->first);
            }
            sortedResultMap[iterator1->first] =tempStatMap;
        }
        
        //writing into the html file
        
        outputFile.open(outputFolder+"/"+"index.htm");
        
        createLayout(outputFile);
        
        outputFile << "<body>" << "\n";
        
        //creating the tables
        for (auto iterator : sortedResultMap ) {
            //writing the title
            outputFile << "<h1>" << iterator.first << "</h1>" << "\n";
            outputFile << "<tablestyle=" << "\"width:" << "500" <<"px\">";
            outputFile << "\n";
            outputFile << "<table>" << "\n";
            outputFile << "<tr>"<<"\n";
            
            //writing the header row
            outputFile << "<td>" << "</td>" <<"\n";
            
            for (auto iterator2 : iterator.second.begin()->second.statMap) {
                outputFile << "<td>" << iterator2.first << "</td>" <<"\n";
            }
            for (int c =0; c<numberOfDisplayedResults; c++) {
                outputFile << "<td>" << "Worst Result " <<(c+1) << "</td>"<<"\n";
            }
            outputFile << "</tr>"<<"\n";
            
            
            //writing the results rows
            for (std::string categoryName : keyOrders[iterator.first]) {
                outputFile << "<tr>" <<"\n";
                outputFile << "<td>" << categoryName <<"</td>"<<"\n";
                
                //writing the statistics
                for (auto iterator2 : iterator.second[categoryName].statMap) {
                    outputFile << "<td>" << iterator2.second << "</td>" <<"\n";
                }
                //writing the 5 worst results
                for (int c=0; c<numberOfDisplayedResults; c++ ) {
                    
                    if (c<iterator.second[categoryName].resultVector.size()) {
                        
                    outputFile << "<td>" << "<a href =\"" << "file://" <<outputDirectory<< "/" << detectorIDs[0] << "/" << iterator.second[categoryName].resultVector[c].imgName<< "/"<< iterator.second[categoryName].resultVector[c].imgName<<".png"<<"\"" <<">" << iterator.second[categoryName].resultVector[c].angleDifference << "</a>" << "</td>" <<"\n";
                    }
                }
                outputFile << "</tr>" <<"\n";
                
            }
            
            outputFile << "</table>" << "\n";
        }
        
        outputFile.close();
        
    }
    
    
    void WordEvaluator::saveDebugImg(std::string outputDir, cmp::Result result)
    {
        cv::imwrite(outputDir, result.debugImg);
        if(result.isWrong){
            cv::imwrite(outputDirectory+"/Failed"+"/"+result.imgName+".png", result.debugImg);
        }
        
    }
    
    
}
