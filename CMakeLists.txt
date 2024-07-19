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
set(SODIUM_PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(sodium_src_dir ${SODIUM_PROJECT_ROOT_DIR}/src)

add_custom_target(CmakeAdditionalFiles
  SOURCES
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/FindASCOM_INTERFACE.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/FindCFITSIO.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/FindINDI.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/FindNova.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/SodiumBuildDoc.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/SodiumGenerateDocScript.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/SodiumRemovegettextwarning.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/compiler_options.cmake
  ${SODIUM_PROJECT_ROOT_DIR}/cmake_modules/SodiumPackaging.cmake)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

#message(${SODIUM_PROJECT_ROOT_DIR})
# Additional files in the workspace, To improve maintainability

# compiler options
include(cmake_modules/compiler_options.cmake)

# declare useful functions about documentation
include(cmake_modules/SodiumBuildDoc.cmake)

# retrieves the version from the source file
get_sodium_version()
message(STATUS "Light Guider version '${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}' (extracted from source)")

#################################################################################
#
# general defines
if(UNIX AND NOT APPLE)
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "sodium install path" FORCE)
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
# zip files SODIUM2GuideHelp.zip. In order to include them in the main build
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
# WxWidgets inclusion narrowed to sodium project only
# include_directories(${wxWidgets_INCLUDE_DIRS})
# include_directories(${sodium_src_dir}/cameras/)
include_directories(${sodium_src_dir})
include_directories(${sodium_src_dir}/algorithm)
include_directories(${sodium_src_dir}/ao)
include_directories(${sodium_src_dir}/camera)
include_directories(${sodium_src_dir}/config)
include_directories(${sodium_src_dir}/device)
include_directories(${sodium_src_dir}/gui)
include_directories(${sodium_src_dir}/guiding)
include_directories(${sodium_src_dir}/icons)
include_directories(${sodium_src_dir}/image)
include_directories(${sodium_src_dir}/logger)
include_directories(${sodium_src_dir}/network)
include_directories(${sodium_src_dir}/rotator)
include_directories(${sodium_src_dir}/server)
include_directories(${sodium_src_dir}/telescope)
include_directories(${sodium_src_dir}/win32)

#################################################################################
#
# main project

set(cam_SRC
  ${sodium_src_dir}/camera/cam_ascom.cpp
  ${sodium_src_dir}/camera/cam_ascom.hpp
  ${sodium_src_dir}/camera/cam_indi.cpp
  ${sodium_src_dir}/camera/cam_indi.hpp
  ${sodium_src_dir}/camera/camera.cpp
  ${sodium_src_dir}/camera/camera.hpp
  ${sodium_src_dir}/camera/cameras.hpp
)

source_group(Cameras FILES ${cam_SRC})

set(server_SRC
  ${sodium_src_dir}/server/event_server.cpp
  ${sodium_src_dir}/server/event_server.hpp
  ${sodium_src_dir}/server/socket_server.cpp
  ${sodium_src_dir}/server/socket_server.hpp
  ${sodium_src_dir}/server/json_parser.cpp
  ${sodium_src_dir}/server/json_parser.hpp
  ${sodium_src_dir}/server/worker_thread.cpp
  ${sodium_src_dir}/server/worker_thread.hpp
)

source_group(Server FILES ${server_SRC})

set(scopes_SRC
  ${sodium_src_dir}/telescope/mount.cpp
  ${sodium_src_dir}/telescope/mount.hpp
  ${sodium_src_dir}/telescope/scope.cpp
  ${sodium_src_dir}/telescope/scope.hpp
  ${sodium_src_dir}/telescope/scope_manual_pointing.cpp
  ${sodium_src_dir}/telescope/scope_manual_pointing.hpp
  ${sodium_src_dir}/telescope/scope_onboard_st4.cpp
  ${sodium_src_dir}/telescope/scope_onboard_st4.hpp
  ${sodium_src_dir}/telescope/scope_oncamera.cpp
  ${sodium_src_dir}/telescope/scope_oncamera.hpp
  ${sodium_src_dir}/telescope/scope_onstepguider.cpp
  ${sodium_src_dir}/telescope/scope_onstepguider.hpp
  ${sodium_src_dir}/telescope/scope_indi.hpp
  ${sodium_src_dir}/telescope/scope_indi.cpp
  ${sodium_src_dir}/telescope/scopes.hpp
)

