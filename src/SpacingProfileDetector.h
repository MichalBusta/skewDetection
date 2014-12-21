
//  SpacingProfileDetector.h
//  SkewDetection
//
//  Created by David Helekal on 15/11/14.
//
//

#ifndef __SkewDetection__SpacingProfileDetector__
#define __SkewDetection__SpacingProfileDetector__

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "SkewDetector.h"
#include <math.h>
#include "WordSkewDetector.h"

#endif /* defined(__SkewDetection__SpacingProfileDetector__) */

namespace cmp {
    
    struct VisData{
        
        std::vector<cv::Point> profiles;
        std::vector<std::pair<size_t, size_t>> pivots;
        
    };
    
    class SpacingProfileDetector : public ContourWordSkewDetector
    {
        public:
        
            SpacingProfileDetector();
        
            ~SpacingProfileDetector();
        
        double probMeasure2;
        
            using ContourWordSkewDetector::detectContoursSkew;
        
            double detectSkew( std::vector<Blob>& blobs ,double lineK, double& probability, cv::Mat* debugImage =NULL);
        
            double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL, std::vector<cv::Rect>* bounds=NULL);
        
        private:
        
            size_t spaceCount;
        
            void findProfiles(std::vector<cv::Point>& leftFace,std::vector<cv::Point>& rightFace,std::vector<double>& angles, std::vector<double>& widths, VisData* visData=NULL, cv::Mat* debugImage = NULL);
        
            void drawSpaceProfiles(cv::Mat& img, std::vector<std::vector<cv::Point>> characters, std::vector<std::pair<std::vector<size_t>,std::vector<size_t>>> facePointIndices, VisData& visData);
        
            void getFace(const std::vector<cv::Point> &input, std::vector<cv::Point>& leftFace,std::vector<cv::Point>& rightFace, std::vector<size_t>& leftIndices, std::vector<size_t>& rightIndices);
        
            bool testBounds(cv::Point& edge, cv::Point& pivotVertex, std::vector<cv::Point>& opposingFace, bool convex);
        
            cv::Vec3d getLine(cv::Point edge, cv::Point point);
        
            double getWidth(cv::Vec3d line, cv::Point point);
        
            double createHistogram(double* histogram,std::vector<double> widths, std::vector<double> angles, cv::Mat* debugImg = NULL);
        
    };
}