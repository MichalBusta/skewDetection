//
//  WordSkewDetector.h
//  SkewDetection
//
//  Created by David Helekal on 12/03/14.
//
//

#ifndef __SkewDetection__WordSkewDetector__
#define __SkewDetection__WordSkewDetector__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _USE_MATH_DEFINES
#include "SkewDetector.h"
#include <math.h>

namespace cmp{

struct Blob{

	cv::Mat mask;
	cv::Rect bBox;

	Blob(cv::Mat mask, cv::Rect bBox = cv::Rect()){
		this->mask = mask;
		this->bBox = bBox;

	};
};

class WordSkewDetector
{
public:

	WordSkewDetector();
	virtual ~WordSkewDetector();

	virtual double detectSkew( std::vector<Blob>& blobs, double lineK, double& probability, cv::Mat* debugImage =NULL) = 0;

};

/**
 * Interface definition for skew estimation from the contours skew
 */
 class ContourWordSkewDetector : public WordSkewDetector
 {
 public:

	ContourWordSkewDetector();
	virtual ~ContourWordSkewDetector();

	virtual double detectSkew( std::vector<Blob>& blobs, double lineK, double& probability, cv::Mat* debugImage =NULL);

	virtual double detectContoursSkew( std::vector<std::vector<cv::Point>* >& contours, double lineK, double& probability, cv::Mat* debugImage =NULL, std::vector<cv::Rect>* bounds=NULL) = 0;
 };
}
#endif /* defined(__SkewDetection__WordSkewDetector__) */