source_group(Scopes FILES ${scopes_SRC})

set(stepguiders_SRC
  ${sodium_src_dir}/ao/stepguider_sxao_indi.cpp
  ${sodium_src_dir}/ao/stepguider_sxao_indi.hpp
  ${sodium_src_dir}/ao/stepguider_sbigao_indi.cpp
  ${sodium_src_dir}/ao/stepguider_sbigao_indi.hpp
  ${sodium_src_dir}/ao/stepguider.cpp
  ${sodium_src_dir}/ao/stepguider.hpp
  ${sodium_src_dir}/ao/stepguiders.hpp
)

source_group(StepGuider FILES ${stepguiders_SRC})

set(rotators_SRC
  # rotators
  ${sodium_src_dir}/rotator/rotators.hpp
  ${sodium_src_dir}/rotator/rotator.cpp
  ${sodium_src_dir}/rotator/rotator.hpp
  ${sodium_src_dir}/rotator/rotator_ascom.cpp
  ${sodium_src_dir}/rotator/rotator_ascom.hpp
  ${sodium_src_dir}/rotator/rotator_indi.cpp
  ${sodium_src_dir}/rotator/rotator_indi.hpp
)

source_group(Rotator FILES ${rotators_SRC})

set(guiding_SRC
  ${sodium_src_dir}/algorithm/backlash_comp.cpp
  ${sodium_src_dir}/algorithm/backlash_comp.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_hysteresis.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_hysteresis.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_gaussian_process.cpp # MPI.IS PEC Guider: requires link to the GP target (contrib)
  ${sodium_src_dir}/algorithm/guide_algorithm_gaussian_process.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_identity.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_identity.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_lowpass.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_lowpass.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_lowpass2.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_lowpass2.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_resistswitch.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_resistswitch.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm_zfilter.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm_zfilter.hpp
  ${sodium_src_dir}/algorithm/guide_algorithm.cpp
  ${sodium_src_dir}/algorithm/guide_algorithm.hpp
  ${sodium_src_dir}/algorithm/guide_algorithms.hpp
  ${sodium_src_dir}/algorithm/zfilterfactory.cpp
  ${sodium_src_dir}/algorithm/zfilterfactory.hpp

  ${sodium_src_dir}/guiding/guider.cpp
  ${sodium_src_dir}/guiding/guider.hpp
  ${sodium_src_dir}/guiding/guider_multistar.cpp
  ${sodium_src_dir}/guiding/guider_multistar.hpp
  ${sodium_src_dir}/guiding/guiders.hpp
)
source_group(Guiding FILES ${guiding_SRC})

