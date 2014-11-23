//
//  SpacingProfileDetector.cpp
//  SkewDetection
//
//  Created by David Helekal on 15/11/14.
//
//

#include "SpacingProfileDetector.h"
#include "ThinProfileSkDet.cpp"

namespace cmp {
    
    SpacingProfileDetector::SpacingProfileDetector() : ContourWordSkewDetector(){
        
    }
    
    SpacingProfileDetector::~SpacingProfileDetector(){
        
    }
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point> *> &contours, double lineK, double& probability, cv::Mat* debugImage){
        
        spaceCount = contours.size()-1;
        
        assert(spaceCount>0);
        
        std::vector<cv::Point> spacingCountours;
        double hist[180];
        
        ThinProfileSkDet detector = ThinProfileSkDet();
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> spaceProfile;
            createMat(contours[i+1][0], contours[i][0], spaceProfile);
            detector.voteInHistogram(spaceProfile, hist, 1);
            
        }
        
        return 0;
        
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
        
        invertMerge(firstFaceContour, secondFaceContour, outputCont);
        
    }
    
    void SpacingProfileDetector::getFace(std::vector<cv::Point> input, std::vector<cv::Point> output, bool getLeft){
        
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
    
    void SpacingProfileDetector::invertMerge(std::vector<cv::Point> firstFace, std::vector<cv::Point> secondFace, std::vector<cv::Point> &outputCont){
        
        //move both countours to x=0
        
    }
    
    void SpacingProfileDetector::deOffset(std::vector<cv::Point *>& cont){
        
        for (int i=0; i<cont.size(); i++) {
            
        }
        
    }
    
    
}