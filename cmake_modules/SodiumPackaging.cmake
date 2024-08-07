#=============================================================================
# Copyright 2017, Max Planck Society.
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
#=============================================================================

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
SET(CPACK_PACKAGE_VENDOR "LightAPT Develop Team")

string(TIMESTAMP cdate "%Y%m%d%H%M%S" UTC)
site_name(HOST_NAME)

if(UNIX AND NOT APPLE)
  install(TARGETS sodium
          RUNTIME DESTINATION bin)
  install(FILES ${LGuider_INSTALL_LIBS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/sodium/)
  install(FILES ${sodium_src_dir}/src/icons/phd2_48.png
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/
          RENAME "sodium.png")
  install(FILES ${sodium_src_dir}/sodium.desktop
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/ )
  install(FILES ${sodium_src_dir}/sodium.appdata.xml
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/metainfo/ )

  # Make Debian package
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LightAPT Develop Team https://github.com/AstroAir-Develop-Team/Sodium")
  # get package information
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm(.*)")
    set(debarch "armhf")
  else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(debarch "amd64")
    else()
      set(debarch "i386")
    endif()
  endif()
  # package name is lowercase short name
  set(CPACK_DEBIAN_PACKAGE_NAME "sodium")
  # architecture use debian terminology
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${debarch}")
  # version control compatible version name < ppa name to allow further upgrade
  set(CPACK_DEBIAN_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${cdate}.")
  # set version and arch compatible file name
  set(CPACK_PACKAGE_FILE_NAME "sodium_${CPACK_DEBIAN_PACKAGE_VERSION}_${debarch}")
  # Ubuntu 14.04 compatible minimal dependency
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17), libgcc1 (>= 1:4.1.1), libnova-0.14-0 | libnova-0.16-0, libstdc++6 (>= 4.2.1), libusb-1.0-0 (>= 2:1.0.8), libwxbase3.0-0 | libwxbase3.0-0v5 (>= 3.0.0), libwxgtk3.0-0 | libwxgtk3.0-0v5 | libwxgtk3.0-gtk3-0v5 (>=3.0.0), libx11-6, zlib1g (>= 1:1.1.4)")
  set(CPACK_DEBIAN_PACKAGE_SUGGESTS "indi-bin (>= 0.9.7)")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Sodium Auto-Guiding Software (Modified from LGuider2)")
  # same section as many astronomy packages
  set(CPACK_DEBIAN_PACKAGE_SECTION "education")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
endif()

include(CPack)
