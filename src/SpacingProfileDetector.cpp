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
        
        std::vector<std::vector<cv::Point>* > contours;
        std::vector<std::vector<cv::Point> > contoursWall;
        std::vector<cv::Rect> bounds;
        
        std::vector<double> widths;
        std::vector<double> angles;
        
        
        contoursWall.reserve(blobs.size());
        
        for (Blob &blob : blobs) {
            
            std::vector<std::vector<cv::Point> > tempContours;
            std::vector<cv::Vec4i> hierarchy;
            
            cv::findContours(blob.mask.clone(), tempContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
            
            ContourSkewDetector::getBigestContour(tempContours, hierarchy);
            
            contoursWall.push_back(tempContours[0]);
            contours.push_back(&contoursWall.back());
            bounds.push_back(blob.bBox);
            
        }
        return detectContoursSkew(contours, lineK, probability, debugImage, &bounds);
        
    }
    
    void SpacingProfileDetector::findProfiles(std::vector<cv::Point>& leftFace,std::vector<cv::Point>& rightFace,std::vector<double>& angles, std::vector<double>& widths){
        
        int topMostIndex=0, bottomMostIndex=0;
        
        std::vector<double> tempWidths, tempAngles;
        
        int maxXIndex=0, minXIndex=0;
        
        if (leftFace[0].y<leftFace[leftFace.size()-1].y) {
            
            topMostIndex = leftFace.size()-1;
            
        }
        
        if (rightFace[0].y>rightFace[rightFace.size()-1].y) {
            
            bottomMostIndex = rightFace.size()-1;
            
        }
        
        for (int i =0; i<rightFace.size(); i++) {
            
            rightFace[i].x < rightFace[minXIndex].x ? minXIndex=i:minXIndex;
            
        }
        for (int i =0; i<leftFace.size(); i++) {
            
            leftFace[i].x > leftFace [maxXIndex].x ? maxXIndex=i:maxXIndex;
            
        }
        
        int leftVertex = topMostIndex, leftVertex_next = topMostIndex;
        int rightVertex = bottomMostIndex, rightVertex_next = bottomMostIndex;
        bool control = true;
        
        bool hasNext_Right = true, hasNext_Left = true;
        
        while (control) {
            
            control = false;
            
            if (topMostIndex>bottomMostIndex) {
                
                if (leftVertex>0) leftVertex_next=leftVertex-1; else hasNext_Left = false;
                if (rightVertex<rightFace.size()-1) rightVertex_next=rightVertex+1; else hasNext_Right = false;
            }
            else if(topMostIndex<bottomMostIndex){
                
                if (leftVertex<leftFace.size()-1)leftVertex_next=leftVertex+1; else hasNext_Left = false;
                if (rightVertex>0) rightVertex_next=rightVertex-1; else hasNext_Right = false;
            }
            else {
                
                if (topMostIndex==0) {
                    if (rightVertex<rightFace.size()-1) rightVertex_next=rightVertex+1; else hasNext_Right = false;
                    if(leftVertex<leftFace.size()-1)leftVertex_next=leftVertex+1; else hasNext_Left = false;
                }
                
                else {
                    
                    if (rightVertex>0) rightVertex_next=rightVertex-1; else hasNext_Right = false;
                    if (leftVertex>0) leftVertex_next=leftVertex-1; else hasNext_Left = false;
                }
            }
            
            cv::Point leftEdge = leftFace[leftVertex]-leftFace[leftVertex_next];
            cv::Point rightEdge = rightFace[rightVertex]-rightFace[rightVertex_next];
            
            double angle =0;
            double width =-1;
            
            double angleA=0;
            
            leftEdge.x > 0 ? angleA =atan(leftEdge.y/leftEdge.x) : M_PI_2;
            
            double angleB =0;
            
            rightEdge.x > 0 ? angleB = atan(rightEdge.y/rightEdge.x) : M_PI_2;
            
            
            
            if ((angleA < angleB && hasNext_Left) || (!hasNext_Right && hasNext_Left)) {
                
                if (testBounds(leftEdge, leftFace[leftVertex_next], rightFace,true)){
                    
                    width = getWidth(getLine(leftEdge, leftFace[leftVertex_next]), rightFace[rightVertex]);
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
                leftVertex = leftVertex_next;
                control = true;
            }
            
            else if (hasNext_Right){
                
                if (testBounds(rightEdge, rightFace[rightVertex_next], leftFace, false)){
                    
                    width = getWidth(getLine(rightEdge, rightFace[rightVertex_next]), leftFace[leftVertex]);
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
                rightVertex = rightVertex_next;
                control = true;
            }
            
            if (width > 0/* && angle >= (M_PI/180*IGNORE_ANGLE-M_PI/2) && angle <= (M_PI/2-M_PI/180*IGNORE_ANGLE)*/) {
                
                tempWidths.push_back(width);
                tempAngles.push_back(angle);
                
            }
        }
        
        assert(tempAngles.size()==tempWidths.size());
        
        double index=0;
        
        if (widths.size()==0) {
            return;
        }
        
        for (int i =0; i<tempWidths.size(); i++) {
            
            if (tempWidths[index]<tempWidths[i]) {
                index = i;
            }
        }
        
        angles.push_back(tempAngles[index]);
        widths.push_back(tempWidths[index]);
    }
    
    //FIXME
    
    bool SpacingProfileDetector::testBounds(cv::Point& edge, cv::Point& pivotVertex, std::vector<cv::Point>& opposingFace, bool convex){
        
        int bottomMostVertex=0;
        int topMostVertex;
        int furthestVertex=0;
        int increment;
        
        cv::Vec3d line = getLine(edge, pivotVertex);
        bool maxXcrossed = false;
        
        if(opposingFace[0].y>opposingFace[opposingFace.size()-1].y){
            bottomMostVertex=(opposingFace.size()-1);
            increment=-1;
        }
        else{
            topMostVertex=opposingFace.size()-1;
            increment=+1;
        }
        
        for (int i =0; i< opposingFace.size(); i++) {
            
            if (convex){
                opposingFace[i].x > opposingFace[furthestVertex].x ? furthestVertex = i:furthestVertex;
            }
            else{
                opposingFace[i].x < opposingFace[furthestVertex].x ? furthestVertex = i:furthestVertex;
            }
            
        }
        
        for (int i=bottomMostVertex; i!=topMostVertex; i+=increment) {
            
            double yCoord = (line[0]*opposingFace[i].x+line[2])/(-line[1]);
            
            if (maxXcrossed) {
                
                if (yCoord<opposingFace[i].y && yCoord>opposingFace[furthestVertex].y) return false;
                
            }
            else {
                
                if (yCoord>opposingFace[i].y && yCoord>opposingFace[furthestVertex].y) return false;
                
            }
            
            if (i==furthestVertex) {
                maxXcrossed = true;
            }
        }
        
        return true;
        
    }
    
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point>* > &contours, double lineK, double& probability, cv::Mat* debugImage, std::vector<cv::Rect>* bounds){
        
        spaceCount = contours.size()-1;
        assert(spaceCount>0);
        std::vector<int> yPos;
        std::vector<int> xPos;
        
        assert(bounds->size()==contours.size());
        
        for (int i=0; i<bounds->size(); i++) {
            
            cv::Rect rect = (*bounds)[i];
            xPos.push_back(rect.x);
            yPos.push_back(rect.y);
            
        }
        
        std::vector<double> widths;
        std::vector<double> angles;
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> leftChar = *contours.at(i);
            std::vector<cv::Point> rightChar = *contours.at(i+1);
            
            std::vector<cv::Point> frontFace;
            std::vector<cv::Point> backFace;
            
            std::vector<cv::Point> leftChar_Convex;
            cv::convexHull(leftChar, leftChar_Convex);
            
            std::vector<cv::Point> rightChar_Convex;
            cv::convexHull(rightChar, rightChar_Convex);
            
            getFace(leftChar_Convex, frontFace);
            getFace(rightChar_Convex, backFace, true);
            
            int yOffset=0;
            int xOffset=0;
            yOffset = abs(yPos[i] - yPos[i+1]);
            xOffset = abs(xPos[i] + (*bounds)[i].width - xPos[i+1]) ;
            
            if (yPos[i]>yPos[i+1]) {
                deOffset(frontFace,0,yOffset);
                
                double xmax =0;
                for(cv::Point pt : frontFace){
                    xmax = MAX(pt.x, xmax);
                }
                
                deOffset(backFace,xOffset+xmax,0);
            }
            else{
                deOffset(frontFace,0,0);
                double xmax =0;
                
                for(cv::Point pt : frontFace){
                    xmax = MAX(pt.x, xmax);
                }
                
                deOffset(backFace,xOffset+xmax,yOffset);
            }
            
            cv::Mat img1(cv::Mat::zeros(100, 100, CV_8UC3));
            
            std::vector<std::vector<cv::Point> > ctr;
            ctr.push_back(frontFace);
            ctr.push_back(backFace);
            
            cv::drawContours(img1, ctr, 0, cv::Scalar(255,80,255),1,8);
            
            cv::drawContours(img1, ctr, 1, cv::Scalar(255,80,255));
            
            findProfiles(frontFace, backFace,angles,widths);
            
            /*if (widths.size()==0) {
                cv::imshow(" ", img1);
                cv::waitKey(0);
            }*/

        }
        double min_width=DBL_MAX;
        double max_width=0;
        
        for (double w : widths) {
            min_width = MIN(w, min_width);
            max_width = MAX(w, max_width);
        }
        
        double profilesRange=0.02;
        double thinProfilesRange = min_width * ( profilesRange + 1 );
        
        double histColWidth=1;
        double probMeasure1 = 0;
        
        assert(angles.size()==widths.size());
        
        //set values to histogram
        double hist[180];
        memset (hist, 0, 180 * sizeof(double));
        int sigma = 3, range = 3;
        // counting all profiles in thinProfilesRange
        double maxHistValue = 0;
        for(int c=0;c<widths.size();c++)
        {
            double ang = angles[c] * 180/M_PI;;
            for (int i = ang-sigma*range; i <= ang+sigma*range; i++)
            {
                int j = i;
                if (j<0) j += int(180/histColWidth);
                if (j>=int(180/histColWidth)) j -= int(180/histColWidth);
                assert(widths[c] != 0);
                double length = 1/widths[c];
                hist[j] +=  length/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i*histColWidth+histColWidth/2-ang)*(i*histColWidth+histColWidth/2-ang)/(2*sigma*sigma));
                maxHistValue = MAX(maxHistValue, hist[j]);
            }
            
            if( (widths[c] <= thinProfilesRange ))
            {
                probMeasure1++;
            }
        }
        
        int height = 300;
        
        cv::Mat histogram = cv::Mat::zeros(height, 380, CV_8UC3);
        int maxI = ceil(double(IGNORE_ANGLE/histColWidth));
        
        double totalLen = 0.0;
        double resLen = 0.0;
        
        for(int i=0;i<int(180/histColWidth);i++)
        {
            int rectH = hist[i] / maxHistValue * (height - 20);
            cv::rectangle(histogram, cv::Rect(10+histColWidth*i*2, height-rectH-10, histColWidth*2, rectH), cv::Scalar(0,0,255), CV_FILLED);
            if (i > IGNORE_ANGLE/histColWidth && i < (180-IGNORE_ANGLE)/histColWidth)
            {
                if (hist[i] > hist[maxI]) maxI = i;
                totalLen += hist[i];
            }
        }
        for (int i = maxI-sigma*range; i <= maxI+sigma*range; i++)
        {
            if (i > IGNORE_ANGLE/histColWidth && i < (180-IGNORE_ANGLE)/histColWidth)
            {
                int j = i;
                if (j<0) j = j + int(180/histColWidth);
                if (j>=int(180/histColWidth)) j = j - int(180/histColWidth);
                
                resLen += hist[j];
            }
        }
        
        if(totalLen > 0)
            probMeasure2 = (resLen/totalLen);
        //imshow("Hist", histogram);
        //cv::waitKey(0);
        
        
        double width=0;
        double index=0;
        
        for (int i=0; i<180; i++) {
            
            if (hist[i]>=width) {
                width = hist[i];
                index = i;
            }
            
        }
        
        double ang = index*(M_PI/180);
        
        debugImage = &histogram;
        
        return ang;
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
        double c = (-ax)*point.x-by*point.y;
        
        return cv::Vec3d{ax,by,c};
    }
    double SpacingProfileDetector::getWidth(cv::Vec3d line, cv::Point point){
        
        return (fabs(line[0]*point.x+line[1]*point.y+line[2]) / sqrt(line[0]*line[0]+line[1]*line[1]) );
        
    }
    
}