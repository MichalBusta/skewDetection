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
#define IGNORE_ANGLE 30

#define ANGLE_TOLERANCE M_PI/60.0

//common functions
void draw_polar_histogram(cv::Mat& img, double* histogram, cv::Scalar color);

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


	/** the measure of "how sure" the detector is about the result */
	double lastDetectionProbability;

	/** probality measure 1 */
	int probMeasure1;
	/** probality measure 2 */
	double probMeasure2;
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

	virtual void getSkewAngles( std::vector<cv::Point>& outerContour, double lineK, std::vector<double>& angles, std::vector<double>& probabilities, std::vector<int>& detecotrsId, cv::Mat* debugImage){

	}

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate = false, cv::Mat* debugImage = NULL) = 0;
	/**
	 * Descendants have to implement this method
	 *
	 * @param mask
	 * @param contours
	 * @param debugImage
	 * @return
	 */
	virtual double detectSkew( std::vector<cv::Point>& contour, double lineK,  bool approximate = false, cv::Mat* debugImage = NULL ) = 0;

	static void getBigestContour( std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy );

	// function to filter things that are closer than ANGLE_TOLERANCE to other thing (e.g. thin profiles)
	static void filterValuesBySimiliarAngle
	(const std::vector<double>& values, const std::vector<double>& angles, std::vector<double>& valuesOut, std::vector<double>& anglesOut, double angleRange = ANGLE_TOLERANCE * 2);

protected:

	void approximateContour(std::vector<cv::Point>& contour, std::vector<cv::Point>& contourOut);

	//@see cv::findContorurs
	int approximatioMethod;
	/** the constant in approximation method @see cv::approxPolyDP */
	double epsilon;
	const double scalefactor;
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

/**
 * Create image which is horizontal alignment of imagesToMerge
 *
 * @param imagesToMerge
 * @param spacing space between images
 * @param verticalDisplacement
 *
 * @return the merged image
 */
inline cv::Mat mergeHorizontal(std::vector<cv::Mat>& imagesToMerge, int spacing, int verticalDisplacement, std::vector<cv::Point>* imagesCenters, cv::Scalar color = cv::Scalar(0, 0, 0) )
{
	int sw = 0;
	int sh = 0;
	for( std::vector<cv::Mat>::iterator it =  imagesToMerge.begin(); it < imagesToMerge.end(); it++ )
	{
		sw += it->cols + spacing;
		sh = MAX(it->rows + verticalDisplacement, sh);
	}



	cv::Mat mergedImage = cv::Mat::zeros(sh, sw, imagesToMerge[0].type()) + color;
	int wOffset = 0;

	int i = 0;
	for( std::vector<cv::Mat>::iterator it =  imagesToMerge.begin(); it < imagesToMerge.end(); it++ )
	{
		int hoffset = (i % 2 ) * verticalDisplacement;
		if( it->rows < mergedImage.rows)
		{
			hoffset += (mergedImage.rows - it->rows) / 2;
		}
		if(it->cols == 0)
			continue;
		cv::Rect roi = cv::Rect(wOffset, hoffset, it->cols, it->rows);
		mergedImage(roi) = cv::Scalar(0, 0, 0);
		mergedImage(roi) += *it;
		wOffset += it->cols + spacing;
		if(imagesCenters != NULL)
			imagesCenters->push_back( cv::Point(roi.x + roi.width / 2, roi.y + roi.height / 2) );
		i++;
	}

	return mergedImage;
}

} /* namespace cmp */
#endif /* SKEWDETECTOR_H_ */
