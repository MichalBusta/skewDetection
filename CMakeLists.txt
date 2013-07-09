
cmake_minimum_required(VERSION 2.6)
project(SkewDetection)

#Find opencv
find_package( OpenCV REQUIRED )


file(GLOB test_srcs "test/*.cpp")
file(GLOB lib_srcs "src/*.cpp")

add_library(skewDetection
    ${lib_srcs}
)

