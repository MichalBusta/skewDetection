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
    DiscreteVotingWordSkDet::DiscreteVotingWordSkDet(cv::Ptr<SkewDetector> detector) : ContourWordSkewDetector(detector)
    {
        
    }
    DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
    {
        
    }
    
    double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, std::vector<cv::Mat> debugImages, cv::Mat* debugImage)
    {
        
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
        
        //get max debugimg height
        
        for (size_t i = 0; i<debugImages.size(); i++) {
            maxImgHeight = MAX(debugImages[i].rows, maxImgHeight);
        }
        

        
        //draw the histogram
        
        int headerbuffer =20;
        int histWidth =500;
        int histHeight = 300;
        int colWidth = 5;
        int confidenceBarWidth =8;
        int imgBufferBar = 5;
        cv::Scalar confidenceBarColor(50,90,45);
        
        //calculate number of images per row
        int rowSize =0;
        int rowIdx =0;
        rowImages.resize(1);
        for (size_t i =0; i<debugImages.size(); i++) {
            
            if (rowSize + debugImages[i].cols +(confidenceBarWidth*(i+1))+imgBufferBar < histWidth) {
                rowSize += debugImages[i].cols;
                rowSize += confidenceBarWidth*(i+1)+imgBufferBar;
                rowImages[rowIdx].push_back(debugImages[i]);
            }
            else{
                
                rowIdx++;
                rowImages.resize(rowIdx+1);
                rowImages[rowIdx].push_back(debugImages[i]);
                rowSize=debugImages[i].cols +(confidenceBarWidth*(i+1))+imgBufferBar;
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
                
                cv::rectangle(histogram, cv::Point(rowWidth+rowImages[i][i1].cols, maxImgHeight*(i+1)-1), cv::Point(rowWidth+rowImages[i][i1].cols+confidenceBarWidth, maxImgHeight*(i+1)-(probabilities[i]*maxImgHeight)-1), confidenceBarColor,CV_FILLED);
                
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