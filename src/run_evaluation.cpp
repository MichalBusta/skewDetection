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


	//TODO implement!!!
	/*
	SkewEvaluator evaluator;
	evaluator.evaluate( argv[1] );
	*/

	return 0;
}
