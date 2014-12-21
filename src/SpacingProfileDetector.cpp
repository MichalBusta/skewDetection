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

#define VERBOSE 1
#define SPACING 5

namespace cmp {
    
    inline void moveContour(std::vector<cv::Point >& cont, cv::Point pt){
        
        for (int i=0; i<cont.size(); i++) {
            
            cont[i] =cont[i]-pt;
            
        }
        
    }
    
    
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
        return detectContoursSkew(contours, lineK, probability, (debugImage), &bounds);
        
    }
    
    void SpacingProfileDetector::findProfiles(std::vector<cv::Point>& leftFace,std::vector<cv::Point>& rightFace,std::vector<double>& angles, std::vector<double>& widths, VisData* visData ,cv::Mat* debugImage){
        
        int topMostIndex=0, bottomMostIndex = rightFace.size()-1;
        
        std::vector<double> tempWidths, tempAngles;
        std::vector<size_t> pivotPoints, opposingPoints;
        std::vector<cv::Point> profiles;
        
        int maxXIndex=0, minXIndex=0;
        
        assert (leftFace[0].y<leftFace[leftFace.size()-1].y);
        
        assert(topMostIndex<bottomMostIndex);
        
        assert (rightFace[0].y<rightFace[rightFace.size()-1].y);
        
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
            
            cv::Point rightEdge;
            cv::Point leftEdge;
            
            leftEdge = leftFace[leftVertex_next]-leftFace[leftVertex];
            rightEdge = rightFace[rightVertex]-rightFace[rightVertex_next];
            
            cv::Point nonSense(0,0);
            
            double angle =0;
            double width =-1;
            
            double angleA=0;
            double angleB =0;

            assert(leftEdge!=nonSense && rightEdge!=nonSense);
            assert(leftEdge.y!=0 && rightEdge.y!=0);
            
            angleA = acos(leftEdge.x/sqrt((leftEdge.x*leftEdge.x)+(leftEdge.y*leftEdge.y)));
            angleB = acos(rightEdge.x/sqrt((rightEdge.x*rightEdge.x)+(rightEdge.y*rightEdge.y)));
            
            if (angleA <= angleB) {
                
                assert(leftEdge != nonSense);
                
                if (testBounds(leftEdge, leftFace[leftVertex_next], rightFace,false) &&
                    testBounds(leftEdge, rightFace[rightVertex], leftFace,true)){
                    
                    width = getWidth(getLine(leftEdge, leftFace[leftVertex_next]), rightFace[rightVertex]);
                    angle=angleA;

                }
                
                if (angleA<0) {
                    angleA +=M_PI;
                }
                
                leftVertex = hasNext_Left ? leftVertex_next : leftVertex;
                
                if (hasNext_Left) control = true;
                
                if (width > 0 && angle >= (M_PI/180*IGNORE_ANGLE) && angle <= M_PI-(M_PI/180*IGNORE_ANGLE)) {
                    
                    tempWidths.push_back(width);
                    tempAngles.push_back(angle);
                    pivotPoints.push_back(leftVertex_next);
                    profiles.push_back(leftEdge);
                    opposingPoints.push_back(rightVertex);
                    
                }
            }
            
            if(angleA >= angleB) {
                
                assert(rightEdge != nonSense);
                
                if (testBounds(rightEdge, rightFace[rightVertex_next], leftFace, true) &&
                    testBounds(rightEdge, leftFace[leftVertex], rightFace, false)){
                    
                    width = getWidth(getLine(rightEdge, rightFace[rightVertex_next]), leftFace[leftVertex]);
                    angle=angleB;

                }
                
                if (angleB<0) {
                    angleB +=M_PI;
                }
                
                rightVertex = hasNext_Right ? rightVertex_next : rightVertex;
                
                if (hasNext_Right) control = true;
                
                if (width > 0 && angle >= (M_PI/180*IGNORE_ANGLE) && angle <= M_PI-(M_PI/180*IGNORE_ANGLE)) {
                    
                    tempWidths.push_back(width);
                    tempAngles.push_back(angle);
                    pivotPoints.push_back(rightVertex_next);
                    profiles.push_back(rightEdge);
                    opposingPoints.push_back(leftVertex);
                    
                }
                
            }
            
        }
        
        assert(tempAngles.size()==tempWidths.size());
        
        double index=0;
        
        if (tempWidths.size()==0) {
            return;
        }
        
        for (int i =0; i<tempWidths.size(); i++) {
            
            angles.push_back(tempAngles[i]);
            widths.push_back(tempWidths[i]);
            
            if (tempWidths[index]<tempWidths[i]) {
                index = i;
            }
        }
        
        visData->pivots.push_back(std::pair<size_t,size_t>(pivotPoints[index], opposingPoints[index]));
        visData->profiles.push_back(profiles[index]);
        
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
        
        int i=bottomMostVertex;
        
        while (true) {
            
            if (i==(topMostVertex+increment)) break;
            
            double yCoord = (line[0]*opposingFace[i].x+line[2])/(-line[1]);
            
            if (i==furthestVertex) {
                
                maxXcrossed = true;
                i+=increment;
                continue;
                
            }
            
            if (maxXcrossed) {
                
                if (yCoord>opposingFace[i].y && yCoord<opposingFace[furthestVertex].y) return false;
                
            }
            else {
                
                if (yCoord<opposingFace[i].y && yCoord>opposingFace[furthestVertex].y) return false;
                
            }
            i+=increment;
            
        }
        
        return true;
        
    }
    
    
    double SpacingProfileDetector::detectContoursSkew(std::vector<std::vector<cv::Point>* > &contours, double lineK, double& probability, cv::Mat* debugImage, std::vector<cv::Rect>* bounds){
        
        spaceCount = contours.size()-1;
        assert(spaceCount>0);
        std::vector<int> yPos;
        std::vector<int> xPos;
        std::vector<double> widths;
        std::vector<double> angles;
        std::vector<cv::Mat> profileImages;
        std::vector<std::vector<cv::Point> > convexChars;
        std::vector<std::pair<std::vector<size_t>,std::vector<size_t>>> facePointIndices;
        double hist;
        cv::Mat histogram;
        
        assert(bounds->size()==contours.size());
        
        for (int i=0; i<bounds->size(); i++) {
            
            cv::Rect rect = (*bounds)[i];
            xPos.push_back(rect.x);
            yPos.push_back(rect.y);
            
        }
        
        
        int xOffset = xPos[0];
        int xShift=0;
        for (int i=0; i<contours.size(); i++) {
            
            std::vector<cv::Point> contour = *contours.at(i);
            
            std::vector<cv::Point> convexContour;
            cv::convexHull(contour, convexContour);
            
            if (i> 0 && xPos[i-1]+(*bounds)[i-1].width<(xPos[i]-5)) {
                
                moveContour(convexContour, cv::Point(-(xPos[i]+xShift-xOffset),-yPos[i]));
                
            }
            
            else{
                
                xShift +=10;
                moveContour(convexContour, cv::Point(-(xPos[i]+xShift-xOffset),-yPos[i]));
                
            }
            
            convexChars.push_back(convexContour);

        }
        
        
        VisData visData;
        int yMax = 0;
        int xSum =0;
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<cv::Point> leftChar =convexChars[i];
            std::vector<cv::Point> rightChar = convexChars[i+1];
            
            std::vector<cv::Point> frontFace, backFace;
            
            std::vector<size_t> frontIndices, backIndices;
            
            getFace(leftChar, frontFace, false, &frontIndices);
            getFace(rightChar, backFace, true, &backIndices);
            
            facePointIndices.push_back(std::pair<std::vector<size_t>,std::vector<size_t>>(frontIndices,backIndices));
            
            assert(frontFace.size()>1);
            assert(backFace.size()>1);
            
            cv::Point offset(cv::boundingRect(frontFace).x,
                             MIN(cv::boundingRect(frontFace).y, cv::boundingRect(backFace).y));
            
            moveContour(frontFace, offset);
            moveContour(backFace, offset);
            
            int tempXmax=0, tempXmin=INT16_MAX;
            int width = 0;
            int tempYMax = 0;
            
            for (cv::Point p : frontFace) {
                
                tempYMax = MAX(tempYMax, p.y);
                tempXmin = MIN(tempXmin, p.x);
                
            }
            for (cv::Point p : backFace) {
                
                tempYMax = MAX(tempYMax, p.y);
                tempXmax = MAX(tempXmax, p.x);
            }
            
            width = tempXmax-tempXmin+10;
            
            cv::Mat temp = cv::Mat::zeros(tempYMax+=10, width, CV_8UC3);
            
            std::vector<std::vector<cv::Point> > ctr;
            ctr.push_back(frontFace);
            ctr.push_back(backFace);
            
            cv::drawContours(temp, ctr, 0, cv::Scalar(255,80,255));
            
            cv::drawContours(temp, ctr, 1, cv::Scalar(255,80,255));
            
            findProfiles(frontFace, backFace,angles,widths, &visData);
            
            yMax = MAX(yMax, tempYMax);
            xSum +=width+SPACING;
            
#if VERBOSE
            cv::imshow(" ", temp);
            cv::waitKey(0);
#endif
        }
        
        cv::Mat debug;
        
        drawSpaceProfiles(debug,convexChars, facePointIndices, visData);
        
        double angle = createHistogram(&hist, widths, angles, &histogram);
        
        
