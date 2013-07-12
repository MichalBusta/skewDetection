/*
 * ThinProfileSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ThinProfileSkDet.h"

using namespace std;
using namespace cv;

namespace cmp
{

ThinProfileSkDet::ThinProfileSkDet()
{
	// TODO Auto-generated constructor stub

}

ThinProfileSkDet::~ThinProfileSkDet()
{
	// TODO Auto-generated destructor stub
}

double ThinProfileSkDet::detectSkew(cv::Mat& mask, double lineK,
		cv::Mat* debugImage)
{
	//TODO implement
	std::vector<std::vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );

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
		//printf("%i - %i", hull[i].x, hull[i].y);
	}
	/**
	 * Rotating calipers method by Godfsried Toussaint
	 * http://en.wikipedia.org/wiki/Rotating_calipers
	 */
	int p_a = topMost;
	int p_b = bottomMost;

	float rotated_angle = 0;
	float min_width = std::numeric_limits<float>::infinity();
	
	Point2f horizont_poz(1,0);
	Point2f horizont_neg(-1,0);
	double angle = 0;
	Point2f resVector, resPoint, resPoint2;
	while(rotated_angle < M_PI)
	{
		int p_a_1 = p_a + 1;
		int p_b_1 = p_b + 1;
		if (p_a_1 >= hull.size()) p_a_1 = 0;
		if (p_b_1 >= hull.size()) p_b_1 = 0;
		Point2f edge_a(hull[p_a_1].x - hull[p_a].x, hull[p_a_1].y - hull[p_a].y);
		Point2f edge_b(hull[p_b_1].x - hull[p_b].x, hull[p_b_1].y - hull[p_b].y);
		
		float angle_a = acos((edge_a.x*horizont_poz.x + edge_a.y*horizont_poz.y)/(sqrt(edge_a.x*edge_a.x+edge_a.y*edge_a.y)*sqrt(horizont_poz.x*horizont_poz.x+horizont_poz.y*horizont_poz.y)));
		float angle_b = acos((edge_b.x*horizont_neg.x + edge_b.y*horizont_neg.y)/(sqrt(edge_b.x*edge_b.x+edge_b.y*edge_b.y)*sqrt(horizont_neg.x*horizont_neg.x+horizont_neg.y*horizont_neg.y)));
		float width = 0;

		float x1 = horizont_poz.x;
		float y1 = horizont_poz.y;
		float x2 = horizont_neg.x;
		float y2 = horizont_neg.y;
		
		horizont_poz.x = x1*cos(min(angle_a, angle_b))-y1*sin(min(angle_a, angle_b));
		horizont_poz.y = x1*sin(min(angle_a, angle_b))+y1*cos(min(angle_a, angle_b));

		horizont_neg.x = x2*cos(min(angle_a, angle_b))-y2*sin(min(angle_a, angle_b));
		horizont_neg.y = x2*sin(min(angle_a, angle_b))+y2*cos(min(angle_a, angle_b));
		float b,a;
		Point2f tmpVector, tmpPoint, tmpPoint2;
		if(angle_a < angle_b)
		{
			p_a++;
			if(p_a >= hull.size())
				p_a = 0;
			a = horizont_poz.x*hull[p_b].x+horizont_poz.y*hull[p_b].y-horizont_poz.x*hull[p_a].x-horizont_poz.y*hull[p_a].y;
			b = sqrt(horizont_poz.x*horizont_poz.x+horizont_poz.y*horizont_poz.y);
			width = abs(horizont_poz.y*hull[p_b].x-horizont_poz.x*hull[p_b].y-horizont_poz.y*hull[p_a].x+horizont_poz.x*hull[p_a].y)/sqrt(horizont_poz.x*horizont_poz.x+horizont_poz.y*horizont_poz.y);
			//width = caliper_a.distance(points[p_b]);
			tmpVector = horizont_poz;
			tmpPoint = hull[p_a];
			tmpPoint2 = hull[p_b];
		}
		else
		{
			p_b++;
			if(p_b >= hull.size())
				p_b = 0;
			a = horizont_neg.x*hull[p_a].x+horizont_neg.y*hull[p_a].y-horizont_neg.x*hull[p_b].x-horizont_neg.y*hull[p_b].y;
			b = sqrt(horizont_neg.x*horizont_neg.x+horizont_neg.y*horizont_neg.y);
			width = abs(horizont_neg.y*hull[p_a].x-horizont_neg.x*hull[p_a].y-horizont_neg.y*hull[p_b].x+horizont_neg.x*hull[p_b].y)/sqrt(horizont_neg.x*horizont_neg.x+horizont_neg.y*horizont_neg.y);
			//width = caliper_b.distance(points[p_a]);
			tmpVector = horizont_neg;
			tmpPoint = hull[p_b];
			tmpPoint2 = hull[p_a];
		}

		rotated_angle = rotated_angle + min(angle_a, angle_b);

		double ang = 0;
		if(width <= min_width)
		{
			if(angle_a < angle_b) ang = atan2(horizont_poz.y, horizont_poz.x);
			else ang = atan2(horizont_neg.y, horizont_neg.x);

			/*
			if(
				(ang >= M_PI/3 && ang <= M_PI-M_PI/3) ||
				(ang >= M_PI+M_PI/3 && ang <= 2*M_PI-M_PI/3)
			)
			{*/
				ang = ang + M_PI/2;
				while (ang > M_PI/2) ang = ang - M_PI;
				while (ang < -M_PI/2) ang = ang + M_PI;

				angle = ang;
				min_width = width;
				resVector = tmpVector;
				resPoint = tmpPoint;
				resPoint2 = tmpPoint2;
			/*}
			*/
		}
		/*printf("%6.3f   %6.3f", width, ang);
		printf("\n");/**/
	}

	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );
		
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );
		cv::line(drawing, resPoint-resVector*100, resPoint+resVector*100, Scalar( 0, 0, 255 ), 1);
		cv::line(drawing, resPoint2-resVector*100, resPoint2+resVector*100, Scalar( 0, 255, 255 ), 1);
		cv::circle(drawing, resPoint2, 3, Scalar( 0, 255, 0 ), 2);
	}

	return angle;
}

} /* namespace cmp */
