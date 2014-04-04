//
//  VotingWordSkDet.cpp
//  SkewDetection
//
//  Created by David Helekal on 21/03/14.
//
//

#include "VotingWordSkDet.h"
#include "WordSkewDetector.h"
#include "stdlib.h"

namespace cmp
{
    VotingWordSkDet::VotingWordSkDet(cv::Ptr<SkewDetector> detector) :
    ContourWordSkewDetector( detector )
    {
        
    }
    VotingWordSkDet::~VotingWordSkDet()
    {
        
    }
    
    double VotingWordSkDet::computeAngle(std::vector<double> angles, std::vector<double> probabilities){
        
        
        //get highest certainty
        /*int maxProb =0;
        int iterator;
        for (int i=0; i<probabilities.size(); i++) {
            if(probabilities[i]>maxProb){
                maxProb = probabilities[i];
                iterator = i;
            }
        }
        return angles[iterator];
         **/
        int begin=0;
        int end=0;
        int max =0;
        int maximum =0;
        int iterator;
        int sum = 0;
        
        bool isGroup = false;
        
        std::vector<double> sortedAngles;
        std::vector<double> grad;
        std::vector<double> sortedProbs;
        std::vector<double> groupProbs;
        std::vector<std::vector<double> > groups;
        std::vector<bool> isAssigned;
        
        //sort the angles

        sortedAngles = angles;
        std::sort(sortedAngles.begin(), sortedAngles.end());
        
        //calcuulate gradients
        for(int i =0; i < sortedAngles.size()-1; i++)
        {
            grad.push_back(std::abs(sortedAngles[i]-sortedAngles[i+1]));
        }
        
        //match probs with sorted angles
        
        for (int i=0; i<angles.size(); i++) {
            isAssigned.push_back(false);
        }
        
        for (int i=0; i<sortedAngles.size(); i++) {
            int val = sortedAngles[i];
            
            for (int i2 = 0; i2< angles.size(); i2++) {
                if(angles[i2] == val && isAssigned[i2] == false){
                    sortedProbs.push_back(probabilities[i2]);
                    isAssigned[i2] = true;
                }
            }
            
        }
        
        //group similar values
        for (int i =0; i<grad.size(); i++) {
            if (max<grad[i] && !isGroup) {
                max = grad[i];
                begin = i;
                isGroup = true;
            }
            else if (max>grad[i] && isGroup) {
                continue;
            }
            else if (max<grad[i] && isGroup) {
                max = grad[i];
                end = i-1;
                
                std::vector<double> tempVector;
                int totalProb=0;
                
                for(int i = begin; i < end; i++){
                    tempVector.push_back(sortedAngles[i]);
                    totalProb += sortedProbs[i];
                    
                }
                
                groupProbs.push_back(totalProb);
                groups.push_back(tempVector);
                tempVector.clear();
                
                begin =i;
                
            }
        }
        //return the avg of the group with the highest total certainty
        
        for (int i = 0; i<groupProbs.size(); i++) {
            if(groupProbs[i]> maximum){
                maximum = groupProbs[i];
                iterator =i;
                
            }
        }
        for (double d : groups[iterator]) {
            sum += d;
        }
        return sum/groups[iterator].size();
    }
}