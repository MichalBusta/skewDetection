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

	CentersSkDet::CentersSkDet(int approximatioMethod, double epsilon, float precision) : ContourSkewDetector(approximatioMethod, epsilon),  precision(precision)
	{

	}

	CentersSkDet::~CentersSkDet()
	{
		// TODO Auto-generated destructor stub
	}

	double CentersSkDet::detectSkew( std::vector<cv::Point>& outerContour, cv::Mat* debugImage )
	{
		//ziskani souradnic Y
		cv::Rect bbox = cv::boundingRect(outerContour);
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
        
        

		//ziskani souradnic X
		int TLX = bbox.x + bbox.width;
		int TRX = 0;
		int BLX = bbox.x + bbox.width;
		int BRX = 0;
		for (int c = 0; c < outerContour.size();c++)
		{
			if(outerContour[c].y <= (topPoint + addEdgeThickness))
			{
				TLX = MIN(TLX, outerContour[c].x);
				TRX = MAX(TRX, outerContour[c].x);
			}
			if(outerContour[c].y >= (bottomPoint - addEdgeThickness))
			{
				BLX = MIN(BLX, outerContour[c].x);
				BRX = MAX(BRX, outerContour[c].x);
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
		Point BM((BL.x + BR.x)/2.0,BR.y);

		//vypocet uhlu zkoseni
		float angle=0, deltaX=0, deltaY=0;
		deltaX = TM.x - BM.x;
		deltaY = BM.y - TM.y;
		angle = atan((deltaX)*1.0/(deltaY));
		//uhel promenne angle je v radianech

		if(debugImage != NULL)
		{
            int borderForVis=10;
            
			Mat& drawing =  *debugImage;
			drawing =  Mat::zeros( bbox.height*scalefactor+borderForVis, bbox.width*scalefactor+borderForVis, CV_8UC3 );
			Scalar color = Scalar( 255, 255, 255 );
			std::vector<cv::Point> outerContourNorm;
			for(size_t j = 0; j < outerContour.size(); j++)
			{
				outerContourNorm.push_back(cv::Point((outerContour[j].x - bbox.x)*scalefactor, (outerContour[j].y - bbox.y)*scalefactor));
				cv::circle(drawing, outerContourNorm[j], 2, cv::Scalar(0, 255, 255), 1);
			}
			std::vector<std::vector<cv::Point> > contours;
			contours.push_back(outerContourNorm);
			drawContours( drawing, contours, 0, color, 1, 8);

			cv::Point offset(bbox.x, bbox.y);
            //scaling up the points
            offset.x *=scalefactor;
            offset.y *=scalefactor;
            
            P1.x = P1.x*scalefactor;
            P1.y = P1.y*scalefactor;
            P2.x = P2.x*scalefactor;
            P2.y = P2.y*scalefactor;
            P3.x = P3.x*scalefactor;
            P3.y = P3.y*scalefactor;
            P4.x = P4.x*scalefactor;
            P4.y = P4.y*scalefactor;
            
            TL.x = TL.x*scalefactor;
            TL.y = TL.y*scalefactor;
            TR.x = TR.x*scalefactor;
            TR.y = TR.y*scalefactor;
            TM.x = TM.x*scalefactor;
            TM.y = TM.y*scalefactor;
            
            BL.x = BL.x*scalefactor;
            BL.y = BL.y*scalefactor;
            BR.x = BR.x*scalefactor;
            BR.y = BR.y*scalefactor;
            BM.x = BM.x*scalefactor;
            BM.y = BM.y*scalefactor;
            
			cv::line(drawing, P1-offset, P2-offset, cv::Scalar(255, 255, 0), 1 );
			cv::line(drawing, P3-offset, P4-offset, cv::Scalar(255, 255, 0), 1 );
			cv::line(drawing, (TL - offset), (TR - offset), cv::Scalar(0, 255, 0), 2 );
			cv::line(drawing, (BL - offset), (BR - offset), cv::Scalar(0, 255, 0), 2 );
			cv::circle(drawing, (TM - offset), 4, cv::Scalar(0, 0, 255), 2);
			cv::circle(drawing, (BM - offset), 4, cv::Scalar(0, 0, 255), 2);
		}

		return angle;
	}

} /* namespace cmp */

