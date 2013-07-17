/*
 * SkewDetector.h
 *
 *  Created on: Jul 9, 2013
 *      Author: Michal Busta
 */

#ifndef SKEWDETECTOR_H_
#define SKEWDETECTOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace cmp
{

//common functions

void filterContour(std::vector<cv::Point>& vector);

/**
 * @class cmp::SkewDetector
 * 
 * @brief The skew detector interface
 *
 * TODO type description
 */
class SkewDetector
{
public:
	SkewDetector();
	virtual ~SkewDetector();

	/**
	 *
	 * @param mask the character mask - ink is white
	 * @param lineK
	 * @param debugImage
	 * @return the skew angle in radians
	 */
	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL ) = 0;

	static void getBigestContour( std::vector<std::vector<cv::Point> >& contours );
};

class ContourSkewDetector : public SkewDetector
{
public:
	virtual double detectSkew( std::vector<std::vector<cv::Point> >& contours, double lineK, const cv::Mat& mask, cv::Mat* debugImage = NULL );
};

/**
 * @class cmp::MockSkewDetector
 *
 * @brief The test detector, detected skew is allways 0
 */
class MockSkewDetector : public SkewDetector
{
	inline virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL )
	{
		if(debugImage != NULL)
		{
			cv::Mat& drawing =  *debugImage;
			drawing =  cv::Mat::zeros( mask.size(), CV_8UC3 );

			std::vector<std::vector<cv::Point> > contours;
			std::vector<cv::Vec4i> hierarchy;

			cv::findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

			cv::Scalar color = cv::Scalar( 255, 255, 255 );
			drawContours( drawing, contours, 0, color, 2, 8, hierarchy, 0, cv::Point() );
		}
		return 0.0;
	}
};

} /* namespace cmp */
#endif /* SKEWDETECTOR_H_ */
