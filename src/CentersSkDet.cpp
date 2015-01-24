/*
 * CentersSkDet.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */
#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "CentersSkDet.h"

using namespace std;
using namespace cv;


namespace cmp
{

CentersSkDet::CentersSkDet(int approximatioMethod, double epsilon, float precision, bool recursive, double zoneOffset, bool useMaxMin ) : ContourSkewDetector(approximatioMethod, epsilon),  precision(precision), recursive(recursive), useMaxMin(useMaxMin) ,zoneOffset(zoneOffset)
{

}

CentersSkDet::~CentersSkDet()
{
	// TODO Auto-generated destructor stub
}
    
/*
 
 A complementary method for skew estimation. Improves performance on cyrillic alphabet.
*/
    
double CentersSkDet::doEstimate2(std::vector<cv::Point> &contour, cv::Mat* debugImage){
    
    //get yMax
    
    int ymax, ymin, xmax,xmin;
    
    xmax =0;
    xmin =INT_MAX;
    ymax =0;
    ymin =INT_MAX;
    
    for (int i=0; i<contour.size(); i++) {
        
        cv::Point pt = contour[i];
        
        xmax =MAX(xmax, pt.x);
        xmin =MIN(xmin, pt.x);
        ymax =MAX(ymax, pt.y);
        ymin =MIN(ymin, pt.y);
        
        
    }
    
    cv::Point TL = cv::Point(xmin,ymax);
    cv::Point TR = cv::Point(xmax,ymax);
    cv::Point BL = cv::Point(xmin,ymin);
    cv::Point BR = cv::Point(xmax,ymin);
    
    cv::Point TLC, TRC, BLC, BRC;
    TLC = BR;
    TRC = BL;
    BRC = TL;
    BLC = TR;
    
    for (int i =0; i<contour.size(); i++) {
        
        Point pt = contour[i];
        
        if (norm(TLC-TL) > norm(pt-TL)) {
            TLC = pt;
        }
        if (norm(TRC-TR) > norm(pt-TR)) {
            TRC = pt;
        }
        if (norm(BRC-BR) > norm(pt-BR)) {
            BRC = pt;
        }
        if (norm(BLC-BL) > norm(pt-BL)) {
            BLC = pt;
        }
        
    }
    
    cv::Point CB, CT;
    
    CT = cv::Point((TRC.x+TLC.x)/2, TRC.y);
    CB = cv::Point((BRC.x+BLC.x)/2, BRC.y);
    
    float angle=0, deltaX=0, deltaY=0;
    deltaX = CT.x - CB.x;
    deltaY = CB.y - CT.y;
    if(deltaY != 0)
        angle = atan((deltaX)*1.0/(deltaY));
    
    return angle;
    
}
    

static inline double cross_product( cv::Point& a, cv::Point& b ){
   return a.x*b.y - a.y*b.x;
}

double distance_to_line( cv::Point& begin, cv::Point end_m_begin, cv::Point x ){
   //translate the begin to the origin
   //end -= begin;
   x -= begin;

   //¿do you see the triangle?
   double area = cross_product(x, end_m_begin);
   return area / norm(end_m_begin);
}

static void findTopBottomPoints(cv::Point center, std::vector<cv::Point>& outerContour, double lineK, cv::Point& top, cv::Point& bottom, double& distance)
{
	double c = center.y - lineK*center.x;
	cv::Point center2(center.x + 100, (center.x + 100) * lineK + c);
	cv::Point end_m_begin = center2 - center;
	double maxDist = 0;
	double minDist = 0;
	for (int c = 0; c < outerContour.size();c++)
	{
		double dist =  distance_to_line(center, end_m_begin, outerContour[c]);
		if( maxDist < dist)
		{
			bottom = outerContour[c];
			maxDist = dist;
		}
		if(minDist > dist)
		{
			top = outerContour[c];
			minDist = dist;
		}
	}
	cv::Point top2(top.x + 100, (top.x + 100) * lineK + c);
	end_m_begin = top2 - top;
	distance = distance_to_line( top, end_m_begin, bottom );
}

struct SymPoints{
	cv::Point leftTop;
	cv::Point rightTop;
	cv::Point leftBottom;
	cv::Point rightBottom;
};

static void findPointsInBand(std::vector<cv::Point>& outerContour, cv::Point& center, double lineK, double letterSize, double delta, std::vector<double>& offsets,  std::vector<SymPoints>& symPoints )
{
	symPoints.clear();
	symPoints.resize(offsets.size());
	for(size_t i = 0; i < offsets.size(); i++)
	{
		symPoints[i].leftTop.x = INT_MAX;
		symPoints[i].leftBottom.x = INT_MAX;
	}

	double c = center.y - lineK*center.x;
	cv::Point center2(center.x + 100, (center.x + 100) * lineK + c);
	cv::Point end_m_begin = center2 - center;
	for(size_t i = 0; i < offsets.size(); i++)
	{
		double distCheck = letterSize * offsets[i] / 2;
		double distCheckDelta = distCheck - delta;
		for(size_t p = 0; p < outerContour.size(); p++)
		{
			double dist = distance_to_line(center, end_m_begin, outerContour[p]);
			if( dist <= distCheck && dist > distCheckDelta )
			{
				if(symPoints[i].leftTop.x > outerContour[p].x)
					symPoints[i].leftTop = outerContour[p];
				if(symPoints[i].rightTop.x < outerContour[p].x)
					symPoints[i].rightTop = outerContour[p];
			}else if(-dist <= distCheck && -dist > distCheckDelta)
			{
				if(symPoints[i].leftBottom.x > outerContour[p].x)
					symPoints[i].leftBottom = outerContour[p];
				if(symPoints[i].rightBottom.x < outerContour[p].x)
					symPoints[i].rightBottom = outerContour[p];
			}
		}
	}
}

cv::Point mirror(cv::Point& p, cv::Point& p0, cv::Point& p1)
{
   long x2,y2;

   double dx  = (double) (p1.x - p0.x);
   double dy  = (double) (p1.y - p0.y);

   double a   = (dx * dx - dy * dy) / (dx * dx + dy*dy);
   double b   = 2 * dx * dy / (dx*dx + dy*dy);

   x2  = round(a * (p.x - p0.x) + b*(p.y - p0.y) + p0.x);
   y2  = round(b * (p.x - p0.x) - a*(p.y - p0.y) + p0.y);

   cv::Point pm = Point((int)x2,(int)y2);

   return pm;

}

double CentersSkDet::doEstimate( std::vector<cv::Point>& outerContour, double lineK, cv::Mat* debugImage )
{

	cv::Point top;
	cv::Point bottom;
	double distance = 0;
	cv::Rect bbox = cv::boundingRect(outerContour);
	cv::Point center(bbox.x + bbox.width / 2, bbox.y + bbox.height / 2);
	findTopBottomPoints(center, outerContour, lineK, top, bottom, distance);

	double delta = distance * precision;

	std::vector<double> offsets;
	offsets.push_back(0.9);
	offsets.push_back(0.85);
	offsets.push_back(0.80);
	offsets.push_back(0.75);
	offsets.push_back(0.7);
	offsets.push_back(0.65);
	std::vector<SymPoints> symPoints;
	findPointsInBand(outerContour, center, lineK, distance, delta, offsets, symPoints );

	//ziskani souradnic Y
	//cv::Rect bbox = cv::boundingRect(outerContour);
	int topPoint = bbox.y + bbox.height;
	int bottomPoint = 0;
	for (int c = 0; c < outerContour.size();c++)
	{
		if(outerContour[c].y < topPoint)
		{
			topPoint = outerContour[c].y;
		}
		if(outerContour[c].y > bottomPoint)
		{
			bottomPoint = outerContour[c].y;
		}
	}

	//vypocet velikosti pisma
	int letterSize = bottomPoint - topPoint;
	assert(letterSize >1);
	double addEdgeThickness = letterSize * precision;
	double addEdgeThickness2 = letterSize * precision * 4;

	topPoint = topPoint + (1.0 - zoneOffset) * letterSize;
	bottomPoint = bottomPoint - (1.0 - zoneOffset) * letterSize;


	//ziskani souradnic X
	int TLX = bbox.x + bbox.width;
	int TLX2 = TLX;
	int TRX = 0;
	int TRX2 = 0;
	int BLX = bbox.x + bbox.width;
	int BLX2 = BLX;
	int BRX = 0;
	int BRX2 = 0;
	for (int c = 0; c < outerContour.size();c++)
	{
		if(outerContour[c].y <= (topPoint + addEdgeThickness) && outerContour[c].y >= topPoint)
		{
			TLX = MIN(TLX, outerContour[c].x);
			TRX = MAX(TRX, outerContour[c].x);
		}

		if(outerContour[c].y <= (topPoint + addEdgeThickness2) && outerContour[c].y >= topPoint)
		{
			TLX2 = MIN(TLX2, outerContour[c].x);
			TRX2 = MAX(TRX2, outerContour[c].x);
		}

		if(outerContour[c].y >= (bottomPoint - addEdgeThickness) && outerContour[c].y <= bottomPoint)
		{
			BLX = MIN(BLX, outerContour[c].x);
			BRX = MAX(BRX, outerContour[c].x);
		}

		if(outerContour[c].y >= (bottomPoint - addEdgeThickness2) && outerContour[c].y <= bottomPoint)
		{
			BLX2 = MIN(BLX2, outerContour[c].x);
			BRX2 = MAX(BRX2, outerContour[c].x);
		}
	}
	//konstrukce krajnich bodu ze souradic X a Y
	Point TL(TLX, topPoint);
	Point TR(TRX, topPoint);
	Point BL(BLX, bottomPoint);
	Point BR(BRX, bottomPoint);

	//pomocne body pro vztvareni usecek
	Point P1(0, topPoint + addEdgeThickness);
	Point P2(bbox.width+bbox.x, topPoint + addEdgeThickness);
	Point P3(0, bottomPoint - addEdgeThickness);
	Point P4(bbox.width+bbox.x, bottomPoint - addEdgeThickness);

	/*Point P1(0, topPoint + 100*precision);
			Point P2(mask.cols, topPoint + 100*precision);
			Point P3(0, bottomPoint - 100*precision);
			Point P4(mask.cols, bottomPoint - 100*precision);*/

	//ziskani prostrednich bodu
	Point TM((TL.x + TR.x)/2.0,TL.y);
	cv::Point TMo = TM;
	Point BM((BL.x + BR.x)/2.0,BR.y);
	cv::Point BMo = BM;

	//vypocet uhlu zkoseni
	float angle=0, angle2 = 0, deltaX=0, deltaY=0;
	deltaX = TM.x - BM.x;
	deltaY = BM.y - TM.y;
	if(deltaY != 0)
		angle = atan((deltaX)*1.0/(deltaY));
	float deltaX2 = (TLX2 + TRX2) / 2 - (BLX2 + BRX2) / 2;
	if(deltaY != 0)
		angle2 = atan((deltaX2)*1.0/(deltaY));
	//uhel promenne angle je v radianech

	int symPointsCount = 0;
	for(size_t i = 0; i < symPoints.size(); i++)
	{
		cv::Point mlt = mirror(symPoints[i].leftTop, TMo, BMo);
		cv::Point delta = symPoints[i].rightTop - mlt;
		double dist =  (delta.x * delta.x + delta.y * delta.y);
		if( dist < (letterSize * 0.05 * letterSize * 0.05) )
			symPointsCount++;
		mlt = mirror(symPoints[i].leftBottom, TMo, BMo);
		delta = symPoints[i].rightBottom - mlt;
		dist =  (delta.x * delta.x + delta.y * delta.y);
		if( dist < (letterSize * 0.05 * letterSize * 0.05) )
			symPointsCount++;
	}
	this->probMeasure2 = 0.88 * symPointsCount / (2.0 * symPoints.size());


	if(debugImage != NULL)
	{
		int borderForVis=10;
		bbox.y -= 1;

		Mat& drawing =  *debugImage;
		drawing =  Mat::zeros( bbox.height*scalefactor+borderForVis, bbox.width*scalefactor+borderForVis, CV_8UC3 ) + cv::Scalar(255, 255, 255);
		Scalar color = Scalar( 0, 0, 0 );
		std::vector<cv::Point> outerContourNorm;
		for(size_t j = 0; j < outerContour.size(); j++)
		{
			outerContourNorm.push_back(cv::Point((outerContour[j].x - bbox.x)*scalefactor, (outerContour[j].y - bbox.y)*scalefactor));
			//cv::circle(drawing, outerContourNorm[j], 2, cv::Scalar(0, 255, 255), 1);
		}
		std::vector<std::vector<cv::Point> > contours;
		contours.push_back(outerContourNorm);
		drawContours( drawing, contours, 0, color, 1, 8);

		cv::Point offset(bbox.x, bbox.y);
		//scaling up the points
		offset.x *=scalefactor;
		offset.y *=scalefactor;

		P1.x *=scalefactor;
		P1.y *=scalefactor;
		P2.x *=scalefactor;
		P2.y *=scalefactor;
		P3.x *=scalefactor;
		P3.y *=scalefactor;
		P4.x *=scalefactor;
		P4.y *=scalefactor;

		TL.x *=scalefactor;
		TL.y *=scalefactor;
		TR.x *=scalefactor;
		TR.y *=scalefactor;
		TM.x *=scalefactor;
		TM.y *=scalefactor;

		BL.x *=scalefactor;
		BL.y *=scalefactor;
		BR.x *=scalefactor;
		BR.y *=scalefactor;
		BM.x *=scalefactor;
		BM.y *=scalefactor;

		cv::line(drawing, TL-offset, TR-offset, cv::Scalar(50, 50, 50), 1 );
		cv::line(drawing, P1-offset, P2-offset, cv::Scalar(50, 50, 50), 1 );
		cv::line(drawing, BL-offset, BR-offset, cv::Scalar(50, 50, 50), 1 );
		cv::line(drawing, P3-offset, P4-offset, cv::Scalar(50, 50, 50), 1 );
		//cv::line(drawing, (TL - offset), (TR - offset), cv::Scalar(0, 255, 0), 2 );
		//cv::line(drawing, (BL - offset), (BR - offset), cv::Scalar(0, 255, 0), 2 );
		cv::circle(drawing, (TM - offset), 4, cv::Scalar(0, 0, 255), 2);
		cv::circle(drawing, (BM - offset), 4, cv::Scalar(0, 0, 255), 2);

		for(size_t i = 0; i < symPoints.size(); i++)
		{
			cv::Point mlt = mirror(symPoints[i].leftTop, TMo, BMo);
			cv::Point delta = symPoints[i].rightTop - mlt;
			double dist =  (delta.x * delta.x + delta.y * delta.y);
			cv::Scalar color(0, 255, 0);
			if( dist > (letterSize * 0.05 * letterSize * 0.05) )
				color = cv::Scalar(0, 0, 255);
			cv::Point normLeft((symPoints[i].leftTop.x - bbox.x)*scalefactor, (symPoints[i].leftTop.y - bbox.y)*scalefactor);
			cv::circle(drawing, normLeft, 4, color, 2);
			cv::Point normRight((symPoints[i].rightTop.x - bbox.x)*scalefactor, (symPoints[i].rightTop.y - bbox.y)*scalefactor);
			cv::circle(drawing, normRight, 4, cv::Scalar(0, 255, 0), 2);
			cv::line(drawing, normLeft, normRight, cv::Scalar(255, 0, 0), 1 );

			mlt = mirror(symPoints[i].leftBottom, TMo, BMo);
			delta = symPoints[i].rightBottom - mlt;
			dist =  (delta.x * delta.x + delta.y * delta.y);
			color = cv::Scalar(0, 255, 0);
			if( dist > (letterSize * 0.05 * letterSize * 0.05) )
				color = cv::Scalar(0, 0, 255);
			cv::Point normLeftB((symPoints[i].leftBottom.x - bbox.x)*scalefactor, (symPoints[i].leftBottom.y - bbox.y)*scalefactor);
			cv::circle(drawing, normLeftB, 4, color, 2);
			cv::Point normRightB((symPoints[i].rightBottom.x - bbox.x)*scalefactor, (symPoints[i].rightBottom.y - bbox.y)*scalefactor);
			cv::circle(drawing, normRightB, 4, cv::Scalar(0, 255, 0), 2);
			cv::line(drawing, normLeftB, normRightB, cv::Scalar(255, 0, 0), 1 );
		}

		//cv::imshow("CentersSk", drawing);
		//cv::waitKey(0);
	}
	this->lastDetectionProbability = this->probMeasure2;
	return angle;
}

double CentersSkDet::detectSkew( std::vector<cv::Point>& outerContourO, double lineK, bool approximate, cv::Mat* debugImage )
{
	std::vector<cv::Point> outerContour = outerContourO;
	double angleAcc = 0;
	std::vector<cv::Point2f> workCont;
	workCont.resize(outerContour.size());
	for(size_t i = 0; i < workCont.size(); i++)
	{
		workCont[i] = outerContour[i];
	}
	int level = 0;
	while(true)
	{
		if( level > 0)
		{
			for(size_t i = 0; i < workCont.size(); i++)
			{
				outerContour[i] = cv::Point(round(workCont[i].x), round(workCont[i].y));
			}
		}

        double angle;
        if (useMaxMin)
        	angle = doEstimate2( outerContour, debugImage );
		else
			angle = doEstimate( outerContour, lineK, debugImage );
        
		angleAcc += angle;
		if(fabs(angle) < (M_PI / 180) || !recursive || level > 11 )
		{
			break;
		}
		double skewValue = (float) tan(angle);
		for( size_t i = 0; i < workCont.size(); i++ )
		{
			workCont[i].x = workCont[i].x + skewValue * workCont[i].y;
		}
		level++;
	}
	return angleAcc;
}

void CentersSkDet::voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate, cv::Mat* debugImage)
{
	double angle = detectSkew( outerContour, false, debugImage);
	int angleDeg = angle * 180 / M_PI;
 	angleDeg += 90;
	int sigma = 3;
	int range = 3;
	for (int i = angleDeg-sigma*range; i <= angleDeg+sigma*range; i++)
	{
		int j = i;
		if(j < 0) j += 180;
		if (j >= 180) j -= 180;

		histogram[j] =  histogram[j] + weight * this->lastDetectionProbability/(sqrt(2*M_PI)*sigma)*pow(M_E, -(i - angleDeg)*(i - angleDeg)/(2*sigma*sigma));
	}
}

} /* namespace cmp */

