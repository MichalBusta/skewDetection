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

ThinProfileSkDet::ThinProfileSkDet(int approximatioMethod, double epsilon, int ignoreAngle, double profilesRange, bool returnMiddleAngle) :
				ContourSkewDetector(approximatioMethod, epsilon), ignoreAngle(ignoreAngle), profilesRange(profilesRange),
				returnMiddleAngle(returnMiddleAngle)
{
	probabilities.push_back(0.48);
	probabilities.push_back(0.79);
	probabilities.push_back(0.74);
	probabilities.push_back(0.53);
	probabilities.push_back(0.50);
	probabilities.push_back(0.48);
}

ThinProfileSkDet::~ThinProfileSkDet()
{

}

double ThinProfileSkDet::detectSkew( const cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Vec4i>& hierarchy, cv::Mat* debugImage )
{

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

	vector<double>widths;
	vector<double>angles;
	vector<Point2d>PointsForWiderProfiles;
	vector<Point2d>PointsForWiderProfiles2;
	vector<Point2d>VectorsForWiderProfiles;
	vector<double>thinProfiles;

	// using method of rotating calipers to find thin profiles
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


		if((ang >= (M_PI/180*ignoreAngle-M_PI/2) && ang <= (M_PI/2-M_PI/180*ignoreAngle)))
		{
			// noting all information about profiles from our angle range
			widths.push_back(width);
			angles.push_back(ang);
			PointsForWiderProfiles.push_back(tmpPoint);
			PointsForWiderProfiles2.push_back(tmpPoint2);
			VectorsForWiderProfiles.push_back(tmpVector);

			if(width <= min_width)
			{
				angle = ang;
				min_width = width;
				resVector = tmpVector;
				resPoint = tmpPoint;
				resPoint2 = tmpPoint2;
			}
		}
	}

	// max width of profiles
	double thinProfilesRange = min_width * ( profilesRange + 1 );
	
	probMeasure1 = 0;
	probMeasure2 = 0;

	double greatestAngle = -M_PI;
	double smallestAngle = M_PI;

	// filtering thin profiles that are closer than ANGLE_TOLERANCE to other profile
	vector<double>widths2;
	vector<double>angles2;

	filterValuesBySimiliarAngle( widths, angles, widths2, angles2 );
	
	// counting profiles filtered by filterValuesBySimiliarAngle
	for(int c=0;c<widths.size();c++)
		if( ( widths2[c] != 0 ) && ( widths2[c] <= thinProfilesRange ) )
			probMeasure2++;


	// counting all profiles in thinProfilesRange
	for(int c=0;c<widths.size();c++)
		if( (widths[c] <= thinProfilesRange ))
		{
			probMeasure1++;
			greatestAngle = MAX(greatestAngle, angles[c]);
			smallestAngle = MIN(smallestAngle, angles[c]);
		}

	

#ifdef VERBOSE
	std::cout << "goodThinProfiles is: " << probMeasure2 << "\n";
	std::cout << "\n";
	std::cout << "noOfThinProfilesInRange is: " << probMeasure1 << "\n";
	std::cout << "greatestAngle is: " << greatestAngle << "\n";
	std::cout << "smallestAngle is: " << smallestAngle << "\n";
	std::cout << "middleAngle is: " << middleAngle << "\n";

#endif




	if(debugImage != NULL)
	{
		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( mask.size(), CV_8UC3 );

		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, 0, color, 1, 8, hierarchy, 0, Point() );

		//cmp::filterContour(contours[0]);

		// drawing the thiniest profile
		for(int i=0;i<hull.size();i++)
		{
			cv::circle(drawing, hull[i], 2, Scalar( 255, 0, 255 ), 2);
		}
		cv::line(drawing, resPoint-resVector*100, resPoint+resVector*100, Scalar( 0, 0, 255 ), 2);
		cv::line(drawing, resPoint2-resVector*100, resPoint2+resVector*100, Scalar( 0, 0, 255 ), 2);

		cv::circle(drawing, resPoint2, 3, Scalar( 0, 255, 0 ), 2);

		// drawing other filtered profiles
		for(int i=0;i<widths.size();i++)
		{
			if( ( widths2[i] != 0 ) && ( widths2[i] <= thinProfilesRange ) )
			{
				if(resVector.y*VectorsForWiderProfiles[i].y < 0) VectorsForWiderProfiles[i] = VectorsForWiderProfiles[i]*(-1);


				cv::Point2f middleVector, middlePoint;
				cv::line(drawing, PointsForWiderProfiles[i]-VectorsForWiderProfiles[i]*100, PointsForWiderProfiles[i]+VectorsForWiderProfiles[i]*100, Scalar( 0, 255, 255 ), 1);
				cv::line(drawing, PointsForWiderProfiles2[i]-VectorsForWiderProfiles[i]*100, PointsForWiderProfiles2[i]+VectorsForWiderProfiles[i]*100, Scalar( 0, 255, 255 ), 1);
				cv::circle(drawing, PointsForWiderProfiles2[i], 3, Scalar( 0, 0, 255 ), 1);
			}
		}
	}

	int index = (probMeasure1 - 1);
	index = MAX(index, this->probabilities.size() - 1);
	lastDetectionProbability = probabilities[index];


	if( returnMiddleAngle == true) angle = ( greatestAngle + smallestAngle ) / 2.0;

	return angle;
}

} /* namespace cmp */
