

if(ANDROID)


  # --------------------------------------------------------------------------------------------
  #  Installation for Android ndk-build makefile:  FastText.mk
  #  Part 1/2: ${BIN_DIR}/FastText.mk              -> For use *without* "make install"
  # -------------------------------------------------------------------------------------------

  # build the list of libs and dependencies for all modules
  set(SK_3RDPARTY_COMPONENTS_CONFIGMAKE "skewDetection" )

  # -------------------------------------------------------------------------------------------
  #  Part 1/2: ${BIN_DIR}/FastText.mk              -> For use *without* "make install"
  # -------------------------------------------------------------------------------------------
  set(SK_INCLUDE_DIRS_CONFIGCMAKE "\"${CMAKE_SOURCE_DIR}/src\"")
  set(SK_LIBS_DIR_CONFIGCMAKE "\$(SK_THIS_DIR)/lib/\$(TARGET_ARCH_ABI)")

  configure_file("${CMAKE_SOURCE_DIR}/cmake/templates/SKDet.mk.in" "${CMAKE_BINARY_DIR}/SKDet.mk" IMMEDIATE @ONLY)

endif(ANDROID)
