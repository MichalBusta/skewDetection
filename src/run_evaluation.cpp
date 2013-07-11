/*
 * run_evaluation.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include "SkewEvaluator.h"
#define PI 3.14159265

using namespace cmp;

static void help()
{
	std::cout << "\nRuns evaluation \n Call:\n ./run_evaluation input_direcoty \n\n";
}

int main( int argc, char **argv)
{
 	if( argc < 2)
	{
		help();
		return -1;
	}
	int x;
	float y;
	for(x=-60;x<=60;x=x+10)
	{
		y=tan (x*PI/180);
		cv::Mat sourceImage = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
		cv::Mat transformed;
		cv::Mat affineTransform = cv::Mat::eye(2, 3, CV_32F);
		affineTransform.at<float>(0, 1) = y;
		cv::warpAffine(sourceImage, transformed, affineTransform, cv::Size(sourceImage.cols * 2, sourceImage.rows * 2), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
		imshow("Test", sourceImage);
		imshow("TestTransformed", transformed);
		cv::waitKey(0);
	}

	//TODO implement!!!
	/*
	SkewEvaluator evaluator;
	evaluator.evaluate( argv[1] );
	*/

	return 0;
}
