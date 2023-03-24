# Copyright 2014-2015, Max Planck Society.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

# File created by Raffi Enficiaud

# custom cmake packages, should have lower priority than the ones bundled with cmake
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules/ )



# these variables allow to specify to which the main project will link and
# to potentially copy some resources to the output directory of the main project.
# They are used by the CMakeLists.txt calling this file.

set(LGuider_LINK_EXTERNAL)          # target to which the phd2 main library will link to
set(LGuider_COPY_EXTERNAL_ALL)      # copy of a file for any configuration
set(LGuider_COPY_EXTERNAL_DBG)      # copy for debug only
set(LGuider_COPY_EXTERNAL_REL)      # copy for release only


# this module will be used to find system installed libraries on Linux
if(UNIX AND NOT APPLE)
  find_package(PkgConfig)
endif()

if(APPLE)
  find_library(quicktimeFramework      QuickTime)
  find_library(iokitFramework          IOKit)
  find_library(carbonFramework         Carbon)
  find_library(cocoaFramework          Cocoa)
  find_library(systemFramework         System)
  find_library(webkitFramework         Webkit)
  find_library(audioToolboxFramework   AudioToolbox)
  find_library(openGLFramework         OpenGL)
  find_library(coreFoundationFramework CoreFoundation)
endif()

##############################################
# cfitsio
set(USE_SYSTEM_CFITSIO ON)
if(USE_SYSTEM_CFITSIO)
  find_package(CFITSIO REQUIRED)
  include_directories(${CFITSIO_INCLUDE_DIR})
  set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${CFITSIO_LIBRARIES})
  message(STATUS "Using system's CFITSIO.")
else(USE_SYSTEM_CFITSIO)

  set(CFITSIO_MAJOR_VERSION 3)
  set(CFITSIO_MINOR_VERSION 47)
  set(CFITSIO_VERSION ${CFITSIO_MAJOR_VERSION}.${CFITSIO_MINOR_VERSION})

  set(libcfitsio_root ${thirdparties_deflate_directory}/cfitsio-${CFITSIO_VERSION})

  if(NOT EXISTS ${libcfitsio_root})
    # untar the dependency
    message(STATUS "[thirdparty] untarring cfitsio")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${thirdparty_dir}/cfitsio-${CFITSIO_VERSION}-patched.tar.gz
                    WORKING_DIRECTORY ${thirdparties_deflate_directory})
  endif()

  # copied and adapted from the CMakeLists.txt of cftsio project. The sources of the project
  # are left untouched

  file(GLOB CFTSIO_H_FILES "${libcfitsio_root}/*.h")

  # OpenPhdGuiding COMMENT HERE
  # Raffi: these should also be cleaned (link against zlib of the system)

  set(CFTSIO_SRC_FILES
    buffers.c cfileio.c checksum.c drvrfile.c drvrmem.c
    drvrnet.c drvrsmem.c drvrgsiftp.c editcol.c edithdu.c eval_l.c
    eval_y.c eval_f.c fitscore.c getcol.c getcolb.c getcold.c getcole.c
    getcoli.c getcolj.c getcolk.c getcoll.c getcols.c getcolsb.c
    getcoluk.c getcolui.c getcoluj.c getkey.c group.c grparser.c
    histo.c iraffits.c
    modkey.c putcol.c putcolb.c putcold.c putcole.c putcoli.c
    putcolj.c putcolk.c putcoluk.c putcoll.c putcols.c putcolsb.c
    putcolu.c putcolui.c putcoluj.c putkey.c region.c scalnull.c
    swapproc.c wcssub.c wcsutil.c imcompress.c quantize.c ricecomp.c
    pliocomp.c fits_hcompress.c fits_hdecompress.c zlib/zuncompress.c
    zlib/zcompress.c zlib/adler32.c zlib/crc32.c zlib/inffast.c
    zlib/inftrees.c zlib/trees.c zlib/zutil.c zlib/deflate.c
    zlib/infback.c zlib/inflate.c zlib/uncompr.c simplerng.c
    f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c
  )

  foreach(_src_file IN LISTS CFTSIO_SRC_FILES)
    set(CFTSIO_SRC_FILES_rooted "${CFTSIO_SRC_FILES_rooted}" ${libcfitsio_root}/${_src_file})
  endforeach()

  add_library(cfitsio STATIC ${CFTSIO_H_FILES} ${CFTSIO_SRC_FILES_rooted})
  target_include_directories(cfitsio PUBLIC ${libcfitsio_root}/)

  # OpenPhdGuiding MODIFICATION HERE: we link against math library only on UNIX
  if(UNIX)
    target_link_libraries(cfitsio m)
  endif()

  # OpenPhdGuiding MODIFICATION HERE: suppress warning about unused function result
  if(UNIX AND NOT APPLE)
    set_target_properties(cfitsio PROPERTIES COMPILE_FLAGS "-Wno-unused-result")
    # Raffi: use target_compile_options ?
  endif()

  if(APPLE)
    set_target_properties(cfitsio PROPERTIES COMPILE_FLAGS "-DHAVE_UNISTD_H")
  endif()

  if(WIN32)
    target_compile_definitions(
      cfitsio
      PRIVATE BUILDING_CFITSIO
      PRIVATE FF_NO_UNISTD_H
      PRIVATE _CRT_SECURE_NO_WARNINGS
      PRIVATE _CRT_SECURE_NO_DEPRECATE)
  endif()

  set_target_properties(cfitsio PROPERTIES
                        VERSION ${CFITSIO_VERSION}
                        SOVERSION ${CFITSIO_MAJOR_VERSION}
                        FOLDER "Thirdparty/")


  # indicating the link and include directives to the main project.
  # already done by the directive target_include_directories(cfitsio PUBLIC
  # include_directories(${libcfitsio_root})
  set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} cfitsio)