set(gui_src

  ${sodium_src_dir}/gui/SodiumDeviceDialogBase.cpp
  ${sodium_src_dir}/gui/SodiumDeviceDialogBase.hpp

  ${sodium_src_dir}/gui/SodiumDeviceDialog.cpp
  ${sodium_src_dir}/gui/SodiumDeviceDialog.hpp

  ${sodium_src_dir}/gui/about_dialog.cpp
  ${sodium_src_dir}/gui/about_dialog.hpp

  ${sodium_src_dir}/gui/advanced_dialog.cpp
  ${sodium_src_dir}/gui/advanced_dialog.hpp

  ${sodium_src_dir}/gui/aui_controls.cpp
  ${sodium_src_dir}/gui/aui_controls.hpp

  ${sodium_src_dir}/gui/calreview_dialog.cpp
  ${sodium_src_dir}/gui/calreview_dialog.hpp

  ${sodium_src_dir}/gui/calstep_dialog.cpp
  ${sodium_src_dir}/gui/calstep_dialog.hpp

  ${sodium_src_dir}/gui/configdialog.cpp
  ${sodium_src_dir}/gui/configdialog.hpp

  ${sodium_src_dir}/gui/confirm_dialog.cpp
  ${sodium_src_dir}/gui/confirm_dialog.hpp

  ${sodium_src_dir}/gui/darks_dialog.cpp
  ${sodium_src_dir}/gui/darks_dialog.hpp

  ${sodium_src_dir}/gui/indi_gui.cpp
  ${sodium_src_dir}/gui/indi_gui.hpp

  ${sodium_src_dir}/gui/gear_dialog.cpp
  ${sodium_src_dir}/gui/gear_dialog.hpp

  ${sodium_src_dir}/gui/manualcal_dialog.cpp
  ${sodium_src_dir}/gui/manualcal_dialog.hpp

  ${sodium_src_dir}/gui/myframe.cpp
  ${sodium_src_dir}/gui/myframe.hpp
  ${sodium_src_dir}/gui/myframe_events.cpp

  ${sodium_src_dir}/gui/profile_wizard.hpp
  ${sodium_src_dir}/gui/profile_wizard.cpp

  ${sodium_src_dir}/gui/statswindow.cpp
  ${sodium_src_dir}/gui/statswindow.hpp

  ${sodium_src_dir}/gui/config_indi.cpp
  ${sodium_src_dir}/gui/config_indi.hpp

  ${sodium_src_dir}/gui/comet_tool.cpp
  ${sodium_src_dir}/gui/comet_tool.hpp

  ${sodium_src_dir}/gui/drift_tool.cpp
  ${sodium_src_dir}/gui/drift_tool.hpp

  ${sodium_src_dir}/gui/Refine_DefMap.cpp
  ${sodium_src_dir}/gui/Refine_DefMap.hpp

  ${sodium_src_dir}/gui/graph-stepguider.cpp
  ${sodium_src_dir}/gui/graph-stepguider.hpp

  ${sodium_src_dir}/gui/graph.cpp
  ${sodium_src_dir}/gui/graph.hpp

  ${sodium_src_dir}/gui/guiding_assistant.cpp
  ${sodium_src_dir}/gui/guiding_assistant.hpp

  ${sodium_src_dir}/gui/calibration_assistant.cpp
  ${sodium_src_dir}/gui/calibration_assistant.hpp

  ${sodium_src_dir}/gui/optionsbutton.cpp
  ${sodium_src_dir}/gui/optionsbutton.hpp

  ${sodium_src_dir}/gui/pierflip_tool.cpp
  ${sodium_src_dir}/gui/pierflip_tool.hpp

  ${sodium_src_dir}/gui/polardrift_tool.hpp
  ${sodium_src_dir}/gui/polardrift_toolwin.hpp
  ${sodium_src_dir}/gui/polardrift_toolwin.cpp

  ${sodium_src_dir}/gui/starcross_test.cpp
  ${sodium_src_dir}/gui/starcross_test.hpp

  ${sodium_src_dir}/gui/staticpa_tool.hpp
  ${sodium_src_dir}/gui/staticpa_toolwin.hpp
  ${sodium_src_dir}/gui/staticpa_toolwin.cpp

  ${sodium_src_dir}/gui/star_profile.cpp
  ${sodium_src_dir}/gui/star_profile.hpp

  ${sodium_src_dir}/gui/testguide.cpp
  ${sodium_src_dir}/gui/testguide.hpp

  ${sodium_src_dir}/gui/wxled.cpp
  ${sodium_src_dir}/gui/wxled.hpp

  ${sodium_src_dir}/gui/camcal_import_dialog.cpp
  ${sodium_src_dir}/gui/camcal_import_dialog.hpp

  ${sodium_src_dir}/gui/messagebox_proxy.cpp
  ${sodium_src_dir}/gui/messagebox_proxy.hpp

  ${sodium_src_dir}/gui/eegg.cpp

  ${sodium_src_dir}/gui/nudge_lock.cpp
  ${sodium_src_dir}/gui/nudge_lock.hpp

  ${sodium_src_dir}/gui/target.cpp
  ${sodium_src_dir}/gui/target.hpp

  ${sodium_src_dir}/gui/server_dialog_base.cpp
  ${sodium_src_dir}/gui/server_dialog_base.hpp
  ${sodium_src_dir}/gui/server_dialog.cpp
  ${sodium_src_dir}/gui/server_dialog.hpp

  ${sodium_src_dir}/gui/solver_dialog_base.cpp
  ${sodium_src_dir}/gui/solver_dialog_base.hpp
  ${sodium_src_dir}/gui/solver_dialog.cpp
  ${sodium_src_dir}/gui/solver_dialog.hpp

  ${sodium_src_dir}/gui/runinbg.cpp
  ${sodium_src_dir}/gui/runinbg.hpp
)
source_group(Gui FILES ${gui_src})

