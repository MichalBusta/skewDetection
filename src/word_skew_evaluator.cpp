/*
* word_skew_evaluator.cpp
*
*  Created on: May 5, 2014
*      Author: David Helekal
*/

#include <iostream>
#include "SkewDetection.h"
#include "WordSkewDetection.h"
#include "WordEvaluator.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


static void help()
{
	std::cout << "\nRuns evaluation \n Call:\n ./word_skew_evaluator input_direcory output_directory \n\n";
}

int main( int argc, char **argv)
{
	if( argc < 3)
	{
		help();
		return -1;
    }
    
    cmp::WordEvaluator eval(argv[2],argv[1],true);
    eval.addWordDetector(new cmp::DiscreteVotingWordSkDet(), "DiscreteVoting");
    eval.run();
    
}
