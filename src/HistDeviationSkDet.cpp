//
//  HistDeviationSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 11/05/14.
//
//

#include "HistDeviationSkDet.h"

namespace cmp {
    HistDeviationSkDet::HistDeviationSkDet(){
        
    }
    HistDeviationSkDet::~HistDeviationSkDet(){
        
    }
    
    double HistDeviationSkDet::detectSkew(cv::Mat &mask , double lineK, cv::Mat* debugImage)
    {
        
    }
    
    double HistDeviationSkDet::getDeviance(std::vector<double> &input)
    {
        
    }
    
    std::vector<double> HistDeviationSkDet::getSkeleton(cv::Mat &src)
    {
        std::vector<double> temp;
        int sum=0;
        int noOfElems=0;
        for (int y=0;y< src.rows; y++) {
            for (int x=0; x<src.cols; x++) {
                if (src.at<uchar>(y, x) != 0) {
                    sum +=x;
                    noOfElems++;
                }
            }
            temp.push_back(sum/noOfElems);
        }
        return temp;
    }
    
    cv::Mat HistDeviationSkDet::transformMask(cv::Mat &src, double angle)
    {
        
    }
}