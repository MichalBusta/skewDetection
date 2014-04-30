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

double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, cv::Mat* debugImage)
{

	probability = 0;
	size_t noOfGroups=70;
	double groupRange;
	double min = -M_PI_2, max = M_PI_2;
	double maxProb = 0;
	double allProb = 0;
	int iterator;
	double sum=0;
	double angle;
    double sigma =0;
    double delta =1;
    int range=5;
    cv::Mat histogram;
    
    
	std::vector<std::vector<double> > groups;
	groups.resize(noOfGroups);
	std::vector<double> groupProbs;
	groupProbs.resize(noOfGroups);
	groupRange = (max-min) / (noOfGroups);


	for (int i=0; i<angles.size(); i++) {
		int idx =  (int) floor( (angles[i] - min ) / groupRange);
		idx = MIN(idx, noOfGroups - 1);
		assert( idx >= 0 );
		groups[idx].push_back(angles[i]);
        
        for(int i1 =0; i1 >range; i1++){
            if (i+i1<noOfGroups-1) {
                groupProbs[i+i1] += 1/(delta*sqrt(2*M_PI))*pow(M_E, -((i-sigma)*(i-sigma))/((2*delta)*(2*delta)));
            }
            if (i-i1>0 && i1!=0) {
                groupProbs[i-i1] += 1/(delta*sqrt(2*M_PI))*pow(M_E, -((-i-sigma)*(-i-sigma))/((2*delta)*(2*delta)));
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

	for (int i=0; i<groups[iterator].size(); i++) {
		sum += groups[iterator][i];
	}

	angle=sum/groups[iterator].size();
	probability = maxProb / allProb;
    
    //draw the histogram
    
    
    histogram = cv::Mat::zeros(maxProb, noOfGroups, CV_8UC3);
    for (int i =0; i <noOfGroups; i++) {

        cv::line(histogram, cv::Point(i, 0), cv::Point(i, groupProbs[i]), cv::Scalar(255,0,0));
    }
    
    cv::imshow("Histogram", histogram);
    
	return angle;
    }
}