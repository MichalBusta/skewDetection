//
//  DiscreteVotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#include "DiscreteVotingWordSkDet.h"
#include "WordSkewDetector.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//#define VERBOSE

namespace cmp
{
    DiscreteVotingWordSkDet::DiscreteVotingWordSkDet(std::vector< cv::Ptr<SkewDetector> > detectors, std::vector<std::string> detNames,std::vector<double>weights,std::map<std::string, cv::Scalar> detectorIDColours)
    {

        
        this->detNames = detNames;
        this->detectors = detectors;
        this->weights = weights;
        this->detectorIDColours = detectorIDColours;
        int noOfDetectors = detectors.size();
        assert(detNames.size() == noOfDetectors && weights.size() ==noOfDetectors && detectorIDColours.size()==noOfDetectors);
    }
    DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
    {
        
    }
    
    double DiscreteVotingWordSkDet::detectSkew(std::vector<Blob> &blobs, double lineK, cv::Mat* debugImage)
    {
        double confidence;
        std::vector<double> angles;
        std::vector<double> probabilities;
        std::vector<std::map<std::string, double> > confidenceData;
        std::vector<std::map<std::string, cv::Mat> > debugImages;
        
        for (size_t t=0; t<blobs.size(); t++) {
            
            std::map<std::string,cv::Mat> tempImgMap;
            std::map<std::string,double> tempConfidenceMap;
            std::vector<double> detectorConfidence;
            double bestDetection=0;
            int bestIndex=0;
            for (size_t t1=0; t1<detectors.size(); t1++) {
                cv::Mat tempDebug;
                double angle;
                angle =detectors[t1]->detectSkew(blobs[t].mask, lineK, &tempDebug);
                if (bestDetection<detectors[t1]->lastDetectionProbability) {
                    bestDetection=detectors[t1]->lastDetectionProbability;
                    bestIndex=t1;
                }
                
                detectorConfidence.push_back(detectors[t1]->lastDetectionProbability*weights[t1]);
                
                assert(t1<detNames.size());
                tempImgMap[detNames[t1]] = tempDebug;
                tempConfidenceMap[detNames[t1]] =detectors[t1]->lastDetectionProbability;
                
                
            }
            assert(bestIndex<detectorConfidence.size());
            probabilities.push_back(detectorConfidence[bestIndex]);
            debugImages.push_back(tempImgMap);
            confidenceData.push_back(tempConfidenceMap);
            angles.push_back(bestDetection);
        }
        
        VisualisationData visData(confidenceData,debugImages);
        return computeAngle(angles, probabilities, confidence, visData, debugImage);
    }
    
