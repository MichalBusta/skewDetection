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

namespace cmp
{
DiscreteVotingWordSkDet::DiscreteVotingWordSkDet(cv::Ptr<SkewDetector> detector) : ContourWordSkewDetector(detector)
{

}
DiscreteVotingWordSkDet::~DiscreteVotingWordSkDet()
{

}

double DiscreteVotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability)
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
		groupProbs[idx] += probabilities[i];
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
	return angle;
    }
}