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
        int maxProb =0;
        int iterator;
        for (int i=0; i<probabilities.size(); i++) {
            if(probabilities[i]>maxProb){
                maxProb = probabilities[i];
                iterator = i;
            }
        }
        return angles[iterator];
    

    }
}