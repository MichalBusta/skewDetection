/*
 * ImageFilter.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: Michal Busta
 */
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ImageFilter.h"

using namespace std;
using namespace cv;
namespace cmp
{

ImageFilter::ImageFilter()
{
	// TODO Auto-generated constructor stub

}

ImageFilter::~ImageFilter()
{
	// TODO Auto-generated destructor stub
}

cv::Mat ImageFilter::filterImage(cv::Mat source)
{
	vector<vector<Point> > contours;
	vector<vector<Point> > contours2;
	vector<Vec4i> hierarchy;
	findContours( source, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	int maxArea=0;
	double y=0;
	int numberOfContour=0;
	if(contours.size()>1)
	{
		for(int c = 0; c < contours.size(); c++)
		{
			Rect rect=boundingRect(contours[c]);
			y=rect.height*rect.width;
			if(y > maxArea)	//contour[numberOfContour] je nejvetsi contoura(na plochu)
			{
				maxArea = y;
				numberOfContour=c;
			}
			//contour[numberOfContour] je nejvetsi contoura(na plochu)
			int i = 0;
			contours2.push_back( contours[numberOfContour] );

			for(vector<vector<Point> >::iterator it = contours.begin(); it < contours.end();)
			{
				if(i == numberOfContour)
				{
					it++;
				}else{
					it = contours.erase(it);
				}
				i++;
			}

		}

	}


}

} /* namespace cmp */
