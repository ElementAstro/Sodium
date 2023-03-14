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

cmake_minimum_required(VERSION 3.16)

enable_language(C CXX)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    if(POLICY CMP0043)
      cmake_policy(SET CMP0043 NEW)
    endif()
endif()


# root directory of the project
set(PHD_PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(phd_src_dir ${PHD_PROJECT_ROOT_DIR})

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

#message(${PHD_PROJECT_ROOT_DIR})
# Additional files in the workspace, To improve maintainability

add_subdirectory(src/logviewer)

# compiler options
include(cmake_modules/compiler_options.cmake)

# declare useful functions about documentation
include(cmake_modules/LightGuiderBuildDoc.cmake)

# retrieves the version from the source file
get_lightguider_version()
message(STATUS "Light Guider version '${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}' (extracted from source)")

#################################################################################
#
# general defines
if(UNIX AND NOT APPLE)
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "lightguider install path" FORCE)
  endif()
endif()

set(USE_FOLDERS TRUE)
set(BUILD_SHARED_LIBS FALSE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#################################################################################
#
# main project
# this should appear after setting the architecture
project(guider)

#################################################################################
#
# external libraries
# to be changed in add_subdirectory
include(cmake_modules/thirdparty.cmake)

find_package(spdlog REQUIRED)

#################################################################################
#
#

add_subdirectory(libs/)


add_subdirectory(websrc/)

#################################################################################
#
# contributions

# PEC Guider, Max Planck Institute for Intelligent Systems, Tuebingen, Germany.
 add_subdirectory(src/contributions/MPI_IS_gaussian_process tmp_gaussian_process)

#################################################################################
#
# Global include directories
# WxWidgets inclusion narrowed to lightguider project only
# include_directories(${wxWidgets_INCLUDE_DIRS})
# include_directories(${phd_src_dir}/src/cameras/)
include_directories(${phd_src_dir}/src/)
include_directories(${phd_src_dir}/libs/)


#################################################################################
#
# main project

set(cam_SRC
  ${phd_src_dir}/src/camera/cam_indi.cpp
  ${phd_src_dir}/src/camera/cam_indi.h
  ${phd_src_dir}/src/camera/camera.cpp
  ${phd_src_dir}/src/camera/camera.h
  ${phd_src_dir}/src/camera/cameras.h
)

source_group(Cameras FILES ${cam_SRC})

set(scopes_SRC
  ${phd_src_dir}/src/telescope/mount.cpp
  ${phd_src_dir}/src/telescope/mount.h
  ${phd_src_dir}/src/telescope/scope.cpp
  ${phd_src_dir}/src/telescope/scope.h
  ${phd_src_dir}/src/telescope/scope_manual_pointing.cpp
  ${phd_src_dir}/src/telescope/scope_manual_pointing.h
  ${phd_src_dir}/src/telescope/scope_onboard_st4.cpp
  ${phd_src_dir}/src/telescope/scope_onboard_st4.h
  ${phd_src_dir}/src/telescope/scope_oncamera.cpp
  ${phd_src_dir}/src/telescope/scope_oncamera.h
  ${phd_src_dir}/src/telescope/scope_onstepguider.cpp
  ${phd_src_dir}/src/telescope/scope_onstepguider.h
  ${phd_src_dir}/src/telescope/scope_indi.h
  ${phd_src_dir}/src/telescope/scope_indi.cpp
  ${phd_src_dir}/src/telescope/scopes.h

  ${phd_src_dir}/src/ao/stepguider_sxao.cpp
  ${phd_src_dir}/src/ao/stepguider_sxao.h
  ${phd_src_dir}/src/ao/stepguider_sxao_indi.cpp
  ${phd_src_dir}/src/ao/stepguider_sxao_indi.h
  ${phd_src_dir}/src/ao/stepguider_sbigao_indi.cpp
  ${phd_src_dir}/src/ao/stepguider_sbigao_indi.h
  ${phd_src_dir}/src/ao/stepguider.cpp
  ${phd_src_dir}/src/ao/stepguider.h
  ${phd_src_dir}/src/ao/stepguiders.h

)
source_group(Scopes FILES ${scopes_SRC})

set(guiding_SRC
  ${phd_src_dir}/src/backlash_comp.cpp
  ${phd_src_dir}/src/backlash_comp.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_hysteresis.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_hysteresis.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_gaussian_process.cpp # MPI.IS PEC Guider: requires link to the GP target (contrib)
  ${phd_src_dir}/src/algorithm/guide_algorithm_gaussian_process.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_identity.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_identity.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_lowpass.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_lowpass.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_lowpass2.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_lowpass2.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_resistswitch.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_resistswitch.h
  ${phd_src_dir}/src/algorithm/guide_algorithm_zfilter.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm_zfilter.h
  ${phd_src_dir}/src/algorithm/guide_algorithm.cpp
  ${phd_src_dir}/src/algorithm/guide_algorithm.h
  ${phd_src_dir}/src/algorithm/guide_algorithms.h
  ${phd_src_dir}/src/algorithm/guider_multistar.cpp
  ${phd_src_dir}/src/algorithm/guider_multistar.h
  ${phd_src_dir}/src/guider.cpp
  ${phd_src_dir}/src/guider.h
  ${phd_src_dir}/src/guiders.h
  ${phd_src_dir}/src/zfilterfactory.cpp
  ${phd_src_dir}/src/zfilterfactory.h
)
source_group(Guiding FILES ${guiding_SRC})

set(gui_src
  ${phd_src_dir}/src/gui/about_dialog.cpp
  ${phd_src_dir}/src/gui/about_dialog.h

  ${phd_src_dir}/src/gui/advanced_dialog.cpp
  ${phd_src_dir}/src/gui/advanced_dialog.h

  ${phd_src_dir}/src/gui/aui_controls.cpp
  ${phd_src_dir}/src/gui/aui_controls.h

  ${phd_src_dir}/src/gui/calreview_dialog.cpp
  ${phd_src_dir}/src/gui/calreview_dialog.h

  ${phd_src_dir}/src/gui/calstep_dialog.cpp
  ${phd_src_dir}/src/gui/calstep_dialog.h

  ${phd_src_dir}/src/gui/configdialog.cpp
  ${phd_src_dir}/src/gui/configdialog.h

  ${phd_src_dir}/src/gui/confirm_dialog.cpp
  ${phd_src_dir}/src/gui/confirm_dialog.h

  ${phd_src_dir}/src/gui/darks_dialog.cpp
  ${phd_src_dir}/src/gui/darks_dialog.h

  ${phd_src_dir}/src/gui/indi_gui.cpp
  ${phd_src_dir}/src/gui/indi_gui.h

  ${phd_src_dir}/src/gui/gear_dialog.cpp
  ${phd_src_dir}/src/gui/gear_dialog.h

  ${phd_src_dir}/src/gui/manualcal_dialog.cpp
  ${phd_src_dir}/src/gui/manualcal_dialog.h

  ${phd_src_dir}/src/gui/myframe.cpp
  ${phd_src_dir}/src/gui/myframe.h
  ${phd_src_dir}/src/gui/myframe_events.cpp

  ${phd_src_dir}/src/gui/profile_wizard.h
  ${phd_src_dir}/src/gui/profile_wizard.cpp

  ${phd_src_dir}/src/gui/statswindow.cpp
  ${phd_src_dir}/src/gui/statswindow.h

  ${phd_src_dir}/src/gui/config_indi.cpp
  ${phd_src_dir}/src/gui/config_indi.h

  ${phd_src_dir}/src/gui/comet_tool.cpp
  ${phd_src_dir}/src/gui/comet_tool.h

  ${phd_src_dir}/src/gui/drift_tool.cpp
  ${phd_src_dir}/src/gui/drift_tool.h

  ${phd_src_dir}/src/gui/Refine_DefMap.cpp
  ${phd_src_dir}/src/gui/Refine_DefMap.h

  ${phd_src_dir}/src/gui/graph-stepguider.cpp
  ${phd_src_dir}/src/gui/graph-stepguider.h

  ${phd_src_dir}/src/gui/graph.cpp
  ${phd_src_dir}/src/gui/graph.h

  ${phd_src_dir}/src/gui/guiding_assistant.cpp
  ${phd_src_dir}/src/gui/guiding_assistant.h

  ${phd_src_dir}/src/gui/calibration_assistant.cpp
  ${phd_src_dir}/src/gui/calibration_assistant.h

  ${phd_src_dir}/src/gui/optionsbutton.cpp
  ${phd_src_dir}/src/gui/optionsbutton.h

  ${phd_src_dir}/src/gui/pierflip_tool.cpp
  ${phd_src_dir}/src/gui/pierflip_tool.h

  ${phd_src_dir}/src/gui/polardrift_tool.h
  ${phd_src_dir}/src/gui/polardrift_toolwin.h
  ${phd_src_dir}/src/gui/polardrift_toolwin.cpp

  ${phd_src_dir}/src/gui/starcross_test.cpp
  ${phd_src_dir}/src/gui/starcross_test.h

  ${phd_src_dir}/src/gui/staticpa_tool.h
  ${phd_src_dir}/src/gui/staticpa_toolwin.h
  ${phd_src_dir}/src/gui/staticpa_toolwin.cpp

  ${phd_src_dir}/src/gui/star_profile.cpp
  ${phd_src_dir}/src/gui/star_profile.h

  ${phd_src_dir}/src/gui/testguide.cpp
  ${phd_src_dir}/src/gui/testguide.h

  ${phd_src_dir}/src/gui/wxled.cpp
  ${phd_src_dir}/src/gui/wxled.h

  ${phd_src_dir}/src/gui/camcal_import_dialog.cpp
  ${phd_src_dir}/src/gui/camcal_import_dialog.h

  ${phd_src_dir}/src/gui/messagebox_proxy.cpp
  ${phd_src_dir}/src/gui/messagebox_proxy.h

  ${phd_src_dir}/src/gui/eegg.cpp

  ${phd_src_dir}/src/gui/nudge_lock.cpp
  ${phd_src_dir}/src/gui/nudge_lock.h

  ${phd_src_dir}/src/gui/target.cpp
  ${phd_src_dir}/src/gui/target.h

  ${phd_src_dir}/src/gui/server_dialog_base.cpp
  ${phd_src_dir}/src/gui/server_dialog_base.h
  ${phd_src_dir}/src/gui/server_dialog.cpp
  ${phd_src_dir}/src/gui/server_dialog.h

  ${phd_src_dir}/src/gui/solver_dialog_base.cpp
  ${phd_src_dir}/src/gui/solver_dialog_base.h
  ${phd_src_dir}/src/gui/solver_dialog.cpp
  ${phd_src_dir}/src/gui/solver_dialog.h

  ${phd_src_dir}/src/gui/runinbg.cpp
  ${phd_src_dir}/src/gui/runinbg.h
)
source_group(Gui FILES ${gui_src})

# it would ease if files are organised in an alphabetical order
set(lightguider_SRC

  ${phd_src_dir}/src/circbuf.h

  ${phd_src_dir}/src/server/event_server.cpp
  ${phd_src_dir}/src/server/event_server.h
  ${phd_src_dir}/src/server/event_server_ws.cpp
  ${phd_src_dir}/src/server/event_server_ws.h
  ${phd_src_dir}/src/server/socket_server.cpp
  ${phd_src_dir}/src/server/socket_server.h
  ${phd_src_dir}/src/server/json_parser.cpp
  ${phd_src_dir}/src/server/json_parser.h

  ${phd_src_dir}/src/image/fitsiowrap.cpp
  ${phd_src_dir}/src/image/fitsiowrap.h
  ${phd_src_dir}/src/image/image_math.cpp
  ${phd_src_dir}/src/image/image_math.h
  ${phd_src_dir}/src/image/imagelogger.cpp
  ${phd_src_dir}/src/image/imagelogger.h
  ${phd_src_dir}/src/image/usImage.cpp
  ${phd_src_dir}/src/image/usImage.h
  
  ${phd_src_dir}/src/ao/gear_simulator.h
  ${phd_src_dir}/src/ao/gear_simulator.cpp

  

  ${phd_src_dir}/src/logger/debuglog.cpp
  ${phd_src_dir}/src/logger/debuglog.h
  ${phd_src_dir}/src/logger/guidinglog.cpp
  ${phd_src_dir}/src/logger/guidinglog.h
  ${phd_src_dir}/src/logger/logger.cpp
  ${phd_src_dir}/src/logger/logger.h
  ${phd_src_dir}/src/logger/log_uploader.cpp
  ${phd_src_dir}/src/logger/log_uploader.h

  ${phd_src_dir}/src/guiding_stats.cpp
  ${phd_src_dir}/src/guiding_stats.h
  
  ${phd_src_dir}/src/onboard_st4.cpp
  ${phd_src_dir}/src/onboard_st4.h
  
  ${phd_src_dir}/src/lightguider.cpp
  ${phd_src_dir}/src/lightguider.h
  ${phd_src_dir}/src/phdconfig.cpp
  ${phd_src_dir}/src/phdconfig.h
  ${phd_src_dir}/src/phdcontrol.cpp
  ${phd_src_dir}/src/phdcontrol.h
  ${phd_src_dir}/src/phdindiclient.cpp
  ${phd_src_dir}/src/phdindiclient.h
  ${phd_src_dir}/src/phdupdate.cpp
  ${phd_src_dir}/src/phdupdate.h
  
  ${phd_src_dir}/src/point.h
  

  # rotators
  ${phd_src_dir}/src/rotator/rotators.h
  ${phd_src_dir}/src/rotator/rotator.cpp
  ${phd_src_dir}/src/rotator/rotator.h

  ${phd_src_dir}/src/serialport.cpp
  ${phd_src_dir}/src/serialport.h
  ${phd_src_dir}/src/serialport_loopback.cpp
  ${phd_src_dir}/src/serialport_loopback.h
  ${phd_src_dir}/src/serialport_posix.cpp
  ${phd_src_dir}/src/serialport_posix.h
  ${phd_src_dir}/src/serialports.h
  ${phd_src_dir}/src/sha1.cpp
  ${phd_src_dir}/src/sha1.h
  
  ${phd_src_dir}/src/star.cpp
  ${phd_src_dir}/src/star.h
  
  ${phd_src_dir}/src/worker_thread.cpp
  ${phd_src_dir}/src/worker_thread.h
  
)

source_group(src FILES ${lightguider_SRC})

# Add the messages.mo directly in the project such that this triggers a new build
# and all the post_build commands are honored properly
foreach(current_locale IN LISTS all_locales)
  # installation of the document and translation files
  get_zip_file(current_zip LOCALE ${current_locale})
  get_translation_files(current_translations LOCALE ${current_locale})

  foreach(current_file IN LISTS current_translations)
    get_filename_component(message_file_basename ${current_file} NAME)
    get_filename_component(message_file_ext ${current_file} EXT)
    if(("${message_file_ext}" STREQUAL ".mo") OR ("${message_file_ext}" STREQUAL ".zip"))
      # message(STATUS "Harhar ${message_file_ext} / ${current_file}")
      set(lightguider_SRC ${lightguider_SRC} ${current_file})
    endif()
  endforeach()

endforeach() # for all locales


MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "\$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")

    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF()
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

