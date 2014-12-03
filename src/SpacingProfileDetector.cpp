//
//  SpacingProfileDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 15/11/14.
//
//

#include "SpacingProfileDetector.h"
#include "ThinProfileSkDet.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "SkewDetector.h"

namespace cmp {
    
    SpacingProfileDetector::SpacingProfileDetector() : ContourWordSkewDetector(){
        
    }
    
    SpacingProfileDetector::~SpacingProfileDetector(){
        
    }
    
    double SpacingProfileDetector::detectSkew(std::vector<Blob> &blobs, double lineK, double &probability,cv::Mat* debugImage){
        
        std::vector<std::vector<cv::Point> > contours;
        std::vector<int> yPos;
        
        spaceCount = contours.size()-1;
        assert(spaceCount>0);
        
        for (Blob blob : blobs) {
            
            std::vector<std::vector<cv::Point> > tempContours;
            std::vector<cv::Vec4i> hierarchy;
            
            cv::findContours(blob.mask, tempContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
            
            ContourSkewDetector::getBigestContour(tempContours, hierarchy);
            
            contours.push_back(tempContours[0]);
            yPos.push_back(blob.bBox.y);
            
        }
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> &leftChar = contours[i];
            std::vector<cv::Point> &rightChar = contours[i+1];
            
            std::vector<cv::Point> frontFace;
            std::vector<cv::Point> backFace;
            
            std::vector<cv::Point> leftChar_Convex;
            cv::convexHull(leftChar, leftChar_Convex);
            
            std::vector<cv::Point> rightChar_Convex;
            cv::convexHull(rightChar, rightChar_Convex);
            
            getFace(leftChar_Convex, frontFace);
            getFace(rightChar_Convex, backFace);
            
            int yOffset=0;
            yOffset = abs(yPos[i] - yPos[i+1]);
            
            if (yPos[i]>yPos[i+1]) {
                deOffset(frontFace,0,yOffset);
                deOffset(backFace,0,0);
            }
            else{
                deOffset(backFace,0,yOffset);
                deOffset(frontFace,0,0);
            }
            
            findProfiles(frontFace, backFace);
            
        }
        
        return 0;
        
        
    }
    
