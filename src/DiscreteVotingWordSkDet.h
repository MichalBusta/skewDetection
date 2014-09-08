//
//  DiscreteVotingWordSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 14/04/14.
//
//

#ifndef __SkewDetection__DiscreteVotingWordSkDet__
#define __SkewDetection__DiscreteVotingWordSkDet__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "WordSkewDetector.h"
#include <math.h>

#include "BestGuessSKDetector.h"

namespace cmp {
    
    
    struct Blob{
        
        cv::Mat mask;
        cv::Rect bBox;
        
        Blob(cv::Mat mask, cv::Rect bBox = cv::Rect()){
            this->mask = mask;
            this->bBox = bBox;
            
        };
    };
    
    struct VisualisationData{
        
        std::vector<std::map<std::string, double> > confidenceData;
        std::vector<std::map<std::string, cv::Mat> > imageData;
        
        VisualisationData(std::vector<std::map<std::string, double> > confidenceData, std::vector<std::map<std::string, cv::Mat> > imageData){
            this->confidenceData = confidenceData;
            this->imageData = imageData;
        }
    };
    
    class DiscreteVotingWordSkDet
    {
    public:
        
        DiscreteVotingWordSkDet(std::vector< cv::Ptr<SkewDetector> > detectors, std::vector<std::string> detNames, std::vector<double> weights, std::map<std::string, cv::Scalar> detectorIDColors);
        virtual ~DiscreteVotingWordSkDet();
        
        double detectSkew(std::vector<Blob>& blobs, double lineK, cv::Mat* debugImage =NULL);
        
    private:
        
        std::vector<double> weights;
        std::map<std::string, cv::Scalar> detectorIDColors;
        std::vector<std::string> detNames;
        std::vector<cv::Ptr<SkewDetector> > detectors;
        double computeAngle(std::vector<double> angles, std::vector<double> probabilities, double& probability, VisualisationData visData, cv::Mat* debugImage = NULL);
    };

    class DiscreteVotingWordSkDet2 : public ContourWordSkewDetector
        {
        public:

    	DiscreteVotingWordSkDet2(cv::Ptr<BestGuessSKDetector> detector, double delta = 1);
            virtual ~DiscreteVotingWordSkDet2();

            virtual double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL);
        protected:

            virtual double computeAngle(std::vector<double>& angles, std::vector<double>& probabilities, std::vector<int>& detectorsIndex, double& probability, cv::Mat* debugImage);

            double delta;
        };
}

#endif /* defined(__SkewDetection__DiscreteVotingWordSkDet__) */