if(UNIX)

  # should be linux there
  add_executable(
   lightguider
   ${scopes_SRC}
   ${cam_SRC}
   ${guiding_SRC}
   ${gui_src}
   ${lightguider_SRC}
   )
  target_link_libraries(lightguider X11)

  find_package(spdlog REQUIRED)
  target_link_libraries(lightguider spdlog::spdlog_header_only)

  set_target_properties(
    lightguider
    PROPERTIES
      OUTPUT_NAME lightguider
    )

else()
  message(FATAL_ERROR "Unsupported platform")
endif()


# properties of the project common to all platforms
target_compile_definitions(lightguider PRIVATE "${wxWidgets_DEFINITIONS}" "HAVE_TYPE_TRAITS")
target_compile_options(lightguider PRIVATE "${wxWidgets_CXX_FLAGS};")
if(APPLE AND NOT APPLE32)
  # suppress warnings about "overrides a member function but is not marked 'override'" with wxDECLARE_EVENT_TABLE
  target_compile_options(lightguider PRIVATE "-Wno-inconsistent-missing-override")
endif()
target_include_directories(lightguider PRIVATE ${wxWidgets_INCLUDE_DIRS})

if (PHD_LINK_EXTERNAL_RELEASE)
  target_link_libraries(lightguider optimized ${PHD_LINK_EXTERNAL_RELEASE})
