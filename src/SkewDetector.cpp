/*
 * SkewDetector.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: Michal Busta
 */

#include "SkewDetector.h"

namespace cmp
{

SkewDetector::SkewDetector()
{
	// TODO Auto-generated constructor stub

}

SkewDetector::~SkewDetector()
{
	// TODO Auto-generated destructor stub
}

} /* namespace cmp */

void cmp::filterContour(std::vector<cv::Point>& vector)
{
	bool remove = false;
	for(std::vector<cv::Point>::iterator it = vector.begin(); it < vector.end(); )
	{
		//TODO implement
		if(remove)
		{
			it = vector.erase(it);
		}else{
			it++;
		}

	}
}
