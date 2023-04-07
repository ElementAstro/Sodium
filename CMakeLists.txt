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
set(LGuider_PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(lightguider_src_dir ${LGuider_PROJECT_ROOT_DIR})

add_custom_target(CmakeAdditionalFiles
  SOURCES
  ${lightguider_src_dir}/cmake_modules/FindASCOM_INTERFACE.cmake
  ${lightguider_src_dir}/cmake_modules/FindCFITSIO.cmake
  ${lightguider_src_dir}/cmake_modules/FindINDI.cmake
  ${lightguider_src_dir}/cmake_modules/FindNova.cmake
  ${lightguider_src_dir}/cmake_modules/LightGuiderBuildDoc.cmake
  ${lightguider_src_dir}/cmake_modules/LightGuiderGenerateDocScript.cmake
  ${lightguider_src_dir}/cmake_modules/LightGuiderRemovegettextwarning.cmake
  ${lightguider_src_dir}/cmake_modules/compiler_options.cmake
  ${lightguider_src_dir}/cmake_modules/LightGuiderPackaging.cmake)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

#message(${LGuider_PROJECT_ROOT_DIR})
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
set(BUILD_SHARED_LIBS On)
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

#################################################################################
#
# generates the documentation targets. Part of those targets define the
# zip files LGuider2GuideHelp.zip. In order to include them in the main build
# we need to declare them here. Those targets should not depend on any source file
# but may require thirdparty (gettext etc).
generate_doc_targets(
  DEFAULT_LOCALE default_locale
  GENERATED_LOCALES all_locales)

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
# include_directories(${lightguider_src_dir}/src/cameras/)
include_directories(${lightguider_src_dir}/src/)


#################################################################################
#
# main project

set(cam_SRC
  ${lightguider_src_dir}/src/camera/cam_ascom.cpp
  ${lightguider_src_dir}/src/camera/cam_ascom.h
  ${lightguider_src_dir}/src/camera/cam_indi.cpp
  ${lightguider_src_dir}/src/camera/cam_indi.h
  ${lightguider_src_dir}/src/camera/camera.cpp
  ${lightguider_src_dir}/src/camera/camera.h
  ${lightguider_src_dir}/src/camera/cameras.h
)

source_group(Cameras FILES ${cam_SRC})

set(server_SRC
  ${lightguider_src_dir}/src/server/event_server.cpp
  ${lightguider_src_dir}/src/server/event_server.h
  ${lightguider_src_dir}/src/server/socket_server.cpp
  ${lightguider_src_dir}/src/server/socket_server.h
  ${lightguider_src_dir}/src/server/json_parser.cpp
  ${lightguider_src_dir}/src/server/json_parser.h
)

source_group(Server FILES ${server_SRC})

set(scopes_SRC
  ${lightguider_src_dir}/src/telescope/mount.cpp
  ${lightguider_src_dir}/src/telescope/mount.h
  ${lightguider_src_dir}/src/telescope/scope.cpp
  ${lightguider_src_dir}/src/telescope/scope.h
  ${lightguider_src_dir}/src/telescope/scope_manual_pointing.cpp
  ${lightguider_src_dir}/src/telescope/scope_manual_pointing.h
  ${lightguider_src_dir}/src/telescope/scope_onboard_st4.cpp
  ${lightguider_src_dir}/src/telescope/scope_onboard_st4.h
  ${lightguider_src_dir}/src/telescope/scope_oncamera.cpp
  ${lightguider_src_dir}/src/telescope/scope_oncamera.h
  ${lightguider_src_dir}/src/telescope/scope_onstepguider.cpp
  ${lightguider_src_dir}/src/telescope/scope_onstepguider.h
  ${lightguider_src_dir}/src/telescope/scope_indi.h
  ${lightguider_src_dir}/src/telescope/scope_indi.cpp
  ${lightguider_src_dir}/src/telescope/scopes.h
)

source_group(Scopes FILES ${scopes_SRC})

set(stepguiders_SRC
  ${lightguider_src_dir}/src/ao/stepguider_sxao.cpp
  ${lightguider_src_dir}/src/ao/stepguider_sxao.h
  ${lightguider_src_dir}/src/ao/stepguider_sxao_indi.cpp
  ${lightguider_src_dir}/src/ao/stepguider_sxao_indi.h
  ${lightguider_src_dir}/src/ao/stepguider_sbigao_indi.cpp
  ${lightguider_src_dir}/src/ao/stepguider_sbigao_indi.h
  ${lightguider_src_dir}/src/ao/stepguider.cpp
  ${lightguider_src_dir}/src/ao/stepguider.h
  ${lightguider_src_dir}/src/ao/stepguiders.h
)

source_group(StepGuider FILES ${stepguiders_SRC})

set(rotators_SRC
  # rotators
  ${lightguider_src_dir}/src/rotator/rotators.h
  ${lightguider_src_dir}/src/rotator/rotator.cpp
  ${lightguider_src_dir}/src/rotator/rotator.h
  ${lightguider_src_dir}/src/rotator/rotator_ascom.cpp
  ${lightguider_src_dir}/src/rotator/rotator_ascom.h
)

source_group(Rotator FILES ${rotators_SRC})

set(guiding_SRC
  ${lightguider_src_dir}/src/algorithm/backlash_comp.cpp
  ${lightguider_src_dir}/src/algorithm/backlash_comp.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_hysteresis.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_hysteresis.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_gaussian_process.cpp # MPI.IS PEC Guider: requires link to the GP target (contrib)
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_gaussian_process.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_identity.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_identity.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_lowpass.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_lowpass.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_lowpass2.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_lowpass2.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_resistswitch.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_resistswitch.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_zfilter.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm_zfilter.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithm.cpp
  ${lightguider_src_dir}/src/algorithm/guide_algorithm.h
  ${lightguider_src_dir}/src/algorithm/guide_algorithms.h
  ${lightguider_src_dir}/src/algorithm/guider_multistar.cpp
  ${lightguider_src_dir}/src/algorithm/guider_multistar.h
  ${lightguider_src_dir}/src/algorithm/zfilterfactory.cpp
  ${lightguider_src_dir}/src/algorithm/zfilterfactory.h

  ${lightguider_src_dir}/src/guiding/guider.cpp
  ${lightguider_src_dir}/src/guiding/guider.h
  ${lightguider_src_dir}/src/guiding/guiders.h
)
source_group(Guiding FILES ${guiding_SRC})

set(gui_src

  ${lightguider_src_dir}/src/gui/LightGuiderDeviceDialogBase.cpp
  ${lightguider_src_dir}/src/gui/LightGuiderDeviceDialogBase.h

  ${lightguider_src_dir}/src/gui/LightGuiderDeviceDialog.cpp
  ${lightguider_src_dir}/src/gui/LightGuiderDeviceDialog.h

  ${lightguider_src_dir}/src/gui/about_dialog.cpp
  ${lightguider_src_dir}/src/gui/about_dialog.h

  ${lightguider_src_dir}/src/gui/advanced_dialog.cpp
  ${lightguider_src_dir}/src/gui/advanced_dialog.h

  ${lightguider_src_dir}/src/gui/aui_controls.cpp
  ${lightguider_src_dir}/src/gui/aui_controls.h

  ${lightguider_src_dir}/src/gui/calreview_dialog.cpp
  ${lightguider_src_dir}/src/gui/calreview_dialog.h

  ${lightguider_src_dir}/src/gui/calstep_dialog.cpp
  ${lightguider_src_dir}/src/gui/calstep_dialog.h

  ${lightguider_src_dir}/src/gui/configdialog.cpp
  ${lightguider_src_dir}/src/gui/configdialog.h

  ${lightguider_src_dir}/src/gui/confirm_dialog.cpp
  ${lightguider_src_dir}/src/gui/confirm_dialog.h

  ${lightguider_src_dir}/src/gui/darks_dialog.cpp
  ${lightguider_src_dir}/src/gui/darks_dialog.h

  ${lightguider_src_dir}/src/gui/indi_gui.cpp
  ${lightguider_src_dir}/src/gui/indi_gui.h

  ${lightguider_src_dir}/src/gui/gear_dialog.cpp
  ${lightguider_src_dir}/src/gui/gear_dialog.h

  ${lightguider_src_dir}/src/gui/manualcal_dialog.cpp
  ${lightguider_src_dir}/src/gui/manualcal_dialog.h

  ${lightguider_src_dir}/src/gui/myframe.cpp
  ${lightguider_src_dir}/src/gui/myframe.h
  ${lightguider_src_dir}/src/gui/myframe_events.cpp

  ${lightguider_src_dir}/src/gui/profile_wizard.h
  ${lightguider_src_dir}/src/gui/profile_wizard.cpp

  ${lightguider_src_dir}/src/gui/statswindow.cpp
  ${lightguider_src_dir}/src/gui/statswindow.h

  ${lightguider_src_dir}/src/gui/config_indi.cpp
  ${lightguider_src_dir}/src/gui/config_indi.h

  ${lightguider_src_dir}/src/gui/comet_tool.cpp
  ${lightguider_src_dir}/src/gui/comet_tool.h

  ${lightguider_src_dir}/src/gui/drift_tool.cpp
  ${lightguider_src_dir}/src/gui/drift_tool.h

  ${lightguider_src_dir}/src/gui/Refine_DefMap.cpp
  ${lightguider_src_dir}/src/gui/Refine_DefMap.h

  ${lightguider_src_dir}/src/gui/graph-stepguider.cpp
  ${lightguider_src_dir}/src/gui/graph-stepguider.h

  ${lightguider_src_dir}/src/gui/graph.cpp
  ${lightguider_src_dir}/src/gui/graph.h

  ${lightguider_src_dir}/src/gui/guiding_assistant.cpp
  ${lightguider_src_dir}/src/gui/guiding_assistant.h

  ${lightguider_src_dir}/src/gui/calibration_assistant.cpp
  ${lightguider_src_dir}/src/gui/calibration_assistant.h

  ${lightguider_src_dir}/src/gui/optionsbutton.cpp
  ${lightguider_src_dir}/src/gui/optionsbutton.h

  ${lightguider_src_dir}/src/gui/pierflip_tool.cpp
  ${lightguider_src_dir}/src/gui/pierflip_tool.h

  ${lightguider_src_dir}/src/gui/polardrift_tool.h
  ${lightguider_src_dir}/src/gui/polardrift_toolwin.h
  ${lightguider_src_dir}/src/gui/polardrift_toolwin.cpp

  ${lightguider_src_dir}/src/gui/starcross_test.cpp
  ${lightguider_src_dir}/src/gui/starcross_test.h

  ${lightguider_src_dir}/src/gui/staticpa_tool.h
  ${lightguider_src_dir}/src/gui/staticpa_toolwin.h
  ${lightguider_src_dir}/src/gui/staticpa_toolwin.cpp

  ${lightguider_src_dir}/src/gui/star_profile.cpp
  ${lightguider_src_dir}/src/gui/star_profile.h

  ${lightguider_src_dir}/src/gui/testguide.cpp
  ${lightguider_src_dir}/src/gui/testguide.h

  ${lightguider_src_dir}/src/gui/wxled.cpp
  ${lightguider_src_dir}/src/gui/wxled.h

  ${lightguider_src_dir}/src/gui/camcal_import_dialog.cpp
  ${lightguider_src_dir}/src/gui/camcal_import_dialog.h

  ${lightguider_src_dir}/src/gui/messagebox_proxy.cpp
  ${lightguider_src_dir}/src/gui/messagebox_proxy.h

  ${lightguider_src_dir}/src/gui/eegg.cpp

  ${lightguider_src_dir}/src/gui/nudge_lock.cpp
  ${lightguider_src_dir}/src/gui/nudge_lock.h

  ${lightguider_src_dir}/src/gui/target.cpp
  ${lightguider_src_dir}/src/gui/target.h

  ${lightguider_src_dir}/src/gui/server_dialog_base.cpp
  ${lightguider_src_dir}/src/gui/server_dialog_base.h
  ${lightguider_src_dir}/src/gui/server_dialog.cpp
  ${lightguider_src_dir}/src/gui/server_dialog.h

  ${lightguider_src_dir}/src/gui/solver_dialog_base.cpp
  ${lightguider_src_dir}/src/gui/solver_dialog_base.h
  ${lightguider_src_dir}/src/gui/solver_dialog.cpp
  ${lightguider_src_dir}/src/gui/solver_dialog.h

  ${lightguider_src_dir}/src/gui/runinbg.cpp
  ${lightguider_src_dir}/src/gui/runinbg.h
)
source_group(Gui FILES ${gui_src})

# it would ease if files are organised in an alphabetical order
set(lightguider_SRC

  ${lightguider_src_dir}/src/guiding/circbuf.h

  ${lightguider_src_dir}/src/image/fitsiowrap.cpp
  ${lightguider_src_dir}/src/image/fitsiowrap.h
  ${lightguider_src_dir}/src/image/image_math.cpp
  ${lightguider_src_dir}/src/image/image_math.h
  ${lightguider_src_dir}/src/image/imagelogger.cpp
  ${lightguider_src_dir}/src/image/imagelogger.h
  ${lightguider_src_dir}/src/image/usImage.cpp
  ${lightguider_src_dir}/src/image/usImage.h
  
  ${lightguider_src_dir}/src/ao/gear_simulator.h
  ${lightguider_src_dir}/src/ao/gear_simulator.cpp

  

  ${lightguider_src_dir}/src/logger/debuglog.cpp
  ${lightguider_src_dir}/src/logger/debuglog.h
  ${lightguider_src_dir}/src/logger/guidinglog.cpp
  ${lightguider_src_dir}/src/logger/guidinglog.h
  ${lightguider_src_dir}/src/logger/logger.cpp
  ${lightguider_src_dir}/src/logger/logger.h
  ${lightguider_src_dir}/src/logger/log_uploader.cpp
  ${lightguider_src_dir}/src/logger/log_uploader.h

  ${lightguider_src_dir}/src/guiding/guiding_stats.cpp
  ${lightguider_src_dir}/src/guiding/guiding_stats.h
  
  ${lightguider_src_dir}/src/device/onboard_st4.cpp
  ${lightguider_src_dir}/src/device/onboard_st4.h
  
  ${lightguider_src_dir}/src/lightguider.cpp
  ${lightguider_src_dir}/src/lightguider.h

  ${lightguider_src_dir}/src/lightconfig.cpp
  ${lightguider_src_dir}/src/lightconfig.h
  ${lightguider_src_dir}/src/lightcontrol.cpp
  ${lightguider_src_dir}/src/lightcontrol.h
  ${lightguider_src_dir}/src/indi/indiclient.cpp
  ${lightguider_src_dir}/src/indi/indiclient.h

  ${lightguider_src_dir}/src/network/lightupdate.cpp
  ${lightguider_src_dir}/src/network/lightupdate.h
  ${lightguider_src_dir}/src/network/sha1.cpp
  ${lightguider_src_dir}/src/network/sha1.h
  
  ${lightguider_src_dir}/src/guiding/point.h

  ${lightguider_src_dir}/src/device/serialport.cpp
  ${lightguider_src_dir}/src/device/serialport.h
  ${lightguider_src_dir}/src/device/serialport_loopback.cpp
  ${lightguider_src_dir}/src/device/serialport_loopback.h
  ${lightguider_src_dir}/src/device/serialport_mac.cpp
  ${lightguider_src_dir}/src/device/serialport_mac.h
  ${lightguider_src_dir}/src/device/serialport_win32.cpp
  ${lightguider_src_dir}/src/device/serialport_win32.h
  ${lightguider_src_dir}/src/device/serialport_posix.cpp
  ${lightguider_src_dir}/src/device/serialport_posix.h
  ${lightguider_src_dir}/src/device/serialports.h
  
  ${lightguider_src_dir}/src/guiding/star.cpp
  ${lightguider_src_dir}/src/guiding/star.h
  
  ${lightguider_src_dir}/src/worker_thread.cpp
  ${lightguider_src_dir}/src/worker_thread.h
  
)


if (WIN32)
  set(lightguider_SRC
      ${lightguider_SRC}
      # windows-only sources
      ${lightguider_src_dir}/src/win32/parallelports.h
      ${lightguider_src_dir}/src/win32/parallelport.h
      ${lightguider_src_dir}/src/win32/parallelport.cpp

      ${lightguider_src_dir}/src/win32/parallelport_win32.h
      ${lightguider_src_dir}/src/win32/parallelport_win32.cpp

      ${lightguider_src_dir}/src/win32/comdispatch.h
      ${lightguider_src_dir}/src/win32/comdispatch.cpp
     )
endif()

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
   ${server_SRC}
   ${modloader_SRC}
   ${scopes_SRC}
   ${cam_SRC}
   ${stepguiders_SRC}
   ${rotators_SRC}
   ${guiding_SRC}
   ${gui_src}
   ${lightguider_SRC}
   )

  target_precompile_headers(lightguider PUBLIC ${lightguider_src_dir}/src/lightguider.h)

  # 链接 Python 库
  target_link_libraries(lightguider ${Python_LIBRARIES})


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