endif()
if (PHD_LINK_EXTERNAL_DEBUG)
  target_link_libraries(lightguider debug ${PHD_LINK_EXTERNAL_DEBUG})
endif()
target_link_libraries(lightguider
                      MPIIS_GP GPGuider # GP Guider
                      ${PHD_LINK_EXTERNAL})



################################################################
#
# documentation + translation
#
################################################################

# adding dependencies on the generation of the htmls/translations
# Adding the generated documentation artifacts to the project
# if(NOT TARGET ${default_locale}_html)
 # message(FATAL_ERROR "The HTML target for the default locale cannot be found")
#endif()

#if(NOT TARGET ${target_string_extraction_from_sources})
#  message(FATAL_ERROR "The translation target for the default locale cannot be found")
#endif()

foreach(locale IN LISTS all_locales)
  get_zip_file(current_zip LOCALE ${locale})
  if(NOT "${current_zip}" STREQUAL "")
    add_dependencies(lightguider ${locale}_html)
  endif()

  get_translation_files(current_messages LOCALE ${locale})
  if(NOT "${current_messages}" STREQUAL "")
    if("${locale}" STREQUAL "${default_locale}")
      #set(dependent_target ${target_string_extraction_from_sources})
    else()
      #set(dependent_target ${locale}_translation)
      add_dependencies(lightguider ${locale}_translation)
    endif()
  else()
    if(NOT "${locale}" STREQUAL "${default_locale}")
      message(WARNING "The translation for locale ${locale} is not found")
    endif()
  endif()

