# ----------------------------------------------------------------------------
# Detect Microsoft compiler:
# ----------------------------------------------------------------------------
if(CMAKE_CL_64)
    set(MSVC64 1)
endif()

# ----------------------------------------------------------------------------
# Detect GNU version:
# ----------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX AND NOT ANDROID)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER}  ${CMAKE_CXX_COMPILER_ARG1} --version
                  OUTPUT_VARIABLE CMAKE_OPENCV_GCC_VERSION_FULL
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
    
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} -v
                  ERROR_VARIABLE CMAKE_OPENCV_GCC_INFO_FULL
                  OUTPUT_STRIP_TRAILING_WHITESPACE)           
        
    # Typical output in CMAKE_OPENCV_GCC_VERSION_FULL: "c+//0 (whatever) 4.2.3 (...)"
    # Look for the version number
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" CMAKE_GCC_REGEX_VERSION "${CMAKE_OPENCV_GCC_VERSION_FULL}")
    if(NOT CMAKE_GCC_REGEX_VERSION)
      string(REGEX MATCH "[0-9]+\\.[0-9]+" CMAKE_GCC_REGEX_VERSION "${CMAKE_OPENCV_GCC_VERSION_FULL}")
    endif()

    # Split the three parts:
    string(REGEX MATCHALL "[0-9]+" CMAKE_OPENCV_GCC_VERSIONS "${CMAKE_GCC_REGEX_VERSION}")

    list(GET CMAKE_OPENCV_GCC_VERSIONS 0 CMAKE_OPENCV_GCC_VERSION_MAJOR)
    list(GET CMAKE_OPENCV_GCC_VERSIONS 1 CMAKE_OPENCV_GCC_VERSION_MINOR)

    set(CMAKE_OPENCV_GCC_VERSION ${CMAKE_OPENCV_GCC_VERSION_MAJOR}${CMAKE_OPENCV_GCC_VERSION_MINOR})
    math(EXPR CMAKE_OPENCV_GCC_VERSION_NUM "${CMAKE_OPENCV_GCC_VERSION_MAJOR}*100 + ${CMAKE_OPENCV_GCC_VERSION_MINOR}")
    message(STATUS "Detected version of GNU GCC: ${CMAKE_OPENCV_GCC_VERSION} (${CMAKE_OPENCV_GCC_VERSION_NUM})")

    if(WIN32)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
                  OUTPUT_VARIABLE CMAKE_OPENCV_GCC_TARGET_MACHINE
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(CMAKE_OPENCV_GCC_TARGET_MACHINE MATCHES "64")
            set(MINGW64 1)
        endif()
    endif()
endif()


# ----------------------------------------------------------------------------
# Use statically or dynamically linked CRT?
# Default: dynamic
# ----------------------------------------------------------------------------
if(WIN32 AND NOT BUILD_SHARED_LIBS)
  option (BUILD_WITH_STATIC_CRT "Enables use of staticaly linked CRT" ON)
endif()
  
if(MSVC)
    if(BUILD_WITH_STATIC_CRT)
        foreach(flag_var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
           if(${flag_var} MATCHES "/MD")
              string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
           endif()
           if(${flag_var} MATCHES "/MDd")
              string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
           endif()
        endforeach(flag_var)
        
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt.lib")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd.lib")
    else(BUILD_WITH_STATIC_CRT)
        foreach(flag_var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
           if(${flag_var} MATCHES "/MT")
              string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
           endif()
           if(${flag_var} MATCHES "/MTd")
              string(REGEX REPLACE "/MTd" "/MDd" ${flag_var} "${${flag_var}}")
           endif()
        endforeach(flag_var)
    endif(BUILD_WITH_STATIC_CRT)

    if(NOT BUILD_WITH_DEBUG_INFO)
        string(REPLACE "/debug" "" CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/DEBUG" "" CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL:YES" "/INCREMENTAL:NO" CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL " "/INCREMENTAL:NO " CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

        string(REPLACE "/debug" "" CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/DEBUG" "" CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL:YES" "/INCREMENTAL:NO" CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL " "/INCREMENTAL:NO " CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}")

        string(REPLACE "/debug" "" CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/DEBUG" "" CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL:YES" "/INCREMENTAL:NO" CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
        string(REPLACE "/INCREMENTAL " "/INCREMENTAL:NO " CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

        string(REPLACE "/Zi" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    endif()

endif(MSVC)

message( STATUS "CMake Release Linker flags:  ${CMAKE_EXE_LINKER_FLAGS_RELEASE}.")
message( STATUS "CMake Debug Linker flags:  ${CMAKE_EXE_LINKER_FLAGS_DEBUG}.")
