/*
 * run_all_tests.cpp
 *
 *  Created on: Aug 16, 2012
 *      Author: Michal Busta
 */

#include <gtest/gtest.h>



int main( int argc, char **argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}



