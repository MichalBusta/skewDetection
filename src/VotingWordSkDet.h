//
//  VotingWordSkDet.h
//  SkewDetection
//
//  Created by David Helekal on 21/03/14.
//
//

#ifndef __SkewDetection__VotingWordSkDet__
#define __SkewDetection__VotingWordSkDet__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "WordSkewDetector.h"
#include <math.h>

namespace cmp {
    
/**
 * @class cmp::BestGuessSKDetector
 *
 * @brief TODO brief description
 *
 * TODO type description
 */
class VotingWordSkDet : public ContourSkewDetector
{
public:
	VotingWordSkDet(int approximatioMethod = CV_CHAIN_APPROX_TC89_KCOS, double epsilon = 0.05);
	virtual ~VotingWordSkDet();


	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );

	virtual double detectSkew( std::vector<cv::Point>& outerContour, cv::Mat* debugImage = NULL );

	void getSkewAngles( std::vector<cv::Point>& outerContour, std::vector<double>& angles, std::vector<double>& probabilities, cv::Mat* debugImage  = NULL);

	std::vector<double> weights;
private:
	std::vector<cv::Ptr<ContourSkewDetector> > detectors;
};
}
#endif /* defined(__SkewDetection__VotingWordSkDet__) */
