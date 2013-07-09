
cmake_minimum_required(VERSION 2.6)
project(SkewDetection)

option (BUILD_TEST "Build test cases" OFF)

#Find opencv
find_package( OpenCV REQUIRED )

file(GLOB lib_srcs "src/*.cpp")

add_library(skewDetection
    ${lib_srcs}
)

#----------------GTest---------------------------------------------------------
if(BUILD_TEST AND NOT ANDROID)
    message( STATUS "Adding module test ...")
    add_subdirectory (${CMAKE_SOURCE_DIR}/3rdparty/gtest-1.6.0)
    file(GLOB test_srcs "test/*.cpp")
    
    add_executable(run_all_tests ${test_srcs})

    target_link_libraries (run_all_tests     
    skewDetection
    
    gtest
    ${OpenCV_LIBS}
)
    
endif(BUILD_TEST AND NOT ANDROID)