if (LGuider_LINK_EXTERNAL_RELEASE)
  target_link_libraries(lightguider optimized ${LGuider_LINK_EXTERNAL_RELEASE})
endif()
if (LGuider_LINK_EXTERNAL_DEBUG)
  target_link_libraries(lightguider debug ${LGuider_LINK_EXTERNAL_DEBUG})
endif()
target_link_libraries(lightguider
                      MPIIS_GP GPGuider # GP Guider
                      ${LGuider_LINK_EXTERNAL})



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


## Windows bundling of documentation + translation
if(WIN32)
  # here we generate the installation commands and the commands that will make the
  # build tree look like the install tree, such that users can run the application directly
  # from Visual Studio.
  foreach(current_locale IN LISTS all_locales)

    if("${current_locale}" STREQUAL "${default_locale}")
      # because of wXWidget (wxStandardPaths::IgnoreAppBuildSubDirs)
      # the paths "Debug", "Release" are discarded, and the parent path
      # is considered instead as the application directory
      # This is only for running the app from a Development environment
      # (TODO: use only for multiconfig cmake projects)
      set(build_tree_target_folder $<TARGET_FILE_DIR:lightguider>/)
      set(install_options DESTINATION ${CMAKE_INSTALL_PREFIX}/)
    else()
      set(build_tree_target_folder $<TARGET_FILE_DIR:lightguider>/locale/${current_locale})
      set(install_options DESTINATION ${CMAKE_INSTALL_PREFIX}/locale/${current_locale}/ OPTIONAL)
    endif()

    # installation of the document and translation files
    get_zip_file(current_zip LOCALE ${current_locale})
    get_translation_files(current_translations LOCALE ${current_locale})

    foreach(current_file IN LISTS current_translations current_zip)
      get_filename_component(message_file_basename ${current_file} NAME)
      get_filename_component(message_file_ext ${current_file} EXT)
      if("${message_file_ext}" STREQUAL ".mo" OR "${message_file_ext}" STREQUAL ".zip")
        add_custom_command(
          TARGET lightguider
          POST_BUILD
             COMMAND ${CMAKE_COMMAND} -E make_directory "${build_tree_target_folder}"
             COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${current_file} "${build_tree_target_folder}/${message_file_basename}"
        )
        install(FILES       ${current_file}
                DESTINATION  ${CMAKE_INSTALL_PREFIX}/locale/${current_locale}
                OPTIONAL)
      endif()
    endforeach()

  endforeach() # for all locales
endif()

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
