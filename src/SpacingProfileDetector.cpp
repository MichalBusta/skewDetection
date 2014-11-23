//
//  SpacingProfileDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 15/11/14.
//
//

#include "SpacingProfileDetector.h"
#include "ThinProfileSkDet.h"

namespace cmp {
    
    SpacingProfileDetector::SpacingProfileDetector() : ContourWordSkewDetector(){
        
    }
    
    SpacingProfileDetector::~SpacingProfileDetector(){
        
    }
    
    double SpacingProfileDetector::detectSkew(std::vector<Blob> &blobs, double lineK, double &probability,cv::Mat* debugImage){
        return 0;
    }
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point> *> &contours, double lineK, double& probability, cv::Mat* debugImage){
        
        spaceCount = contours.size()-1;
        
        assert(spaceCount>0);
        
        double angle=0;
        double maxConfidence=0;
        
        std::vector<cv::Point> spacingCountours;
        double hist[180];
        
        ThinProfileSkDet detector = ThinProfileSkDet(CV_CHAIN_APPROX_NONE, 0.023,IGNORE_ANGLE, 0.02,false,false);
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> spaceProfile;
            createMat(contours[i+1][0], contours[i][0], spaceProfile);
            detector.voteInHistogram(spaceProfile, 1.0,hist, 1);
            
        }
        
        for (int i =0; i<180; i++) {
            
            if (hist[i]>maxConfidence) {
                
                maxConfidence = hist[i];
                angle = i;
                
            }
            
            
        }
        
        return angle;
        
    }
    
    void SpacingProfileDetector::createMat(std::vector<cv::Point> rightChar, std::vector<cv::Point> leftChar, std::vector<cv::Point> &outputCont, double additionalSpacing)
    {
        std::vector<cv::Point> firstFaceContour;
        
        std::vector<cv::Point> secondFaceContour;
        
        std::vector<cv::Point> firstConvex;
        cv::convexHull(leftChar, firstConvex);
        
        std::vector<cv::Point> secondConvex;
        cv::convexHull(rightChar, secondConvex);
        
        getFace(firstConvex, firstFaceContour);
        getFace(secondConvex, secondFaceContour, true);
        
        invertMerge(firstFaceContour, secondFaceContour, outputCont, additionalSpacing);
        
    }
    
    void SpacingProfileDetector::getFace(std::vector<cv::Point> input, std::vector<cv::Point> &output, bool getLeft){
        
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
            
            if (top < bot) {
                
                int i =top;
                
                while (i != bot){
                    
                    if (i==0) {
                        output.push_back(input[i]);
                        i = input.size();
                    }
                    else{
                        output.push_back(input[i]);
                        i--;
                    }
                }
            }
            else {
                
                int i =bot;
                
                while (i != top){
                    
                    if (i==0) {
                        i = input.size();
                    }
                    else{
                        output.push_back(input[i]);
                        i--;
                    }
                }
            }
        }
        
        else {
            if (top < bot) {
                
                for (int i =top; i<=bot; i++) {
                    
                    output.push_back(input[i]);
                    
                }
            }
            else {
                
                for (int i = bot; i<= top; i++) {
                    output.push_back(input[i]);
                }
            }
        }
    }
    
    void SpacingProfileDetector::invertMerge(std::vector<cv::Point>& firstFace, std::vector<cv::Point>& secondFace, std::vector<cv::Point> &outputCont, double spacing){
        
        deOffset(firstFace, spacing);
        deOffset(secondFace);
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
                
                for (auto iter = firstFace.end(); iter != firstFace.begin(); --iter) {
                    
                    outputCont.push_back(*iter);
                    
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
                
                for (auto iter = firstFace.end(); iter != firstFace.begin(); --iter) {
                    
                    outputCont.push_back(*iter);
                    
                }
                
            }
        }
    }
    
    void SpacingProfileDetector::deOffset(std::vector<cv::Point >& cont, int xOrigin){
        
        int minX=INT16_MAX;
        
        for (int i=0; i<cont.size(); i++) {
            
            minX = MIN(minX, cont[i].x);
            
        }
        
        minX += xOrigin;
        
        for (int i=0; i<cont.size(); i++) {
            
            cont[i].x -=minX;
            
        }
    }
}