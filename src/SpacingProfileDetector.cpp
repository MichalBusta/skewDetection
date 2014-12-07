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
        std::vector<int> xPos;
        
        std::vector<double> widths;
        std::vector<double> angles;
        
        
        spaceCount = contours.size()-1;
        assert(spaceCount>0);
        
        for (Blob blob : blobs) {
            
            std::vector<std::vector<cv::Point> > tempContours;
            std::vector<cv::Vec4i> hierarchy;
            
            cv::findContours(blob.mask, tempContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
            
            ContourSkewDetector::getBigestContour(tempContours, hierarchy);
            
            contours.push_back(tempContours[0]);
            yPos.push_back(blob.bBox.y);
            xPos.push_back(blob.bBox.x);
            
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
            int xOffset=0;
            yOffset = abs(yPos[i] - yPos[i+1]);
            xOffset = xPos[i] - xPos[i+1];
            
            if (yPos[i]>yPos[i+1]) {
                deOffset(frontFace,0,yOffset);
                deOffset(backFace,xOffset,0);
            }
            else{
                deOffset(backFace,0,yOffset);
                deOffset(frontFace,xOffset,0);
            }
            
            findProfiles(frontFace, backFace,angles,widths);
            
        }
        
        return 0;
        
        
    }
    
    void SpacingProfileDetector::findProfiles(std::vector<cv::Point> leftFace,std::vector<cv::Point> rightFace,std::vector<double> angles, std::vector<double> widths){
        
        int topMostIndex=0, bottomMostIndex=0;
        
        int maxXIndex=0, minXIndex=0;
        
        if (leftFace[0].y<leftFace[leftFace.size()-1].y) {
            
            topMostIndex = leftFace.size();
            
        }
        
        if (rightFace[0].y>rightFace[rightFace.size()-1].y) {
            
            bottomMostIndex = rightFace.size();
            
        }
        
        for (int i =0; i<rightFace.size(); i++) {
            
            rightFace[i].x < rightFace[minXIndex].x ? minXIndex=i:minXIndex;
            
        }
        for (int i =0; i<leftFace.size(); i++) {
            
            leftFace[i].x > leftFace [maxXIndex].x ? maxXIndex=i:maxXIndex;
            
        }
        
        int leftVertex = topMostIndex, leftVertex_next;
        int rightVertex = bottomMostIndex, rightVertex_next;
        
        bool xMax_left = false, xMax_right = false;
        
        topMostIndex == maxXIndex ? xMax_left=true:false;
        bottomMostIndex == minXIndex ? xMax_right=true:false;
        
        while (true) {
            
            if (topMostIndex>bottomMostIndex) {
                leftVertex_next=leftVertex-1;
                rightVertex_next=rightVertex+1;
            }
            else{
                leftVertex_next=leftVertex+1;
                rightVertex_next=rightVertex-1;
            }
            
            cv::Point leftEdge = leftFace[leftVertex]-leftFace[leftVertex_next];
            cv::Point rightEdge = rightFace[rightVertex]-rightFace[rightVertex_next];
            
            double angle =0;
            double width =-1;
            
            double angleA = atan(leftEdge.y/leftEdge.x);
            double angleB = atan(rightEdge.y/rightEdge.x);
            
            
            if (angleA < angleB) {
                
                leftVertex=leftVertex_next;
                
                if (testBounds(leftEdge, leftFace[leftVertex], rightFace)){
                    
                    width = getWidth(getLine(leftEdge, leftFace[leftVertex]), rightFace[rightVertex]);
                    angle=angleA;
                    
                    angle = angle + M_PI/2;
                    while (angle > M_PI/2) angle = angle - M_PI;
                    while (angle <= -M_PI/2) angle = angle+ M_PI;
                    
                    if(true)
                    {
                        if(fabs(cos(angle)) != 0 )
                            width = width / fabs(cos(angle));
                    }
                }
            }
            
            if (angleA > angleB) {
                
                rightVertex = rightVertex_next;
                
                if (testBounds(rightEdge, rightFace[rightVertex], leftFace)){
                    
                    width = getWidth(getLine(rightEdge, rightFace[rightVertex]), leftFace[leftVertex]);
                    angle=angleB;
                    
                    angle = angle + M_PI/2;
                    while (angle > M_PI/2) angle = angle - M_PI;
                    while (angle <= -M_PI/2) angle = angle+ M_PI;
                    
                    if(true)
                    {
                        if(fabs(cos(angle)) != 0 )
                            width = width / fabs(cos(angle));
                    }
                }
            }
            
            if (width > 0 && angle >= (M_PI/180*IGNORE_ANGLE-M_PI/2) && angle <= (M_PI/2-M_PI/180*IGNORE_ANGLE)) {
                
                widths.push_back(width);
                angles.push_back(angle);
                
            }
        }
    }
    
    bool SpacingProfileDetector::testBounds(cv::Point edge, cv::Point pivotVertex, std::vector<cv::Point> opposingFace){
        
        int bottomMostVertex=0;
        int furthestVertex=0;
        
        cv::Vec3d line = getLine(edge, pivotVertex);
        bool maxXcrossed = false;
        
        opposingFace[0].y>opposingFace[opposingFace.size()-1].y ? bottomMostVertex=(opposingFace.size()-1):0;
        
        for (int i =0; i< opposingFace.size(); i++) {
            
            opposingFace[i].x > opposingFace[furthestVertex].x ? furthestVertex = i:furthestVertex;
            
        }
        
        for (int i=0; i<opposingFace.size(); i++) {
            
            double yCoord = (line[0]*opposingFace[i].x+line[2])/line[1];
            
            if (maxXcrossed) {
                
                if (yCoord<opposingFace[i].y) return false;
                
            }
            else {
                
                if (yCoord>opposingFace[i].y) return false;
                
            }
            
            if (i==furthestVertex) {
                maxXcrossed = true;
            }
        }
        
        return true;
        
    }
    
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point> *> &contours, double lineK, double& probability, cv::Mat* debugImage, std::vector<cv::Rect>* bounds){
        
        std::vector<int> yPos;
        std::vector<int> xPos;
        
        for (cv::Rect rect : *bounds) {
            
            xPos.push_back(rect.x);
            yPos.push_back(rect.y);
            
        }
        
        std::vector<double> widths;
        std::vector<double> angles;
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> leftChar = *contours[i];
            std::vector<cv::Point> rightChar = *contours[i+1];
            
            std::vector<cv::Point> frontFace;
            std::vector<cv::Point> backFace;
            
            std::vector<cv::Point> leftChar_Convex;
            cv::convexHull(leftChar, leftChar_Convex);
            
            std::vector<cv::Point> rightChar_Convex;
            cv::convexHull(rightChar, rightChar_Convex);
            
            getFace(leftChar_Convex, frontFace);
            getFace(rightChar_Convex, backFace);
            
            int yOffset=0;
            int xOffset=0;
            yOffset = abs(yPos[i] - yPos[i+1]);
            xOffset = xPos[i] - xPos[i+1];
            
            if (yPos[i]>yPos[i+1]) {
                deOffset(frontFace,0,yOffset);
                deOffset(backFace,xOffset,0);
            }
            else{
                deOffset(backFace,0,yOffset);
                deOffset(frontFace,xOffset,0);
            }
            
            findProfiles(frontFace, backFace,angles,widths);
        }
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
    
    cv::Vec3d SpacingProfileDetector::getLine(cv::Point edge, cv::Point point){
        
        double ax = edge.y;
        double by = -edge.x;
        double c = -ax*point.x-by*point.y;
        
        return cv::Vec3d{ax,by,c};
    }
    double SpacingProfileDetector::getWidth(cv::Vec3d line, cv::Point point){
        
        return (fabs(line[0]*point.x+line[1]*point.y+line[2]) / sqrt(line[0]*line[0]+line[1]*line[1]) );
        
    }
    
}