    double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, VisualisationData visualisationInfo, cv::Mat* debugImage)
    {
        
        VisualisationData visData =visualisationInfo;
        probability = 0;
        size_t noOfGroups=70;
        double groupRange;
        double min = -M_PI_2, max = M_PI_2;
        double maxProb = 0;
        double allProb = 0;
        int iterator=0;
        double angle=0;
        double sigma =0;
        double delta =1;
        int range=10;
        double resolution =0.4;
        int maxImgHeight=0;
        int headerHeight=0;
        std::vector< std::vector<cv::Mat> > rowImages;;
        cv::Mat histogram;
        
        
        std::vector<double> groupProbs;
        groupProbs.resize(noOfGroups);
        std::fill(groupProbs.begin(), groupProbs.end(), 0);
        groupRange = (max-min) / (noOfGroups);
        
        
        for (int i=0; i<angles.size(); i++) {
            int idx =  (int) floor( (angles[i] - min ) / groupRange);
            idx = MIN(idx, noOfGroups - 1);
            assert( idx >= 0 );
            
            for(int i1 =0; i1 <range; i1++){
                if (idx+i1<noOfGroups-1) {
                    groupProbs[idx+i1] += probabilities[i]/(delta*sqrt(2*M_PI))*pow(M_E, -(i1*resolution-sigma)*(i1*resolution-sigma)/(2*delta*delta));
                }
                if (idx-i1>0 && i1!=0) {
                    groupProbs[idx-i1] += probabilities[i]/(delta*sqrt(2*M_PI))*pow(M_E, -(i1*resolution-sigma)*(i1*resolution-sigma)/(2*delta*delta));
                }
                
            }
            allProb += probabilities[i];
        }
        
        for (int i =0; i <groupProbs.size(); i++) {
            if (maxProb < groupProbs[i]) {
                maxProb = groupProbs[i];
                iterator=i;
            }
        }
        
        angle= iterator*groupRange+min;
        probability = maxProb / allProb;
        
        //the drawing part
        
        int headerbuffer =20;
        int histWidth =500;
        int histHeight = 300;
        int colWidth = 5;
        int confidenceBarWidth =8;
        int confidenceBarSpacer =3;
        int imgBufferBar = 5;
        cv::Scalar confidenceBarColor(50,90,45);
        
        //setting the detectorIDColours
        
        //get max debugimg height

        for (size_t i = 0; i<visData.imageData.size(); i++) {
            for (auto iterator = visData.imageData[i].begin(); iterator != visData.imageData[i].end(); ++iterator) {
                maxImgHeight = MAX(iterator->second.rows, maxImgHeight);
            }
        }
        
        //drawing confidence bar into each debug image
        
        for (size_t i = 0; i<visData.imageData.size(); i++) {
            
            for (auto iterator : visData.imageData[i]) {
                
                //resizing the canvas and copying the debug image
                cv::Mat tempImg = cv::Mat::zeros(maxImgHeight, iterator.second.cols+confidenceBarSpacer+confidenceBarWidth,CV_8UC3);
                cv::Rect roi(0,0, iterator.second.cols, maxImgHeight-iterator.second.rows);
                iterator.second.copyTo(tempImg);
                
                //drawing the bar itself
                cv::rectangle(tempImg, cv::Point(iterator.second.cols+confidenceBarSpacer, maxImgHeight), cv::Point(iterator.second.cols+confidenceBarSpacer+confidenceBarWidth,visData.confidenceData[i][iterator.first]*maxImgHeight), confidenceBarColor,CV_FILLED);
                
                iterator.second = tempImg;
            }
        }
        
        //draw the histogram
        
        //calculate number of images per row
        int rowSize =0;
        int rowIdx =0;
        rowImages.resize(1);
        for (auto imgMap : visData.imageData) {
            
            for (auto iterator : imgMap) {
            
                if (rowSize + iterator.second.cols +imgBufferBar < histWidth) {
                    rowSize += iterator.second.cols;
                    rowSize += imgBufferBar;
                    rowImages[rowIdx].push_back(iterator.second);
                }
                else{
                
                    rowIdx++;
                    rowImages.resize(rowIdx+1);
                    rowImages[rowIdx].push_back(iterator.second);
                    rowSize=iterator.second.cols+imgBufferBar;
                }
            
            }
        }
        
        headerHeight = maxImgHeight*(rowIdx+1);
        
        int totalHeaderHeight = headerbuffer+headerHeight;
        
        histogram = cv::Mat::zeros(histHeight+totalHeaderHeight, histWidth, CV_8UC3);
        int graphWidth =noOfGroups*colWidth;
        int sidebarWidth = (histWidth-graphWidth)/2;
        
        //drawing tbe header buffer
        cv::rectangle(histogram, cvPoint(0, totalHeaderHeight), cvPoint(histWidth, headerHeight), cv::Scalar(213,213,213),CV_FILLED);
        //drawing the sidebars
        cv::rectangle(histogram, cv::Point(0,0+headerbuffer+headerHeight), cv::Point(sidebarWidth,histHeight+totalHeaderHeight), cv::Scalar(213,213,213), CV_FILLED);
        cv::rectangle(histogram, cv::Point(histWidth-sidebarWidth,0+headerbuffer+headerHeight), cv::Point(histWidth,histHeight+totalHeaderHeight), cv::Scalar(213,213,213), CV_FILLED);
        
        //drawing the graph
        
        //get max prob
        
        double maxConfidence=0.0;
        for (int i=0; i<groupProbs.size(); i++) {
            if(maxConfidence<groupProbs[i]){
                maxConfidence=groupProbs[i];
            }
        }
        
        if(maxConfidence>1){
            for (int i =0; i <noOfGroups; i++) {
            
            cv::rectangle(histogram, cv::Point(i*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point(colWidth*i+colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight*(groupProbs[i]/maxConfidence)), cv::Scalar(0,0,255),CV_FILLED);
            }
        }
        else{
            for (int i =0; i <noOfGroups; i++) {
                
                cv::rectangle(histogram, cv::Point(i*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point(colWidth*i+colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight*groupProbs[i]), cv::Scalar(0,0,255),CV_FILLED);
            }
        }
        //drawing the scale
        
        cv::line(histogram, cv::Point((noOfGroups/2)*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point((noOfGroups/2)*colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight+1), cv::Scalar(75,255,60));
        cv::line(histogram, cv::Point((noOfGroups/2+1)*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point((noOfGroups/2+1)*colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight+1), cv::Scalar(75,255,60));
        cv::line(histogram, cv::Point((noOfGroups/4)*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point((noOfGroups/4)*colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight+1), cv::Scalar(40,50,40));
          cv::line(histogram, cv::Point((noOfGroups/4*3+1)*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point((noOfGroups/4*3+1)*colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight+1), cv::Scalar(40,50,40));

        
        
        //drawing the debug images from detectors
        
        for (size_t i =0; i<rowImages.size(); i++) {
            int imageCount =0;
            int rowWidth=0;
            for (size_t i1=0; i1<rowImages[i].size(); i1++) {
                

                cv::Rect roi =cv::Rect(rowWidth,(maxImgHeight*i)+(maxImgHeight-rowImages[i][i1].rows),rowImages[i][i1].cols,rowImages[i][i1].rows);
                cv::Mat temp;
                
#ifdef VERBOSE
                imshow("ts", rowImages[i][i1]);
                cv::waitKey(0);
#endif
                
                try {
                    rowImages[i][i1].copyTo(histogram(roi));
                    
                } catch (...) {
                    
                }
                //drawing the confidence bar;
                
                rowWidth += rowImages[i][i1].cols+(confidenceBarWidth*(i+1))+imgBufferBar;
            }
            imageCount++;
        }
        
        //the debug image
        if(debugImage != NULL)
        {
            *debugImage = histogram;
#ifdef VERBOSE
            
            cv::imshow("Histogram", *debugImage);
            cv::waitKey(0);
#endif
        }
        return angle;
        
    }
}