# it would ease if files are organised in an alphabetical order
set(sodium_SRC

  ${sodium_src_dir}/guiding/circbuf.hpp

  ${sodium_src_dir}/image/fitsiowrap.cpp
  ${sodium_src_dir}/image/fitsiowrap.hpp
  ${sodium_src_dir}/image/image_math.cpp
  ${sodium_src_dir}/image/image_math.hpp
  ${sodium_src_dir}/image/usImage.cpp
  ${sodium_src_dir}/image/usImage.hpp
  
  ${sodium_src_dir}/ao/gear_simulator.hpp
  ${sodium_src_dir}/ao/gear_simulator.cpp

  
  ${sodium_src_dir}/logger/imagelogger.cpp
  ${sodium_src_dir}/logger/imagelogger.hpp
  ${sodium_src_dir}/logger/debuglog.cpp
  ${sodium_src_dir}/logger/debuglog.hpp
  ${sodium_src_dir}/logger/guidinglog.cpp
  ${sodium_src_dir}/logger/guidinglog.hpp
  ${sodium_src_dir}/logger/logger.cpp
  ${sodium_src_dir}/logger/logger.hpp
  ${sodium_src_dir}/logger/log_uploader.cpp
  ${sodium_src_dir}/logger/log_uploader.hpp

  ${sodium_src_dir}/guiding/guiding_stats.cpp
  ${sodium_src_dir}/guiding/guiding_stats.hpp
  
  ${sodium_src_dir}/device/onboard_st4.cpp
  ${sodium_src_dir}/device/onboard_st4.hpp
  
  ${sodium_src_dir}/sodium.cpp
  ${sodium_src_dir}/sodium.hpp

  ${sodium_src_dir}/config/config.cpp
  ${sodium_src_dir}/config/config.hpp
  ${sodium_src_dir}/controller.cpp
  ${sodium_src_dir}/controller.hpp

  ${sodium_src_dir}/network/update.cpp
  ${sodium_src_dir}/network/update.hpp
  ${sodium_src_dir}/network/sha1.cpp
  ${sodium_src_dir}/network/sha1.hpp
  
  ${sodium_src_dir}/guiding/point.hpp

  ${sodium_src_dir}/device/serialport.cpp
  ${sodium_src_dir}/device/serialport.hpp
  ${sodium_src_dir}/device/serialport_loopback.cpp
  ${sodium_src_dir}/device/serialport_loopback.hpp
  ${sodium_src_dir}/device/serialport_mac.cpp
  ${sodium_src_dir}/device/serialport_mac.hpp
  ${sodium_src_dir}/device/serialport_win32.cpp
  ${sodium_src_dir}/device/serialport_win32.hpp
  ${sodium_src_dir}/device/serialport_posix.cpp
  ${sodium_src_dir}/device/serialport_posix.hpp
  ${sodium_src_dir}/device/serialports.hpp
  
  ${sodium_src_dir}/guiding/star.cpp
  ${sodium_src_dir}/guiding/star.hpp
 
)


if (WIN32)
  set(sodium_SRC
      ${sodium_SRC}
      # windows-only sources
      ${sodium_src_dir}/win32/parallelports.hpp
      ${sodium_src_dir}/win32/parallelport.hpp
      ${sodium_src_dir}/win32/parallelport.cpp

      ${sodium_src_dir}/win32/parallelport_win32.hpp
      ${sodium_src_dir}/win32/parallelport_win32.cpp

      ${sodium_src_dir}/win32/comdispatch.hpp
      ${sodium_src_dir}/win32/comdispatch.cpp
     )
