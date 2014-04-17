//
//  HistogramWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#include "HistogramWordSkDet.h"

namespace cmp
{
    double HistogramWordSkDet::detectSkew(std::vector<Blob>& blobs, double line_k, cv::Mat* debugImage)
    {
        
        
    }
    
    cv::Mat HistogramWordSkDet::combineMasks(std::vector<Blob> &blobs){
        
        int cols=0;
        int rows=0;
        int maxHeight=0;
        int maxWidth =0;
        
        const int spacing = 10;
        const int buffer = 10;
        
        for (int i = 0; i<blobs.size(); i++) {
            if (blobs[i].mask.rows > maxHeight) maxHeight = blobs[i].mask.rows;
        }
        rows += maxHeight+buffer;
        cols += blobs.size()*(maxWidth+spacing);
        
        cv::Mat newMask = *new cv::Mat(rows, cols, CV_8UC1);
        
        for (int i = 0; i < blobs.size(); i++) {
            
            for (int x=0; x<blobs[i].mask.cols; x++) {
                
                for (int y=0 ; y<blobs[i].mask.rows; x++) {
                    newMask.at<uchar>(x+i*buffer, y) =blobs[i].mask.at<uchar>(x,y);
                
                }
                
            }
        }
    }
}