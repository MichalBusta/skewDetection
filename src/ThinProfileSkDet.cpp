/*
 * ThinProfileSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ThinProfileSkDet.h"
#include "SkewDetector.h"

#define histColWidth 1

using namespace std;
using namespace cv;

namespace cmp
{

ThinProfileSkDet::ThinProfileSkDet(int approximatioMethod, double epsilon, int ignoreAngle, double ratio) : ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), ratio(ratio)
{
	// TODO Auto-generated constructor stub
	hist = new double [int(180/histColWidth)];
}

ThinProfileSkDet::~ThinProfileSkDet()
{
	// TODO Auto-generated destructor stub
	delete [] hist;
}

double ThinProfileSkDet::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage )
{	
	memset (hist, 0, int(180/histColWidth) * sizeof(double));

	if (contours[0].size() < 3) return 0;
	//cmp::filterContour(contours[0]);
	vector<Point> hull;
	convexHull( contours[0], hull );

	int topMost = 0;
	int bottomMost = 0;
	for(int i=0;i<hull.size();i++)
	{
		if(hull[i].y < hull[topMost].y)
		{
			topMost = i;
		}
		else if(hull[i].y > hull[bottomMost].y)
		{
			bottomMost = i;
		}
	}
	/**
	 * Rotating calipers method by Godfsried Toussaint
	 * http://en.wikipedia.org/wiki/Rotating_calipers
	 */
	int p_a = topMost;
	int p_b = bottomMost;

	double rotated_angle = 0;
	double min_width = std::numeric_limits<double>::infinity();
	
	Point2d horizont_poz(1,0);
	Point2d horizont_neg(-1,0);
	double angle = 0;
	Point2d resVector, resPoint, resPoint2;

	double angle_2nd = 0;
	Point2d resVector_2nd, resPoint_2nd, resPoint2_2nd;
	double min_width_2nd = std::numeric_limits<double>::infinity();

	int maxI = (int) ceil(double(ignoreAngle)/double(histColWidth));

	//std::vector<ThinPrDetection> detections;

	while(rotated_angle < M_PI)
	{
		int p_a_1 = p_a + 1;
		int p_b_1 = p_b + 1;
		if (p_a_1 >= hull.size()) p_a_1 = 0;
		if (p_b_1 >= hull.size()) p_b_1 = 0;
		Point2d edge_a(hull[p_a_1].x - hull[p_a].x, hull[p_a_1].y - hull[p_a].y);
		Point2d edge_b(hull[p_b_1].x - hull[p_b].x, hull[p_b_1].y - hull[p_b].y);
		
		double angleACos = (edge_a.x*horizont_poz.x + edge_a.y*horizont_poz.y)/(sqrt(edge_a.x*edge_a.x+edge_a.y*edge_a.y)*sqrt(horizont_poz.x*horizont_poz.x+horizont_poz.y*horizont_poz.y));
		double angleBCos = (edge_b.x*horizont_neg.x + edge_b.y*horizont_neg.y)/(sqrt(edge_b.x*edge_b.x+edge_b.y*edge_b.y)*sqrt(horizont_neg.x*horizont_neg.x+horizont_neg.y*horizont_neg.y));

		angleACos = MAX(MIN(angleACos, 1.0), -1.0);
		angleBCos = MAX(MIN(angleBCos, 1.0), -1.0);

		double angle_a = acos(angleACos);
		double angle_b = acos(angleBCos);
		double width = 0;

		double x1 = horizont_poz.x;
		double y1 = horizont_poz.y;
		double x2 = horizont_neg.x;
		double y2 = horizont_neg.y;
		
		horizont_poz.x = x1*cos(min(angle_a, angle_b))-y1*sin(min(angle_a, angle_b));
		horizont_poz.y = x1*sin(min(angle_a, angle_b))+y1*cos(min(angle_a, angle_b));

		horizont_neg.x = x2*cos(min(angle_a, angle_b))-y2*sin(min(angle_a, angle_b));
		horizont_neg.y = x2*sin(min(angle_a, angle_b))+y2*cos(min(angle_a, angle_b));
		Point2d tmpVector, tmpPoint, tmpPoint2;
		double ang = 0;
		if(angle_a < angle_b)
		{
			p_a++;
			if(p_a >= hull.size())
				p_a = 0;
			width = abs(horizont_poz.y*hull[p_b].x-horizont_poz.x*hull[p_b].y-horizont_poz.y*hull[p_a].x+horizont_poz.x*hull[p_a].y)/sqrt(horizont_poz.x*horizont_poz.x+horizont_poz.y*horizont_poz.y);
			tmpVector = horizont_poz;
			tmpPoint = hull[p_a];
			tmpPoint2 = hull[p_b];
			ang = atan2(horizont_poz.y, horizont_poz.x);
		}
		else
		{
			p_b++;
			if(p_b >= hull.size())
				p_b = 0;
			width = abs(horizont_neg.y*hull[p_a].x-horizont_neg.x*hull[p_a].y-horizont_neg.y*hull[p_b].x+horizont_neg.x*hull[p_b].y)/sqrt(horizont_neg.x*horizont_neg.x+horizont_neg.y*horizont_neg.y);
			tmpVector = horizont_neg;
			tmpPoint = hull[p_b];
			tmpPoint2 = hull[p_a];
			ang = atan2(horizont_neg.y, horizont_neg.x);
		}

		rotated_angle = rotated_angle + min(angle_a, angle_b);

		ang = ang + M_PI/2;
		while (ang > M_PI/2) ang = ang - M_PI;
		while (ang <= -M_PI/2) ang = ang + M_PI;

		int pos = (int) (ang + M_PI/2)/M_PI*180/histColWidth;
		hist[pos] = hist[pos] + 1/width;
		if (hist[pos] > hist[maxI]) maxI = pos;

		if((ang >= (M_PI/180*ignoreAngle-M_PI/2) && ang <= (M_PI/2-M_PI/180*ignoreAngle)))
		{
			/*ThinPrDetection det;
			det.width = width;
			det.angle = ang;
			det.vector = tmpVector;
			det.point1 = tmpPoint;
			det.point2 = tmpPoint2;
			detections.push_back(det);*/

			if(width <= min_width)
			{
				angle_2nd = angle;
				min_width_2nd = min_width;
				resVector_2nd = resVector;
				resPoint_2nd = resPoint;
				resPoint2_2nd = resPoint2;

				angle = ang;
				min_width = width;
				resVector = tmpVector;
				resPoint = tmpPoint;
				resPoint2 = tmpPoint2;
			}
			else if(width <= min_width_2nd)
			{
				angle_2nd = ang;
				min_width_2nd = width;
				resVector_2nd = tmpVector;
				resPoint_2nd = tmpPoint;
				resPoint2_2nd = tmpPoint2;
			}
		}
	}
	
	int height = 200*hist[maxI]+20;
	cv::Mat histogram = Mat::zeros(height, 380, CV_8UC3);
	double totalLen = 0.0;
	double resLen = 0.0;
	int range = 10;

	for(int i=0;i<int(180/histColWidth);i++)
	{
		int rectH = 200*hist[i];
		cv::rectangle(histogram, Rect(10+histColWidth*i*2, height-rectH-10, histColWidth*2, rectH), Scalar(0,0,255), CV_FILLED);
		if (i > MIN(ignoreAngle/histColWidth,maxI-range) && i < MAX((180-ignoreAngle)/histColWidth, maxI+range))
		{
			if (hist[i] > hist[maxI]) maxI = i;
			totalLen = totalLen + hist[i];
		}
	}
	for (int i = maxI-range; i <= maxI+range; i++)
	{
		int j = i;
		if (j<0) j = j + int(180/histColWidth);
		if (j>=int(180/histColWidth)) j = j - int(180/histColWidth);

		resLen = resLen + hist[j];
	}

	//cv::imshow("Histogram", histogram);
	this->lastDetectionProbability = resLen/totalLen;
	//std::cout << resLen/totalLen << endl;

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );
		
		//cmp::filterContour(contours[0]);

		for(int i=0;i<hull.size();i++)
		{
			cv::circle(drawing, hull[i], 2, Scalar( 255, 0, 0 ), 2);
		}
		cv::line(drawing, resPoint-resVector*100, resPoint+resVector*100, Scalar( 0, 0, 255 ), 1);
		cv::line(drawing, resPoint2-resVector*100, resPoint2+resVector*100, Scalar( 0, 0, 255 ), 1);

		cv::circle(drawing, resPoint2, 3, Scalar( 0, 0, 255 ), 2);

		if ((min_width/min_width_2nd+ratio) > 1)
		{
			cv::Point2f middleVector, middlePoint;
			cv::line(drawing, resPoint_2nd-resVector_2nd*100, resPoint_2nd+resVector_2nd*100, Scalar( 0, 255, 255 ), 1);
			cv::line(drawing, resPoint2_2nd-resVector_2nd*100, resPoint2_2nd+resVector_2nd*100, Scalar( 0, 255, 255 ), 1);
		
			if(resVector.y*resVector_2nd.y < 0) resVector_2nd = resVector_2nd*(-1);

			middleVector.x = ((resVector.x*min_width)+(resVector_2nd.x*min_width_2nd))/(min_width+min_width_2nd);
			middleVector.y = ((resVector.y*min_width)+(resVector_2nd.y*min_width_2nd))/(min_width+min_width_2nd);

			middlePoint.x = (resPoint2_2nd.x+resPoint2.x)/2;
			middlePoint.y = (resPoint2_2nd.y+resPoint2.y)/2;

			cv::line(drawing, middlePoint-middleVector*100, middlePoint+middleVector*100, Scalar( 0, 255, 0 ), 1);
		
			cv::circle(drawing, resPoint2_2nd, 3, Scalar( 0, 255, 255 ), 2);
		}
	}

	if ((min_width/min_width_2nd+ratio) > 1)
	{
		return ((angle*min_width_2nd)+(angle_2nd*min_width))/(min_width+min_width_2nd);
	}
	//return ((angle*min_width_2nd)+(angle_2nd*min_width))/(min_width+min_width_2nd);

	return angle;
}

} /* namespace cmp */