endif(USE_SYSTEM_CFITSIO)

#############################################
# libcurl
#############################################

if(WIN32)
  set(libcurl_root ${thirdparties_deflate_directory}/libcurl)
  set(libcurl_dir ${libcurl_root}/libcurl-7.54.0-win32)
  if(NOT EXISTS ${libcurl_root})
    # unzip the dependency
    file(MAKE_DIRECTORY ${libcurl_root})
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/thirdparty/libcurl-7.54.0-win32.zip --format=zip
        WORKING_DIRECTORY ${libcurl_root})
  endif()
  include_directories(${libcurl_dir}/include)
  set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${libcurl_dir}/lib/LIBCURL.LIB)
else()
  if(APPLE)
    # make sure to pick up the macos curl, not the mapcports curl in /opt/local/lib
    find_library(CURL_LIBRARIES
                 NAMES curl
		 PATHS /usr/lib
		 NO_DEFAULT_PATH)
    if(NOT CURL_LIBRARIES)
      message(FATAL_ERROR "libcurl not found")
    endif()
    set(CURL_FOUND True)
    set(CURL_INCLUDE_DIRS /usr/include)
  else()
    find_package(CURL REQUIRED)
  endif()
  message(STATUS "Using system's libcurl. ${CURL_LIBRARIES}")
  include_directories(${CURL_INCLUDE_DIRS})
  set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${CURL_LIBRARIES})
endif()

#############################################
# the Eigen library, mostly header only
set(USE_SYSTEM_EIGEN3 On)
if(USE_SYSTEM_EIGEN3)
  find_package(Eigen3 REQUIRED)
  set(EIGEN_SRC ${EIGEN3_INCLUDE_DIR})
  message(STATUS "Using system's Eigen3. ${EIGEN3_INCLUDE_DIR}")
else(USE_SYSTEM_EIGEN3)

endif(USE_SYSTEM_EIGEN3)

#############################################
# wxWidgets
# The usage is a bit different on all the platforms. For having version >= 3.0, a version of cmake >= 3.0 should be used on Windows (on Linux/OSX it works properly this way).

