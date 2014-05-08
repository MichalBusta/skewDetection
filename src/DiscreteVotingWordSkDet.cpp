//
//  DiscreteVotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#include "DiscreteVotingWordSkDet.h"
#include "WordSkewDetector.h"
#include "stdlib.h"
#include <opencv2/highgui/highgui.hpp>

namespace cmp
{
DiscreteVotingWordSkDet::DiscreteVotingWordSkDet(cv::Ptr<SkewDetector> detector) : ContourWordSkewDetector(detector)
{

}
DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
{

}

double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, std::vector<cv::Mat*>& images, cv::Mat* debugImage)
{

	probability = 0;
	size_t noOfGroups=70;
	double groupRange;
	double min = -M_PI_2, max = M_PI_2;
	double maxProb = 0;
	double allProb = 0;
	int iterator;
	double angle;
    double sigma =0;
    double delta =1;
    int range=10;
    double resolution =0.4;
    int maxImgHeight=0;
    int headerHeight;
    int headerRows=1;
    std::vector< std::vector<cv::Mat> > rowImages;;
    cv::Mat histogram;
    
    
    std::vector<double> groupProbs;
 	groupProbs.resize(noOfGroups);
	groupRange = (max-min) / (noOfGroups);
 

	for (int i=0; i<angles.size(); i++) {
		int idx =  (int) floor( (angles[i] - min ) / groupRange);
		idx = MIN(idx, noOfGroups - 1);
		assert( idx >= 0 );
        
        for(int i1 =0; i1 <range; i1++){
            if (idx+i1<noOfGroups-1) {
                groupProbs[idx+i1] += probabilities[i]/(delta*sqrt(2*M_PI))*pow(M_E, -(i1*resolution-sigma)*(i1*resolution-sigma)/(2*delta*delta));
            }
            if (idx-i1>0 && i1!=0) {
                groupProbs[idx-i1] += probabilities[i]/(delta*sqrt(2*M_PI))*pow(M_E, -(i1*resolution-sigma)*(i1*resolution-sigma)/(2*delta*delta));
            }

        }
		allProb += probabilities[i];
	}

	for (int i =0; i <groupProbs.size(); i++) {
		if (maxProb < groupProbs[i]) {
			maxProb = groupProbs[i];
			iterator=i;
		}
	}

	angle= iterator*groupRange+min;
	probability = maxProb / allProb;
    
    //the drawing part
    
    //get max debugimg height
    
    
    for (size_t i = 0; i<images.size(); i++) {
        maxImgHeight = MAX(images[i]->rows, maxImgHeight);
    }
    
    
    int rowSize =0;
    int rowIdx =0;
    for (size_t i =0; i<images.size(); i++) {
        
        if (rowSize +=images[i]->cols > 500) {
            rowSize +=images[i]->cols;
            rowImages[rowIdx].push_back(*images[i]);
        }
        else{
            
            rowIdx++;
            rowImages.reserve(rowIdx);
            rowSize=0;
        }
        
    }
    
    headerHeight = maxImgHeight*(rowIdx+1);
    
    //draw the histogram
    
    int headerbuffer =20;
    int histWidth =500;
    int histHeight = 300;
    int totalHeaderHeight = headerbuffer+headerHeight;
    int colWidth = 5;
    
    histogram = cv::Mat::zeros(histHeight+totalHeaderHeight, histWidth, CV_8UC3);
    int graphWidth =noOfGroups*colWidth;
    int sidebarWidth = (histWidth-graphWidth)/2;
    
    //drawing tbe header buffer
    cv::rectangle(histogram, cvPoint(0, totalHeaderHeight), cvPoint(histWidth, headerHeight), cv::Scalar(213,213,213),CV_FILLED);
    //drawing the sidebars
    cv::rectangle(histogram, cv::Point(0,0+headerbuffer+headerHeight), cv::Point(sidebarWidth,histHeight+totalHeaderHeight), cv::Scalar(213,213,213), CV_FILLED);
    cv::rectangle(histogram, cv::Point(histWidth-sidebarWidth,0+headerbuffer+headerHeight), cv::Point(histWidth,histHeight+totalHeaderHeight), cv::Scalar(213,213,213), CV_FILLED);
    
    //drawing the graph
    for (int i =0; i <noOfGroups; i++) {

        cv::rectangle(histogram, cv::Point(i*colWidth+sidebarWidth, histHeight+totalHeaderHeight), cv::Point(colWidth*i+colWidth+sidebarWidth, histHeight+totalHeaderHeight-histHeight*groupProbs[i]), cv::Scalar(0,0,255),CV_FILLED);
    }
    //drawing the debug images from detectors
    
    for (size_t i =0; i<rowImages.size(); i++) {
        int rowWidth=0;
        for (size_t i1=0; i1<rowImages[i].size(); i1++) {
            
            cv::Rect roi =cv::Rect(rowWidth,maxImgHeight*i,rowImages[i][i1].cols,rowImages[i][i1].rows);
            cv::Mat roiImg;
            roiImg=histogram(roi);
            rowImages[i][i1].copyTo(roiImg);
            
            rowWidth += rowImages[i][i1].cols;
        }
    }
    
    //the debug image
    if(debugImage != NULL)
    {
        cv::Mat& draw = *debugImage;
        int x = tan(angle)*debugImage->rows;
        cv::line( draw, cv::Point(0,0), cv::Point(x, debugImage->cols),cv::Scalar(0,255,0));
        
        cv::imshow("DebugImage", draw);
    }
    
    cv::imshow("Histogram", histogram);
    cv::waitKey(0);

	return angle;
    
    }
}