endif()

source_group(src FILES ${sodium_SRC})

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
      set(sodium_SRC ${sodium_SRC} ${current_file})
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
   sodium
   ${server_SRC}
   ${modloader_SRC}
   ${scopes_SRC}
   ${cam_SRC}
   ${stepguiders_SRC}
   ${rotators_SRC}
   ${guiding_SRC}
   ${gui_src}
   ${sodium_SRC}
   )

  target_precompile_headers(sodium PUBLIC ${sodium_src_dir}/sodium.hpp)

  # 链接 Python 库
  target_link_libraries(sodium ${Python_LIBRARIES})


  target_link_libraries(sodium X11)

  find_package(spdlog REQUIRED)
  target_link_libraries(sodium spdlog::spdlog_header_only)

  set_target_properties(
    sodium
    PROPERTIES
      OUTPUT_NAME sodium
    )

else()
  message(FATAL_ERROR "Unsupported platform")
endif()


# properties of the project common to all platforms
target_compile_definitions(sodium PRIVATE "${wxWidgets_DEFINITIONS}" "HAVE_TYPE_TRAITS")
target_compile_options(sodium PRIVATE "${wxWidgets_CXX_FLAGS};")
if(APPLE AND NOT APPLE32)
  # suppress warnings about "overrides a member function but is not marked 'override'" with wxDECLARE_EVENT_TABLE
  target_compile_options(sodium PRIVATE "-Wno-inconsistent-missing-override")
endif()
target_include_directories(sodium PRIVATE ${wxWidgets_INCLUDE_DIRS})

if (SODIUM_LINK_EXTERNAL_RELEASE)
  target_link_libraries(sodium optimized ${SODIUM_LINK_EXTERNAL_RELEASE})
endif()
if (SODIUM_LINK_EXTERNAL_DEBUG)
  target_link_libraries(sodium debug ${SODIUM_LINK_EXTERNAL_DEBUG})
endif()
target_link_libraries(sodium
                      MPIIS_GP GPGuider # GP Guider
                      ${SODIUM_LINK_EXTERNAL})



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
    add_dependencies(sodium ${locale}_html)
  endif()

  get_translation_files(current_messages LOCALE ${locale})
  if(NOT "${current_messages}" STREQUAL "")
    if("${locale}" STREQUAL "${default_locale}")
      #set(dependent_target ${target_string_extraction_from_sources})
    else()
      #set(dependent_target ${locale}_translation)
      add_dependencies(sodium ${locale}_translation)
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
      set(build_tree_target_folder $<TARGET_FILE_DIR:sodium>/)
      set(install_options DESTINATION ${CMAKE_INSTALL_PREFIX}/)
    else()
      set(build_tree_target_folder $<TARGET_FILE_DIR:sodium>/locale/${current_locale})
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
          TARGET sodium
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
      set(destination share/sodium/)
      set(install_help_options DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sodium/)
    else()
      set(destination share/sodium/locale/${current_locale}/)
      set(install_help_options DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sodium/locale/${current_locale}/ OPTIONAL)
    endif()

    get_zip_file(current_zip LOCALE ${current_locale})
    get_translation_files(current_translations LOCALE ${current_locale})

    foreach(current_file IN LISTS current_translations current_zip)
      get_filename_component(message_file_name ${current_file} NAME)
      get_filename_component(message_file_ext ${current_file} EXT)
      if("${message_file_name}" STREQUAL "messages.mo" OR "${message_file_ext}" STREQUAL ".zip")
        add_custom_command(
          TARGET sodium
          POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E make_directory $<TARGET_FILE_DIR:sodium>/${destination}
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${current_file} $<TARGET_FILE_DIR:sodium>/${destination}${message_file_name}
        )
      endif()

      if("${message_file_name}" STREQUAL "messages.mo")
          install(FILES       ${current_file}
                  DESTINATION  ${CMAKE_INSTALL_PREFIX}/share/locale/${current_locale}/LC_MESSAGES/
                  RENAME "sodium.mo"
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
include(cmake_modules/SodiumPackaging.cmake)