endforeach()

## Linux bundling of documentation + translation
if(UNIX AND NOT APPLE)

  foreach(current_locale IN LISTS all_locales)
    if("${current_locale}" STREQUAL "${default_locale}")
      set(destination share/lightguider/)
      set(install_help_options DESTINATION ${CMAKE_INSTALL_PREFIX}/share/lightguider/)
    else()
      set(destination share/lightguider/locale/${current_locale}/)
      set(install_help_options DESTINATION ${CMAKE_INSTALL_PREFIX}/share/lightguider/locale/${current_locale}/ OPTIONAL)
    endif()

    get_zip_file(current_zip LOCALE ${current_locale})
    get_translation_files(current_translations LOCALE ${current_locale})

    foreach(current_file IN LISTS current_translations current_zip)
      get_filename_component(message_file_name ${current_file} NAME)
      get_filename_component(message_file_ext ${current_file} EXT)
      if("${message_file_name}" STREQUAL "messages.mo" OR "${message_file_ext}" STREQUAL ".zip")
        add_custom_command(
          TARGET lightguider
          POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E make_directory $<TARGET_FILE_DIR:lightguider>/${destination}
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${current_file} $<TARGET_FILE_DIR:lightguider>/${destination}${message_file_name}
        )
      endif()

      if("${message_file_name}" STREQUAL "messages.mo")
          install(FILES       ${current_file}
                  DESTINATION  ${CMAKE_INSTALL_PREFIX}/share/locale/${current_locale}/LC_MESSAGES/
                  RENAME "lightguider.mo"
                  OPTIONAL)
      endif()

      if("${message_file_ext}" STREQUAL ".zip")
          install(FILES ${current_file}
                  ${install_help_options})
      endif()
    endforeach()
  endforeach()
endif()


################################################################
#
# Installation and packaging
#
include(cmake_modules/LightGuiderPackaging.cmake)