if(wxWidgets_PREFIX_DIRECTORY)
  set(wxWidgets_CONFIG_OPTIONS --prefix=${wxWidgets_PREFIX_DIRECTORY})
  find_program(wxWidgets_CONFIG_EXECUTABLE NAMES "wx-config" PATHS ${wxWidgets_PREFIX_DIRECTORY}/bin NO_DEFAULT_PATH)
  if(NOT wxWidgets_CONFIG_EXECUTABLE)
    message(FATAL_ERROR "Cannot find wxWidgets_CONFIG_EXECUTABLE from the given directory ${wxWidgets_PREFIX_DIRECTORY}")
  endif()
endif()

find_package(wxWidgets REQUIRED COMPONENTS aui core base adv html net)
if(NOT wxWidgets_FOUND)
  message(FATAL_ERROR "WxWidget cannot be found. Please use wx-config prefix")
endif()
set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${wxWidgets_LIBRARIES})


#############################################
#
#  INDI
#
#############################################

if(WIN32)
  set(indi_zip ${CMAKE_SOURCE_DIR}/thirdparty/indiclient-44aaf5d3-win32.zip)
  set(indiclient_root ${thirdparties_deflate_directory})
  set(indiclient_dir ${indiclient_root}/indiclient)
  if(NOT EXISTS ${indiclient_dir})
    message(STATUS "[thirdparty] untarring indiclient")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${indi_zip}
                    WORKING_DIRECTORY ${indiclient_root})
  endif()
  include_directories(${indiclient_dir}/include)
  set(LGuider_LINK_EXTERNAL_RELEASE ${LGuider_LINK_EXTERNAL_RELEASE} ${indiclient_dir}/lib/indiclient.lib)
  set(LGuider_LINK_EXTERNAL_DEBUG ${LGuider_LINK_EXTERNAL_DEBUG} ${indiclient_dir}/lib/indiclientd.lib)
else()
  # Linux or OSX
  set(USE_SYSTEM_LIBINDI On)
  if(USE_SYSTEM_LIBINDI)
  find_package(INDI REQUIRED)
    message(STATUS "Using system's libindi. ")
    # INDI
    
    # source files include <libindi/baseclient.h> so we need the libindi parent directory in the include directories
    include_directories(${INDI_INCLUDE_DIR})
    set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${INDI_CLIENT_LIBRARIES})

    find_package(ZLIB REQUIRED)
    set(LGuider_LINK_EXTERNAL ${LGuider_LINK_EXTERNAL} ${ZLIB_LIBRARIES})
  else()
    
  endif()
endif()

#############################################
#
# gettext and msgmerge tools for documentation/internationalization
#
#############################################

set(GETTEXT_FINDPROGRAM_OPTIONS)
if(NOT ("${GETTEXT_ROOT}" STREQUAL ""))
  set(GETTEXT_FINDPROGRAM_OPTIONS
      PATHS ${GETTEXT_ROOT}
               PATH_SUFFIXES bin
               DOC "gettext program deflated from the thirdparties"
               NO_DEFAULT_PATH)
endif()

find_program(XGETTEXT
             NAMES xgettext
             ${GETTEXT_FINDPROGRAM_OPTIONS})

find_program(MSGFMT
              NAMES msgfmt
             ${GETTEXT_FINDPROGRAM_OPTIONS})

find_program(MSGMERGE
              NAMES msgmerge
             ${GETTEXT_FINDPROGRAM_OPTIONS})

if(NOT XGETTEXT)
  message(STATUS "xgettext program not found")
else()
  message(STATUS "Found xgettext program at '${XGETTEXT}'")
endif()

if(NOT MSGFMT)
  message(STATUS "msgfmt program not found")
else()
  message(STATUS "Found msgfmt program at '${MSGFMT}'")
endif()

if(NOT MSGMERGE)
  message(STATUS "msgmerge program not found")
else()
  message(STATUS "Found msgmerge program at '${MSGMERGE}'")
endif()
