#
#  This file is part of guiderlogviewer
# 
#  Copyright (C) 2016 Andy Galasso
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, visit the http://fsf.org website.

cmake_minimum_required (VERSION 3.10)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if (POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
  endif()
endif(COMMAND cmake_policy)

project (guiderlogviewer)

set(srcdir ${CMAKE_SOURCE_DIR}/src/)

include_directories(${CMAKE_BINARY_DIR})
#include_directories(${srcdir})

add_definitions(-D_USE_MATH_DEFINES)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
"MinSizeRel" "RelWithDebInfo")
endif()

include(CheckCXXCompilerFlag)

#################################################################################
#

# c++23 options
check_cxx_compiler_flag(-std=c++23 HAS_CXX23_FLAG)
check_cxx_compiler_flag(-std=c++20 HAS_CXX20_FLAG)
check_cxx_compiler_flag(-std=c++17 HAS_CXX17_FLAG)
check_cxx_compiler_flag(-std=c++11 HAS_CXX11_FLAG)
check_cxx_compiler_flag(-std=c++0x HAS_CXX0X_FLAG)

if(HAS_CXX22_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
elseif(HAS_CXX17_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
elseif(HAS_CXX17_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(HAS_CXX11_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(HAS_CXX0X_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if(wxWidgets_PREFIX_DIRECTORY)
    set(wxWidgets_CONFIG_OPTIONS --prefix=${wxWidgets_PREFIX_DIRECTORY})

    find_program(wxWidgets_CONFIG_EXECUTABLE NAMES "wx-config" PATHS
${wxWidgets_PREFIX_DIRECTORY}/bin NO_DEFAULT_PATH)
    if(NOT wxWidgets_CONFIG_EXECUTABLE)
      message(FATAL_ERROR "Cannot find wxWidgets_CONFIG_EXECUTABLE from the
given directory ${wxWidgets_PREFIX_DIRECTORY}")
    endif()
  endif()

  find_package(wxWidgets REQUIRED COMPONENTS aui core base adv html net)
  if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "WxWidget cannot be found. Please use wx-config
prefix")
endif()

set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${wxWidgets_LIBRARIES})

set(SRC
  ${srcdir}/logviewer/AnalysisWin.cpp
  ${srcdir}/logviewer/AnalysisWin.h
  ${srcdir}/logviewer/LogViewApp.cpp
  ${srcdir}/logviewer/LogViewApp.h
  ${srcdir}/logviewer/LogViewFrame.cpp
  ${srcdir}/logviewer/LogViewFrame.h
  ${srcdir}/logviewer/logparser.cpp
  ${srcdir}/logviewer/logparser.h
  ${srcdir}/logviewer/guiderlogviewer.ico
  ${srcdir}/logviewer/guiderlogviewer.rc
  ${srcdir}/logviewer/small.ico
)

set (FBSRC
  ${srcdir}/logviewer/LogViewFrameBase.cpp
  ${srcdir}/logviewer/LogViewFrameBase.h
)
set (ALL_SRC
  ${SRC}
  ${FBSRC}
)

add_executable(guiderlogviewer ${ALL_SRC})

find_package(GSL REQUIRED)
if(APPLE)
  # force static loining on OSX to avoid redistributing GSL .dylib files
  # this is a hack... what's the real way to do this in CMake?
  string(REPLACE ".dylib" ".a" GSL_LIBRARIES "${GSL_LIBRARIES}")
  set(APP_LINK_EXTERNAL ${APP_LINK_EXTERNAL} ${GSL_LIBRARIES})
  include_directories(${GSL_INCLUDE_DIR})
else()
  target_link_libraries(guiderlogviewer GSL::gsl GSL::gslcblas)
endif()

set_target_properties(
    guiderlogviewer
    PROPERTIES
      OUTPUT_NAME lightguider_logviewer
    )

target_compile_definitions( guiderlogviewer PRIVATE "${wxWidgets_DEFINITIONS}" "HAVE_TYPE_TRAITS")
target_compile_options(     guiderlogviewer PRIVATE "${wxWidgets_CXX_FLAGS};")
target_link_libraries(guiderlogviewer ${APP_LINK_EXTERNAL})
target_include_directories(guiderlogviewer PRIVATE ${wxWidgets_INCLUDE_DIRS})

if(UNIX AND NOT APPLE)
  install(TARGETS guiderlogviewer
          RUNTIME DESTINATION bin)
  install(FILES ${CMAKE_SOURCE_DIR}/logviewer/guiderlogviewer.png
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/ )
  install(FILES ${CMAKE_SOURCE_DIR}/logviewer/guiderlogviewer.desktop
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/ )
endif()
