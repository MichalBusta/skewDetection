/*
 * ImageFilter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Michal Busta
 */

#ifndef IMAGEFILTER_H_
#define IMAGEFILTER_H_

#include <opencv2/core/core.hpp>

namespace cmp
{

/**
 * @class cmp::ImageFilter
 * 
 * @brief Filters images such output image contains only one external CC
 *
 */
class ImageFilter
{
public:
	ImageFilter();
	virtual ~ImageFilter();

	cv::Mat filterImage(cv::Mat source);
};

} /* namespace cmp */
#endif /* IMAGEFILTER_H_ */