    void SpacingProfileDetector::findProfiles(std::vector<cv::Point> firstFace,std::vector<cv::Point> secondFace){
        
    }
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point> *> &contours, double lineK, double& probability, cv::Mat* debugImage){
        
        spaceCount = contours.size()-1;
        
        assert(spaceCount>0);
        
        double angle=0;
        double maxConfidence=0;
        
        double hist[180];
        memset (hist, 0, 180 * sizeof(double));
        
        ThinProfileSkDet detector = ThinProfileSkDet(CV_CHAIN_APPROX_NONE, 0.023,IGNORE_ANGLE, 0.02,false,true);
        
        for (int i=0; i<spaceCount; i++) {
           
            std::vector<cv::Point> spaceProfile = createMat(*contours[i+1], *contours[i]);
           /*
            cv::Mat img = cv::Mat::zeros(300, 300, CV_8UC3);
            std::vector<std::vector<cv::Point>> cont;
            
            cont.push_back(spaceProfile);
            
            cv::drawContours(img, cont, 0, cv::Scalar(255,0,0),1,8);
            cv::imshow(" ", img);
            cv::waitKey(0);
            
            */
            
            cv::Mat img;
            
            detector.voteInHistogram(spaceProfile, 0, hist, 1,false ,&img);
            
            //cv::imshow(" ", img);
            //cv::waitKey(0);
            
            
        }
        
        for (int i =0; i<180; i++) {
            
            if (hist[i]>maxConfidence) {
                
                maxConfidence = hist[i];
                angle = i*M_PI/180-M_PI/2;
                
            }
            
            
        }
        
        return angle;
        
    }
    
    std::vector<cv::Point> SpacingProfileDetector::createMat(std::vector<cv::Point> rightChar, std::vector<cv::Point> leftChar, double additionalSpacing)
    {
        std::vector<cv::Point> outputCont;
        
        std::vector<cv::Point> firstFaceContour;
        
        std::vector<cv::Point> secondFaceContour;
        
        std::vector<cv::Point> firstConvex;
        cv::convexHull(leftChar, firstConvex);
        
        std::vector<cv::Point> secondConvex;
        cv::convexHull(rightChar, secondConvex);
        
        getFace(firstConvex, firstFaceContour);
        getFace(secondConvex, secondFaceContour, true);
        
        std::vector<std::vector<cv::Point>> cont;
        
        /*cv::Mat img = cv::Mat::zeros(300, 300, CV_8UC3);
        cont.push_back(firstFaceContour);
        cv::drawContours(img, cont, 0, cv::Scalar(255,0,0),1,8);
        cv::imshow(" ", img);
        cv::waitKey(0);
        */
        
        
        invertMerge(firstFaceContour, secondFaceContour, outputCont, additionalSpacing);
        
        return outputCont;
        
    }
    
    void SpacingProfileDetector::getFace(std::vector<cv::Point> &input, std::vector<cv::Point> &output, bool getLeft){
        
        output = std::vector<cv::Point>();
        
        int top=0;
        int bot=0;
        
        for (int k=0; k<input.size(); k++) {
            
            if (input[k].y < input[top].y) {
                top = k;
            }
            
            if (input[k].y >input[bot].y) {
                bot = k;
            }
        }
        
        if (getLeft){
            
            int next;
            
            if (top-1 >= input.size()) {
                
                next = 0;
                
            }
            else next=top+1;
            
            if (input[next].x < input[top].x) {
                
                int i = bot;
                
                while (i != top){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (i==0) {
                        output.push_back(input[i]);
                        i = input.size()-1;
                    }
                    else{
                        output.push_back(input[i]);
                        i--;
                    }
                }
            }
            
            else {
                
                int i = top;
                
                while (i != bot){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (i==0) {
                        output.push_back(input[i]);
                        i = input.size()-1;
                    }
                    else{
                        output.push_back(input[i]);
                        i--;
                    }
                }
            }
        }
        
        else {
            
            int next;
            
            if (top-1 >= input.size()) {
                
                next = 0;
                
            }
            else next=top+1;
            
            if (input[next].x>input[top].x) {
                
                int i = top;
                
                while (i != bot){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (i>=input.size()-1) {
                        output.push_back(input[i]);
                        i = 0;
                    }
                    else{
                        output.push_back(input[i]);
                        i++;
                    }
                
                }
            }
            else {
                
                int i = top;
                
                while (i != bot){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (i==0) {
                        output.push_back(input[i]);
                        i = input.size()-1;
                    }
                    else{
                        output.push_back(input[i]);
                        i--;
                    }
                }
            }
        }
    }
    
    void SpacingProfileDetector::invertMerge(std::vector<cv::Point> firstFace, std::vector<cv::Point> secondFace, std::vector<cv::Point> &outputCont, double spacing){
        
        int xmax = 0;
        
        deOffset(secondFace);
        
        for (int i =0; i<secondFace.size(); i++) {
            xmax = MAX(xmax, secondFace[i].x);
        }
        
        deOffset(firstFace, spacing+xmax);
        outputCont.clear();
        
        //finding the top most points
        int topMostIndex_1=0, topMostIndex_2=0;
        
        if (firstFace[0].y<firstFace[firstFace.size()-1].y) {
            
            topMostIndex_1 = firstFace.size();
            
        }
        
        if (secondFace[0].y<secondFace[secondFace.size()-1].y) {
            
            topMostIndex_2 = secondFace.size();
            
        }
        
        //merging countours
        
        outputCont = secondFace;
        
        if (topMostIndex_2 != 0) {
            
            if (topMostIndex_2 != 0) {
                
                for (int i =firstFace.size()-1; i>0; i--) {
                    
                    outputCont.push_back(firstFace[i]);
                    
                }
                
            }
            else {
                
                outputCont.insert(outputCont.end(), firstFace.begin(), firstFace.end());
                
            }
        }
        else {
            
            if (topMostIndex_2 != 0) {
                
                outputCont.insert(outputCont.end(), firstFace.begin(), firstFace.end());
                
            }
            else {
                
                for (int i =firstFace.size()-1; i>0; i--) {
                    
                    outputCont.push_back(firstFace[i]);
                    
                }
            }
        }
    }
    
    void SpacingProfileDetector::deOffset(std::vector<cv::Point >& cont, int xOrigin, int yOrigin){
        
        int minX=INT16_MAX;
        int minY=INT16_MAX;
        
        for (int i=0; i<cont.size(); i++) {
            
            minX = MIN(minX, cont[i].x);
            minY = MIN(minY, cont[i].y);
            
        }
        
        minY += yOrigin;
        minX -= xOrigin;
        cv::Point pt =cv::Point(minX,minY);
        
        for (int i=0; i<cont.size(); i++) {
            
            cont[i] =cont[i]-pt;
            
        }
    }
    
    void SpacingProfileDetector::correctYOffset(std::vector<std::vector<cv::Point> > contours, std::vector<cv::Rect> boundingBoxes){
        
        
        
    }
    }