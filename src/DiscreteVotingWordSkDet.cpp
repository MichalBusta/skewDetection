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

        size_t noOfGroups=10;
        double groupRange;
        double min = DBL_MAX, max = -DBL_MAX;
        double maxProb = 0;
        int iterator;
        double sum=0;
        double maxAngle =90;
        double minAngle =45;
        double angle;
        
        std::vector<std::vector<double> > groups;
        groups.resize(noOfGroups*2);
        std::vector<double> groupProbs;
        groupProbs.resize(noOfGroups*2);
        
       /* for (int i =0; i <angles.size(); i++) {
        	max = MAX(max, angles[i]);
        	min = MIN(min, angles[i]);
        }*/
        groupRange = (maxAngle-minAngle) / (noOfGroups);
        
        
        for (int i=0; i<angles.size(); i++) {
            int index =  (int) floor( (angles[i] - min ) / groupRange);
            index += noOfGroups;
            groups[index].push_back(angles[i]);
            groupProbs[index] += probabilities[i];
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
        
      /*  size_t noOfGroups = 10;
        std::vector<double> tempVect;
        std::vector<std::vector<double> > groups;
        groups.resize(noOfGroups);
        std::vector<double> sortedProbs;
        std::vector<double> groupProbs;
        groupProbs.resize(noOfGroups);
        std::vector<bool> isAssigned;
 
        int max, min;
        int range;
        int groupRange;
        int maxProb =0;
        int highestProbIterator;
        int angle;
        int sum=0;
        
        //calculate range
        
        tempVect = angles;
        std::sort(tempVect.begin(), tempVect.end());
        max = tempVect[tempVect.size()];
        min = tempVect[0];
        range = max-min;
        groupRange = range/noOfGroups;
        
        //link probabilities with groups
        
        for (int i=0; i<angles.size(); i++) {
            isAssigned.push_back(false);
        }
        
        for (int i=0; i<tempVect.size(); i++) {
            int val = tempVect[i];
            
            int i2=0;
            bool control =true;
            while (control){
                if(angles[i2] == val && isAssigned[i2] == false) {
                    sortedProbs.push_back(probabilities[i2]);
                    isAssigned[i2] = true;
                    control = false;
                }
                i2++;
            }
        }
        
        //assign values into groups
        
        int multiplier = 1;
        for (int i = 0; i < tempVect.size(); i++) {
            
            if (tempVect[i]<=(multiplier*groupRange)) {
                groups[multiplier].push_back(tempVect[i]);
                groupProbs[multiplier] += sortedProbs[i];
            }
            else{
                multiplier++;
                groups[multiplier].push_back(tempVect[i]);
                groupProbs[multiplier] += sortedProbs[i];
            }
        }
        //get greatest groupProb
        
        for (int i =0; i< groupProbs.size(); i++) {
            if (groupProbs[i] > maxProb){
                highestProbIterator =i;
                maxProb = groupProbs[i];
            }
        }
        
       // calculate the group average
        
        for (int i=0; i<groups[highestProbIterator].size(); i++) {
            sum += groups[highestProbIterator][i];
        }
        angle = sum/groups[highestProbIterator].size();
       */
       return angle;
    }
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

}//namespace cmp
