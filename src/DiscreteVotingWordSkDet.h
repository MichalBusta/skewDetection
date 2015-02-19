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

struct VisualisationData{

	std::vector<std::map<std::string, double> > confidenceData;
	std::vector<std::map<std::string, cv::Mat> > imageData;

	VisualisationData(std::vector<std::map<std::string, double> > confidenceData, std::vector<std::map<std::string, cv::Mat> > imageData){
		this->confidenceData = confidenceData;
		this->imageData = imageData;
	}
};

class DiscreteVotingWordSkDet : public ContourWordSkewDetector
{
public:

	DiscreteVotingWordSkDet();

	virtual ~DiscreteVotingWordSkDet();

	virtual double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL, std::vector<cv::Rect>* bounds=NULL);

private:

	std::vector<double> weights;
	std::map<std::string, cv::Scalar> detectorIDColors;
	std::vector<std::string> detectorNames;
	std::vector<cv::Ptr<ContourSkewDetector> > detectors;
	std::vector<cv::Scalar> detectorColors;
};

}

#endif /* defined(__SkewDetection__DiscreteVotingWordSkDet__) */
