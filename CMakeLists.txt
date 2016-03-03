
cmake_minimum_required(VERSION 2.6)
project(SkewDetection)

option (BUILD_TEST "Build test cases" OFF)
option (BUILD_DOC "Build the documentation" OFF)

#Find opencv
find_package( OpenCV REQUIRED )

# ----------------------------------------------------------------------------
# Output directories 
# ----------------------------------------------------------------------------
if(ANDROID)
    set(LIBRARY_OUTPUT_PATH         "${CMAKE_BINARY_DIR}/lib/${ANDROID_NDK_ABI_NAME}")
    set(EXECUTABLE_OUTPUT_PATH      "${CMAKE_BINARY_DIR}/bin")
    
else()
    set(LIBRARY_OUTPUT_PATH         "${CMAKE_BINARY_DIR}/lib")
    set(EXECUTABLE_OUTPUT_PATH      "${CMAKE_BINARY_DIR}") 
    set(3P_LIBRARY_OUTPUT_PATH      "${CMAKE_BINARY_DIR}/3rdparty/lib${LIB_SUFFIX}")
endif()


add_definitions("-std=c++0x")

if(NOT WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(NOT WIN32)

if (USE_OPENMP)
    add_definitions("-DDO_PARALLEL")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS} /openmp")
    else(MSVC)
        find_package(OpenMP)
        
        if(OPENMP_FOUND)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        else(OPENMP_FOUND)
            message(WARNING "OpenMP NOT found")
        endif(OPENMP_FOUND)
    endif(MSVC)
endif()

file(GLOB lib_srcs "src/*.cpp")
LIST(REMOVE_ITEM lib_srcs "src/run_evaluation.cpp")
LIST(REMOVE_ITEM lib_srcs "src/detect_skew.cpp")
LIST(REMOVE_ITEM lib_srcs "src/detect_word_skew.cpp")
LIST(REMOVE_ITEM lib_srcs "src/word_skew_evaluator.cpp")

add_library(skewDetection
    ${lib_srcs}
)

#----------------Main programs -------------------------------------------------

add_executable(run_evaluation "src/run_evaluation.cpp")

target_link_libraries (run_evaluation     
    skewDetection
    ${OpenCV_LIBS}
)

add_executable(run_word_evaluation "src/run_word_evaluation.cpp")

target_link_libraries (run_word_evaluation     
    skewDetection
    ${OpenCV_LIBS}
)

add_executable(word_skew_evaluator "src/word_skew_evaluator.cpp")

target_link_libraries (word_skew_evaluator
    skewDetection
    ${OpenCV_LIBS}
)

add_executable(detect_word_skew "src/detect_word_skew.cpp")
add_executable(detect_skew "src/detect_skew.cpp")

target_link_libraries (detect_skew     
    skewDetection
    ${OpenCV_LIBS}
)

target_link_libraries (detect_word_skew     
    skewDetection
    ${OpenCV_LIBS}
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

if(ANDROID)
  include(cmake/OpenCVDetectAndroidSDK.cmake REQUIRED)

  if(NOT ANDROID_TOOLS_Pkg_Revision GREATER 13)
    message(WARNING "OpenCV requires Android SDK tools revision 14 or newer. Otherwise tests and samples will no be compiled.")
  endif()
  
  # Generate FastText.mk for ndk-build (Android build tool)
  include(cmake/SKDetGenAndroidMK.cmake REQUIRED)
endif(ANDROID)

# ---------------------------------------------------------------------------
# Build the documentation
# ---------------------------------------------------------------------------
if(BUILD_DOC)
    message( STATUS "Creating documentation ...")
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(
            doc ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    endif(DOXYGEN_FOUND)
endif (BUILD_DOC)

export(TARGETS skewDetection FILE "${PROJECT_BINARY_DIR}/skewDetectionLibraryDepends.cmake")
configure_file(skewDetectionConfig.cmake.in
  "${PROJECT_BINARY_DIR}/skewDetectionConfig.cmake" @ONLY)
