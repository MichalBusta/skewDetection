//
//  DiscreteVotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#include "DiscreteVotingWordSkDet.h"
#include "WordSkewDetector.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


namespace cmp
{

DiscreteVotingWordSkDet::DiscreteVotingWordSkDet(cv::Ptr<SkewDetector> detector) : ContourWordSkewDetector(detector)
{

}
DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
{

}

double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, std::vector<cv::Mat> debugImages, cv::Mat* debugImage)
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
    
    for (size_t i = 0; i<debugImages.size(); i++) {
        maxImgHeight = MAX(debugImages[i].rows, maxImgHeight);
    }
    
    //calculate number of images per row
    int rowSize =0;
    int rowIdx =0;
    rowImages.resize(1);
    for (size_t i =0; i<debugImages.size(); i++) {
        
        if (rowSize +=debugImages[i].cols < 500) {
            rowSize +=debugImages[i].cols;
            rowImages[rowIdx].push_back(debugImages[i]);
        }
        else{
            
            rowIdx++;
            rowImages.resize(rowIdx+1);
            rowSize=0;
        }
        
    }
    
    headerHeight = maxImgHeight*(rowIdx+1);
    
	//draw the histogram
	if(debugImage != NULL)
	{
		int histWidth =500;
		int histHeight = 300;
		int colWidth = 5;

		histogram = cv::Mat::zeros(histHeight, histWidth, CV_8UC3);
		int graphWidth =noOfGroups*colWidth;
		int sidebarWidth = (histWidth-graphWidth)/2;

		cv::rectangle(histogram, cv::Point(0,0), cv::Point(sidebarWidth,histHeight), cv::Scalar(213,213,213), CV_FILLED);
		cv::rectangle(histogram, cv::Point(histWidth-sidebarWidth,0), cv::Point(histWidth,histHeight), cv::Scalar(213,213,213), CV_FILLED);

		for (int i =0; i <noOfGroups; i++) {

			cv::rectangle(histogram, cv::Point(i*colWidth+sidebarWidth, histHeight), cv::Point(colWidth*i+colWidth+sidebarWidth, histHeight-histHeight*groupProbs[i]), cv::Scalar(0,0,255),CV_FILLED);
		}

		cv::Mat& draw = *debugImage;
		int x = tan(angle)*debugImage->rows;
		cv::line( draw, cv::Point(0,0), cv::Point(x, debugImage->cols),cv::Scalar(0,255,0));

		cv::imshow("DebugImage", draw);
		cv::imshow("Histogram", histogram);
		cv::waitKey(0);
	}
    
	return angle;
}

}//namespace cmp
