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
#define _USE_MATH_DEFINES
#include <math.h>


namespace cmp
{

//the horizont threshold
#define IGNORE_ANGLE 20

#define ANGLE_TOLERANCE M_PI / 60.0

//common functions

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
	 * @param debugImage the debug image will be created in same size as mask image
	 * @return the skew angle in radians
	 */
	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL ) = 0;

	/** the measure of "how shure" the detector is about the result */
	double lastDetectionProbability;

	/** probality measure 1 */
	int probMeasure1;
	/** probality measure 2 */
	int probMeasure2;
};

/**
 * @class cmp::ContourSkewDetector
 *
 * @brief The skew detector interface for detectors working on contour
 *
 * Class interface carry out: contour detection and returning just biggest contour
 */
class ContourSkewDetector : public SkewDetector
{
public:

	ContourSkewDetector( int approximatioMethod, double epsilon );

	virtual ~ContourSkewDetector(){  }

	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );

	/**
	 * Descendants have to implement this method
	 *
	 * @param mask
	 * @param contours
	 * @param debugImage
	 * @return
	 */
	virtual double detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage = NULL ) = 0;

	static void getBigestContour( std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy );

	static void filterValuesBySimiliarAngle
		(const std::vector<double>& values, const std::vector<double>& angles, std::vector<double>& valuesOut, std::vector<double>& anglesOut,
		std::vector<bool>condition, double angleRange = ANGLE_TOLERANCE);

protected:

	//@see cv::findContorurs
	int approximatioMethod;
	/** the constant in approximation method @see cv::approxPolyDP */
	double epsilon;
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
