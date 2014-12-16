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
    
    void SpacingProfileDetector::findProfiles(std::vector<cv::Point>& leftFace,std::vector<cv::Point>& rightFace,std::vector<double>& angles, std::vector<double>& widths, cv::Mat* debugImage){
        
        int topMostIndex=0, bottomMostIndex = rightFace.size()-1;
        
        std::vector<double> tempWidths, tempAngles;
        std::vector<cv::Point> profiles, pivotPoints, opposingPoints;
        
        int maxXIndex=0, minXIndex=0;
        
        assert (leftFace[0].y<=leftFace[leftFace.size()-1].y);
        
        assert(topMostIndex<bottomMostIndex);
        
        assert (rightFace[0].y<=rightFace[rightFace.size()-1].y);
        
        for (int i =0; i<rightFace.size(); i++) {
            
            minXIndex = rightFace[i].x < rightFace[minXIndex].x ? i : minXIndex;
            
        }
        for (int i =0; i<leftFace.size(); i++) {
            
            maxXIndex = leftFace[i].x > leftFace [maxXIndex].x ? i : maxXIndex;
            
        }
        
        int leftVertex = topMostIndex, leftVertex_next = topMostIndex;
        int rightVertex = bottomMostIndex, rightVertex_next = bottomMostIndex;
        
        bool control = true;
        
        bool hasNext_Right = true, hasNext_Left = true;
        
        while (control) {
            
            control = false;
                
            if (leftVertex<leftFace.size()-1 && hasNext_Left){
                
                leftVertex_next=leftVertex+1;
                
            }
            
            else if(hasNext_Left){
                
                leftVertex_next=leftVertex;
                leftVertex--;
                
                hasNext_Left = false;
                
            }
            
            if (rightVertex>0 && hasNext_Right){
                
                rightVertex_next=rightVertex-1;
                
            }
            else if(hasNext_Right) {
                
                rightVertex_next=rightVertex;
                rightVertex++;
                
                hasNext_Right = false;
                
            }
            
            cv::Point leftEdge = leftFace[leftVertex]-leftFace[leftVertex_next];
            cv::Point rightEdge = rightFace[rightVertex]-rightFace[rightVertex_next];
            cv::Point nonSense(0,0);
            
            double angle =0;
            double width =-1;
            
            double angleA=0;
            double angleB =0;

            
            assert(leftEdge!=nonSense && rightEdge!=nonSense);
            assert(leftEdge.y!=0 && rightEdge.y!=0);
            
            angleA = leftEdge.x > 0 ? atan(leftEdge.y/leftEdge.x) : M_PI_2;

            angleB = rightEdge.x > 0 ? atan(rightEdge.y/rightEdge.x) : M_PI_2;

            
            if (angleA <= angleB) {
                
                assert(leftEdge != nonSense);
                
                if (testBounds(leftEdge, leftFace[leftVertex_next], rightFace,true) &&
                    testBounds(leftEdge, rightFace[rightVertex], leftFace,false)){
                    
                    width = getWidth(getLine(leftEdge, leftFace[leftVertex_next]), rightFace[rightVertex]);
                    angle=angleA;
    
                    if (angle<0) {
                        angle += M_PI;
                    }
                    
                    if(true)
                    {
                        if(fabs(cos(angle)) != 0 )
                            width = width / fabs(cos(angle));
                    }
                }
                
                leftVertex= hasNext_Left ? leftVertex_next : leftVertex;
                
                if (hasNext_Left) control = true;
                
                if (width > 0 && angle >= (M_PI/180*IGNORE_ANGLE) && angle <= M_PI-(M_PI/180*IGNORE_ANGLE)) {
                    
                    tempWidths.push_back(width);
                    tempAngles.push_back(angle);
                    pivotPoints.push_back(leftFace[leftVertex_next]);
                    profiles.push_back(leftEdge);
                    opposingPoints.push_back(rightFace[rightVertex]);
                    
                }
            }
            
            else if (angleA >= angleB){
                
                assert(rightEdge != nonSense);
                
                if (testBounds(rightEdge, rightFace[rightVertex_next], leftFace, false) &&
                    testBounds(rightEdge, leftFace[leftVertex], rightFace, true)){
                    
                    width = getWidth(getLine(rightEdge, rightFace[rightVertex_next]), leftFace[leftVertex]);
                    angle=angleB;
       
                    if (angle<0) {
                        angle +=M_PI;
                    }
                    
                    if(true)
                    {
                        if(fabs(cos(angle)) != 0 )
                            width = width / fabs(cos(angle));
                    }
                }
                
                rightVertex = hasNext_Right ? rightVertex_next : rightVertex;
                
                if (hasNext_Right) control = true;
                
                if (width > 0 && angle >= (M_PI/180*IGNORE_ANGLE) && angle <= M_PI-(M_PI/180*IGNORE_ANGLE)) {
                    
                    tempWidths.push_back(width);
                    tempAngles.push_back(angle);
                    pivotPoints.push_back(rightFace[rightVertex_next]);
                    profiles.push_back(rightEdge);
                    opposingPoints.push_back(leftFace[leftVertex]);
                    
                }
                
            }
            
        }
        
        assert(tempAngles.size()==tempWidths.size());
        
        double index=0;
        
        if (tempWidths.size()==0) {
            return;
        }
        
        for (int i =0; i<tempWidths.size(); i++) {
            
            if (tempWidths[index]<tempWidths[i]) {
                index = i;
            }
        }
        
        angles.push_back(tempAngles[index]);
        widths.push_back(tempWidths[index]);
        
        drawProfiles(pivotPoints, opposingPoints ,profiles, index, *debugImage);
    }
    
    bool SpacingProfileDetector::testBounds(cv::Point& edge, cv::Point& pivotVertex, std::vector<cv::Point>& opposingFace, bool convex){
        
        int bottomMostVertex=0;
        int topMostVertex=0;
        int furthestVertex=0;
        int increment;
        
        cv::Vec3d line = getLine(edge, pivotVertex);
        bool maxXcrossed = false;
        
        if(opposingFace[0].y>opposingFace[opposingFace.size()-1].y){
            
            topMostVertex=opposingFace.size()-1;
            increment=+1;
        }
        else{
            bottomMostVertex=(opposingFace.size()-1);
            increment=-1;
        }
        
        for (int i =0; i< opposingFace.size(); i++) {
            
            if (convex){
                furthestVertex = opposingFace[i].x > opposingFace[furthestVertex].x ? i : furthestVertex;
            }
            else{
                furthestVertex = opposingFace[i].x < opposingFace[furthestVertex].x ? i : furthestVertex;
            }
            
        }
        
        for (int i=bottomMostVertex; i!=topMostVertex; i+=increment) {
            
            double yCoord = (line[0]*opposingFace[i].x+line[2])/(-line[1]);
            
            if (i==furthestVertex) {
                
                maxXcrossed = true;
                continue;
                
            }
            
            if (maxXcrossed) {
                
                if (yCoord>opposingFace[i].y && yCoord<opposingFace[furthestVertex].y) return false;
                
            }
            else {
                
                if (yCoord<opposingFace[i].y && yCoord>opposingFace[furthestVertex].y) return false;
                
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
            xOffset = abs(xPos[i] + (*bounds)[i].width - xPos[i+1])+5;
            
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
            
            findProfiles(frontFace, backFace,angles,widths, &img1);
            
            
            cv::imshow(" ", img1);
            cv::waitKey(0);

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
        
        assert(angles.size()!=0);
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
        
        if(totalLen > 0){
            probMeasure2 = (resLen/totalLen);
            probability = probMeasure2;
        }
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
        
        int next_plus =0, next_minus=0;
        
        next_minus = (top-1<0) ? input.size()-1 : top-1;
        
        if (top >= input.size()-1) {
            
            next_plus = 0;
            
        }
        else {
            
            next_plus=top+1;
            
        }
        
        assert(top!=bot);
        
        if (getLeft){
            
            if (input[next_plus].x < input[top].x || input[next_minus].x>input[top].x) {
                
                int i = top;
                bool control=true;
                
                while (true){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (output.size()>0 && input[i].y == output.back().y) {
                        output.pop_back();
                    }
                    
                    output.push_back(input[i]);
                    
                    if (i>=input.size()-1) {
                        
                        i = 0;
                        
                    }
                    else{
                        
                        i++;
                    }
                    
                    if (!control) {
                        break;
                    }
                    
                    if (i == bot) {
                        control=false;
                    }
                }
            }
            
            else {
                
                int i = top;
                bool control=true;
                
                while (true){
                    
                    assert(i>=0 && i<input.size());
                    
                    if (output.size()>0 && input[i].y == output.back().y) {
                        output.pop_back();
                    }
                    output.push_back(input[i]);
                    
                    if (i==0) {
                        
                        i = input.size()-1;
                    }
                    else{

                        i--;
                    }
                    
                    if (!control) {
                        break;
                    }
                    
                    if (i == bot) {
                        control=false;
                    }
                }
            }
        }
        
        else {
            
            bool control=true;
            
            if (input[next_plus].x>input[top].x || input[next_minus].x<input[top].x) {
                
                int i = top;
                
                while (true){
                    
                    assert(i>=0 && i<input.size());
                    if (output.size()>0 && input[i].y == output.back().y) {
                        output.pop_back();
                    }
                    output.push_back(input[i]);
                    
                    if (i>=input.size()-1) {
                        i = 0;
                    }
                    else{
                        i++;
                    }
                    
                    if (!control) {
                        break;
                    }
                    
                    if (i == bot) {
                        control=false;
                    }
                }
            }
            else {
                
                int i = top;
                bool control=true;
                
                while (true){
                    
                    assert(i>=0 && i<input.size());
                    if (output.size()>0 && input[i].y == output.back().y) {
                        output.pop_back();
                    }
                    output.push_back(input[i]);
                    
                    if (i==0) {
                        i = input.size()-1;
                    }
                    else{
                        i--;
                    }
                    
                    if (!control) {
                        break;
                    }
                    
                    if (i == bot) {
                        control=false;
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
        
        minY -= yOrigin;
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
    
    void SpacingProfileDetector::drawProfiles(std::vector<cv::Point>& pivotPoints, std::vector<cv::Point>& opposingPoints, std::vector<cv::Point>& profiles, int thinnestIndex, cv::Mat &img){
        
        assert(pivotPoints.size()==profiles.size() && pivotPoints.size()==opposingPoints.size());
        assert(thinnestIndex<pivotPoints.size());
        
        cv::Scalar ptColor(255,255,80);
        cv::Scalar thinColor(100,255,50);
        
        for (int i =0; i<pivotPoints.size(); i++) {
            
            cv::Scalar color;
            
            if (i==thinnestIndex) {
                color = thinColor;
            }
            else{
                color = ptColor;
            }
            
            cv::circle(img, pivotPoints[i], 1, color, 2);
            cv::circle(img, opposingPoints[i], 1, color, 2);
            cv::line(img, pivotPoints[i]-profiles[i]*100, pivotPoints[i]+profiles[i]*100, color);
            cv::line(img, opposingPoints[i]-profiles[i]*100, opposingPoints[i]+profiles[i]*100, color);
            
        }
        
    }
    
}