#if VERBOSE
        imshow("Hist", histogram);
        cv::waitKey(0);
#endif
        
        
#if VERBOSE
        imshow("debug", debug);
        cv::waitKey(0);
#endif
        *debugImage = debug;
        
        return angle;
    }
    
    void SpacingProfileDetector::getFace(std::vector<cv::Point> &input, std::vector<cv::Point> &output, bool getLeft, std::vector<size_t>* indices){
        
        output = std::vector<cv::Point>();
        std::vector<size_t> tempIndices;
        
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
                        
                        if (input[i].x < output.back().x) {
                            output.pop_back();
                            tempIndices.pop_back();
                            
                            output.push_back(input[i]);
                            tempIndices.push_back(i);
                        }
                    }
                    else {
                        
                        output.push_back(input[i]);
                        tempIndices.push_back(i);
                        
                    }
                    
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
                        
                        if (input[i].x < output.back().x) {
                            output.pop_back();
                            tempIndices.pop_back();
                            
                            output.push_back(input[i]);
                            tempIndices.push_back(i);
                        }
            
                    }
                    else {
                        
                        output.push_back(input[i]);
                        tempIndices.push_back(i);
                        
                    }

                    
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
                        
                        if (input[i].x > output.back().x) {
                            output.pop_back();
                            tempIndices.pop_back();
                            
                            output.push_back(input[i]);
                            tempIndices.push_back(i);
                        }
                    }
                    else {
                        
                        output.push_back(input[i]);
                        tempIndices.push_back(i);
                        
                    }

                    
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
                        if (input[i].x > output.back().x) {
                            output.pop_back();
                            tempIndices.pop_back();
                            
                            output.push_back(input[i]);
                            tempIndices.push_back(i);
                        }
                    }
                    else {
                        
                        output.push_back(input[i]);
                        tempIndices.push_back(i);
                        
                    }

                    
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
        
        *indices = tempIndices;
        
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
                continue;
                color = ptColor;
            }
            
            cv::circle(img, pivotPoints[i], 1, color, 2);
            cv::circle(img, opposingPoints[i], 1, color, 2);
            cv::line(img, pivotPoints[i]-profiles[i]*100, pivotPoints[i]+profiles[i]*100, color);
            cv::line(img, opposingPoints[i]-profiles[i]*100, opposingPoints[i]+profiles[i]*100, color);
            
        }
        
    }
    
    void SpacingProfileDetector::drawSpaceProfiles(cv::Mat &img, std::vector<std::vector<cv::Point> > &characters, std::vector<std::pair<std::vector<size_t>, std::vector<size_t> > > &facePointIndices, cmp::VisData &visData){
        
        int imgWidth=0;
        int imgHeight=0;
        cv::Mat tempImg;
        cv::Scalar contourColor(255,180,50);
        cv::Scalar faceColor(40,150,255);
        cv::Scalar pivotColor(255,60,60);
        cv::Scalar profileColor(255,50,255);
        
        for (int i=0; i<characters.size(); i++) {
            cv::Rect bounds = cv::boundingRect(characters[i]);
            imgWidth += bounds.x;
            imgHeight = MAX(bounds.height+bounds.y, imgHeight);
            
            if (i==characters.size()-1) {
                imgWidth += bounds.width;
            }
        }
        
        imgHeight += 10;
        imgWidth += 20;
        
        tempImg = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
        
        for (int i=0; i<characters.size(); i++) {
            
            cv::drawContours(tempImg, characters, i,contourColor);
            
        }
        
        for (int j=0 ; j<spaceCount; j++) {
            
            std::vector<size_t> front = facePointIndices[j].first;
            std::vector<size_t> back = facePointIndices[j].second;
            
            for (int i=0; i<front.size()-1; i++) {
                
                cv::line(tempImg, characters[j][front[i]], characters[j][front[i+1]], faceColor);
                
            }
            
            for (int i=0; i<back.size()-1; i++) {
                
                cv::line(tempImg, characters[j+1][back[i]], characters[j+1][back[i+1]], faceColor);
                
            }
        }
        
        for (int i=0; i<spaceCount; i++) {
            
            std::vector<size_t> front = facePointIndices[i].first;
            std::vector<size_t> back = facePointIndices[i].second;
            
            cv::circle(tempImg, characters[i][front[visData.pivots[i].first]], 2, pivotColor,3);
            cv::circle(tempImg, characters[i+1][back[visData.pivots[i].second]], 2, pivotColor,3);
            
            cv::line(tempImg, characters[i][front[visData.pivots[i].first]]-visData.profiles[i]*100, characters[i][front[visData.pivots[i].first]]+visData.profiles[i]*100, profileColor);
            cv::line(tempImg, characters[i+1][back[visData.pivots[i].second]]-visData.profiles[i]*100, characters[i+1][back[visData.pivots[i].second]]+visData.profiles[i]*100, profileColor);
            
        }
        
        img =  tempImg;
        
    }
    
    double SpacingProfileDetector::createHistogram(double* histogram, std::vector<double> widths, std::vector<double> angles, cv::Mat* debugImg){
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
            double ang = angles[c] * 180/M_PI;
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
        
        cv::Mat img = cv::Mat::zeros(height, 380, CV_8UC3);
        int maxI = ceil(double(IGNORE_ANGLE/histColWidth));
        
        double totalLen = 0.0;
        double resLen = 0.0;
        
        for(int i=0;i<int(180/histColWidth);i++)
        {
            int rectH = hist[i] / maxHistValue * (height - 20);
            cv::rectangle(img, cv::Rect(10+histColWidth*i*2, height-rectH-10, histColWidth*2, rectH), cv::Scalar(0,0,255), CV_FILLED);
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
        }
        
        double result = maxI*(M_PI/180);
        
        histogram = hist;
        *debugImg = img;
        
        return result;
        
    }
}