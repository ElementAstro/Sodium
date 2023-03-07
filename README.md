LightGuider
===========

Light Guider for LightAPT , modified from PHD2

#### Build Guider

On Ubuntu / Debian
```
sudo apt-get install build-essential gcc git cmake pkg-config libwxgtk3.0-gtk3-dev \
   wx-common wx3.0-i18n libindi-dev libnova-dev gettext zlib1g-dev libx11-dev \
   libcurl4-gnutls-dev libcfitsio-dev libeigen3-dev libspdlog-dev
```

```
mkdir build && cd build
cmake ..
make -j4 (depends on the computer)
./lightguider
./lightguider_logviewer

make package # build deb package
```

## Directory schema

```
LightGuider
├─ .gitignore
├─ CMakeLists.txt
├─ LICENSE
├─ LICENSE.PHD2.txt
├─ README.md
├─ client
│  ├─ lightguiderclient.py
│  └─ logger.py
├─ cmake_modules
│  ├─ FindCFITSIO.cmake
│  ├─ FindINDI.cmake
│  ├─ FindNova.cmake
│  ├─ LightGuiderBuildDoc.cmake
│  ├─ LightGuiderGenerateDocScript.cmake
│  ├─ LightGuiderPackaging.cmake
│  ├─ LightGuiderRemovegettextwarning.cmake
│  ├─ compiler_options.cmake
│  └─ thirdparty.cmake
├─ doc
│  ├─ LightGuider.pdf
│  ├─ PHD_2.0_Architecture.docx
│  └─ index.md
├─ locale
│  ├─ messages.pot
│  ├─ zh_CN
│  │  ├─ messages.po
│  │  └─ wxstd.po
│  └─ zh_TW
│     ├─ messages.po
│     └─ wxstd.po
├─ messages.po
├─ phd.rc
├─ phd2.appdata.xml
├─ phd2.desktop
├─ src
│  ├─ algorithm
│  │  ├─ guide_algorithm.cpp
│  │  ├─ guide_algorithm.h
│  │  ├─ guide_algorithm_gaussian_process.cpp
│  │  ├─ guide_algorithm_gaussian_process.h
│  │  ├─ guide_algorithm_hysteresis.cpp
│  │  ├─ guide_algorithm_hysteresis.h
│  │  ├─ guide_algorithm_identity.cpp
│  │  ├─ guide_algorithm_identity.h
│  │  ├─ guide_algorithm_lowpass.cpp
│  │  ├─ guide_algorithm_lowpass.h
│  │  ├─ guide_algorithm_lowpass2.cpp
│  │  ├─ guide_algorithm_lowpass2.h
│  │  ├─ guide_algorithm_resistswitch.cpp
│  │  ├─ guide_algorithm_resistswitch.h
│  │  ├─ guide_algorithm_zfilter.cpp
│  │  ├─ guide_algorithm_zfilter.h
│  │  ├─ guide_algorithms.h
│  │  ├─ guider_multistar.cpp
│  │  └─ guider_multistar.h
│  ├─ ao
│  │  ├─ gear_simulator.cpp
│  │  ├─ gear_simulator.h
│  │  ├─ stepguider.cpp
│  │  ├─ stepguider.h
│  │  ├─ stepguider_sbigao_indi.cpp
│  │  ├─ stepguider_sbigao_indi.h
│  │  ├─ stepguider_sxao.cpp
│  │  ├─ stepguider_sxao.h
│  │  ├─ stepguider_sxao_indi.cpp
│  │  ├─ stepguider_sxao_indi.h
│  │  └─ stepguiders.h
│  ├─ backlash_comp.cpp
│  ├─ backlash_comp.h
│  ├─ camera
│  │  ├─ cam_indi.cpp
│  │  ├─ cam_indi.h
│  │  ├─ camera.cpp
│  │  ├─ camera.h
│  │  └─ cameras.h
│  ├─ circbuf.h
│  ├─ contributions
│  │  └─ MPI_IS_gaussian_process
│  │     ├─ CMakeLists.txt
│  │     ├─ src
│  │     │  ├─ covariance_functions.cpp
│  │     │  ├─ covariance_functions.h
│  │     │  ├─ gaussian_process.cpp
│  │     │  ├─ gaussian_process.h
│  │     │  ├─ gaussian_process_guider.cpp
│  │     │  └─ gaussian_process_guider.h
│  │     └─ tools
│  │        ├─ math_tools.cpp
│  │        └─ math_tools.h
│  ├─ gui
│  │  ├─ Refine_DefMap.cpp
│  │  ├─ Refine_DefMap.h
│  │  ├─ about_dialog.cpp
│  │  ├─ about_dialog.h
│  │  ├─ advanced_dialog.cpp
│  │  ├─ advanced_dialog.h
│  │  ├─ aui_controls.cpp
│  │  ├─ aui_controls.h
│  │  ├─ calibration_assistant.cpp
│  │  ├─ calibration_assistant.h
│  │  ├─ calreview_dialog.cpp
│  │  ├─ calreview_dialog.h
│  │  ├─ calstep_dialog.cpp
│  │  ├─ calstep_dialog.h
│  │  ├─ camcal_import_dialog.cpp
│  │  ├─ camcal_import_dialog.h
│  │  ├─ comet_tool.cpp
│  │  ├─ comet_tool.h
│  │  ├─ config_indi.cpp
│  │  ├─ config_indi.h
│  │  ├─ configdialog.cpp
│  │  ├─ configdialog.h
│  │  ├─ confirm_dialog.cpp
│  │  ├─ confirm_dialog.h
│  │  ├─ darks_dialog.cpp
│  │  ├─ darks_dialog.h
│  │  ├─ drift_tool.cpp
│  │  ├─ drift_tool.h
│  │  ├─ eegg.cpp
│  │  ├─ gear_dialog.cpp
│  │  ├─ gear_dialog.h
│  │  ├─ graph-stepguider.cpp
│  │  ├─ graph-stepguider.h
│  │  ├─ graph.cpp
│  │  ├─ graph.h
│  │  ├─ guiding_assistant.cpp
│  │  ├─ guiding_assistant.h
│  │  ├─ indi_gui.cpp
│  │  ├─ indi_gui.h
│  │  ├─ manualcal_dialog.cpp
│  │  ├─ manualcal_dialog.h
│  │  ├─ messagebox_proxy.cpp
│  │  ├─ messagebox_proxy.h
│  │  ├─ myframe.cpp
│  │  ├─ myframe.h
│  │  ├─ myframe_events.cpp
│  │  ├─ nudge_lock.cpp
│  │  ├─ nudge_lock.h
│  │  ├─ optionsbutton.cpp
│  │  ├─ optionsbutton.h
│  │  ├─ pierflip_tool.cpp
│  │  ├─ pierflip_tool.h
│  │  ├─ polardrift_tool.h
│  │  ├─ polardrift_toolwin.cpp
│  │  ├─ polardrift_toolwin.h
│  │  ├─ profile_wizard.cpp
│  │  ├─ profile_wizard.h
│  │  ├─ star_profile.cpp
│  │  ├─ star_profile.h
│  │  ├─ starcross_test.cpp
│  │  ├─ starcross_test.h
│  │  ├─ staticpa_tool.h
│  │  ├─ staticpa_toolwin.cpp
│  │  ├─ staticpa_toolwin.h
│  │  ├─ statswindow.cpp
│  │  ├─ statswindow.h
│  │  ├─ target.cpp
│  │  ├─ target.h
│  │  ├─ testguide.cpp
│  │  ├─ testguide.h
│  │  ├─ wxled.cpp
│  │  └─ wxled.h
│  ├─ guider.cpp
│  ├─ guider.h
│  ├─ guiders.h
│  ├─ guiding_stats.cpp
│  ├─ guiding_stats.h
│  ├─ icons
│  │  ├─ AltArrow.xpm
│  │  ├─ AzArrow.xpm
│  │  ├─ alert24.png.h
│  │  ├─ ao.xpm
│  │  ├─ at_curs1.cur
│  │  ├─ at_curs3.cur
│  │  ├─ auto_select.png.h
│  │  ├─ auto_select_disabled.png.h
│  │  ├─ brain.png.h
│  │  ├─ bullseye.cur
│  │  ├─ cam2.ico
│  │  ├─ cam2.xpm
│  │  ├─ cam_setup.png.h
│  │  ├─ cam_setup_disabled.png.h
│  │  ├─ connect.png.h
│  │  ├─ connect_disabled.png.h
│  │  ├─ connected.png.h
│  │  ├─ disconnected.png.h
│  │  ├─ down_arrow.xpm
│  │  ├─ down_arrow_bold.xpm
│  │  ├─ guide.png.h
│  │  ├─ guide_disabled.png.h
│  │  ├─ guiderlogview.xpm
│  │  ├─ help22.png.h
│  │  ├─ loop.png.h
│  │  ├─ loop_disabled.png.h
│  │  ├─ multistars.xpm
│  │  ├─ phd2.ico
│  │  ├─ phd2_128.png
│  │  ├─ phd2_128.png.h
│  │  ├─ phd2_16.png
│  │  ├─ phd2_256.png
│  │  ├─ phd2_32.png
│  │  ├─ phd2_48.png
│  │  ├─ phd2_48.png.h
│  │  ├─ phd2_64.png
│  │  ├─ phd2_64.png.h
│  │  ├─ sb_arrow_down_16.png.h
│  │  ├─ sb_arrow_left_16.png.h
│  │  ├─ sb_arrow_right_16.png.h
│  │  ├─ sb_arrow_up_16.png.h
│  │  ├─ sb_led_green.png.h
│  │  ├─ sb_led_red.png.h
│  │  ├─ sb_led_yellow.png.h
│  │  ├─ scope1.xpm
│  │  ├─ select.png.h
│  │  ├─ setup.png.h
│  │  ├─ stop.png.h
│  │  ├─ stop_disabled.png.h
│  │  └─ transparent24.png.h
│  ├─ image
│  │  ├─ fitsiowrap.cpp
│  │  ├─ fitsiowrap.h
│  │  ├─ image_math.cpp
│  │  ├─ image_math.h
│  │  ├─ imagelogger.cpp
│  │  ├─ imagelogger.h
│  │  ├─ usImage.cpp
│  │  └─ usImage.h
│  ├─ json_parser.cpp
│  ├─ json_parser.h
│  ├─ lightguider.cpp
│  ├─ lightguider.h
│  ├─ logger
│  │  ├─ debuglog.cpp
│  │  ├─ debuglog.h
│  │  ├─ guidinglog.cpp
│  │  ├─ guidinglog.h
│  │  ├─ log_uploader.cpp
│  │  ├─ log_uploader.h
│  │  ├─ logger.cpp
│  │  └─ logger.h
│  ├─ logviewer
│  │  ├─ AnalysisWin.cpp
│  │  ├─ AnalysisWin.h
│  │  ├─ CMakeLists.txt
│  │  ├─ LogViewApp.cpp
│  │  ├─ LogViewApp.h
│  │  ├─ LogViewFrame.cpp
│  │  ├─ LogViewFrame.h
│  │  ├─ LogViewFrameBase.cpp
│  │  ├─ LogViewFrameBase.h
│  │  ├─ guiderlogviewer.desktop
│  │  ├─ guiderlogviewer.ico
│  │  ├─ guiderlogviewer.png
│  │  ├─ guiderlogviewer.rc
│  │  ├─ logparser.cpp
│  │  ├─ logparser.h
│  │  └─ small.ico
│  ├─ onboard_st4.cpp
│  ├─ onboard_st4.h
│  ├─ parallelport.cpp
│  ├─ parallelport.h
│  ├─ parallelports.h
│  ├─ phdconfig.cpp
│  ├─ phdconfig.h
│  ├─ phdcontrol.cpp
│  ├─ phdcontrol.h
│  ├─ phdindiclient.cpp
│  ├─ phdindiclient.h
│  ├─ phdupdate.cpp
│  ├─ phdupdate.h
│  ├─ point.h
│  ├─ precompiled_header.cpp
│  ├─ rotator
│  │  ├─ rotator.cpp
│  │  ├─ rotator.h
│  │  └─ rotators.h
│  ├─ runinbg.cpp
│  ├─ runinbg.h
│  ├─ serialport.cpp
│  ├─ serialport.h
│  ├─ serialport_loopback.cpp
│  ├─ serialport_loopback.h
│  ├─ serialport_posix.cpp
│  ├─ serialport_posix.h
│  ├─ serialports.h
│  ├─ server
│  │  ├─ event_server.cpp
│  │  ├─ event_server.h
│  │  ├─ socket_server.cpp
│  │  ├─ socket_server.h
│  ├─ sha1.cpp
│  ├─ sha1.h
│  ├─ star.cpp
│  ├─ star.h
│  ├─ telescope
│  │  ├─ mount.cpp
│  │  ├─ mount.h
│  │  ├─ scope.cpp
│  │  ├─ scope.h
│  │  ├─ scope_indi.cpp
│  │  ├─ scope_indi.h
│  │  ├─ scope_manual_pointing.cpp
│  │  ├─ scope_manual_pointing.h
│  │  ├─ scope_onboard_st4.cpp
│  │  ├─ scope_onboard_st4.h
│  │  ├─ scope_oncamera.cpp
│  │  ├─ scope_oncamera.h
│  │  ├─ scope_onstepguider.cpp
│  │  ├─ scope_onstepguider.h
│  │  └─ scopes.h
│  ├─ worker_thread.cpp
│  ├─ worker_thread.h
│  ├─ xhair.xpm
│  ├─ zfilterfactory.cpp
│  └─ zfilterfactory.h
└─ upload.sh

```