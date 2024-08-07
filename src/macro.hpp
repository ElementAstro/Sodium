/*
 * macro.cpp
 *
 * Copyright (C) 2023-2024 Max Qian <lightapt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define APPNAME _T("Sodium")
#define APPNAME_LOWER _T("sodium")
#define SODIUM_VERSION _T("1.0.0")
#define SODIUM_SUBVER _T("alpha")
#define SODIUM_FULLVER SODIUM_VERSION SODIUM_SUBVER
#define PHDVERSION _T("2.6.13")
#define PHDSUBVER _T("dev5")
#define FULLVER PHDVERSION PHDSUBVER

#if defined(__WINDOWS__)
#pragma warning(disable : 4189)
#pragma warning(disable : 4018)
#pragma warning(disable : 4305)
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)
#if HAVE_VLD
#include <vld.h>
#endif
#endif  // __WINDOWS__

#if defined(__WINDOWS__)
#define SODIUM_OSNAME _T("Windows")
#define PATHSEPCH '\\'
#define PATHSEPSTR "\\"
#endif

#if defined(__APPLE__)
#define SODIUM_OSNAME _T("OSX")
#define PATHSEPCH '/'
#define PATHSEPSTR "/"
#endif

#if defined(__WXGTK__)
#define SODIUM_OSNAME _T("Linux")
#define PATHSEPCH '/'
#define PATHSEPSTR _T("/")
#endif

#define DEGREES_SYMBOL "\u00B0"
#define MICRONS_SYMBOL "\u00B5m"

// #define TEST_TRANSFORMS

#define ROUND(x) (int)floor((x) + 0.5)
#define ROUNDF(x) (int)floorf((x) + 0.5)
#define DIV_ROUND_UP(x, y) (((x) + (y) - 1) / (y))

/* eliminate warnings for unused variables */
#define POSSIBLY_UNUSED(x) (void)(x)
// #define POSSIBLY_UNUSED(x) printf(x)

// these macros are used for building messages for thrown exceptions
// It is surprisingly hard to get the line number into a string...
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define THROW_INFO_BASE(intro, file, line) intro " " file ":" TOSTRING(line)
#define LOG_INFO(s)                                                       \
    (Debug.AddLine(wxString(THROW_INFO_BASE("At", __FILE__, __LINE__) "-" \
                                                                      ">" s)))
#define THROW_INFO(s) \
    (Debug.AddLine(   \
        wxString(THROW_INFO_BASE("Throw from", __FILE__, __LINE__) "->" s)))
#define ERROR_INFO(s)        \
    (Debug.AddLine(wxString( \
        THROW_INFO_BASE("Error thrown from", __FILE__, __LINE__) "->" s)))

#if defined(__WINDOWS__)
#define SODIUM_MESSAGES_CATALOG "messages"
#endif

#if defined(__APPLE__)
#define SODIUM_MESSAGES_CATALOG "messages"
#endif

#if defined(__linux__) || defined(__FreeBSD__)
// On Linux the messages catalogs for all the applications are in the same
// directory in /usr/share/locale, so the catalog name must be the application
// name.
#define SODIUM_MESSAGES_CATALOG "sodium"
#endif

#define DEBUG_INFO(fmt, args...)                                      \
    printf("\033[33m[%s:%d]\033[0m " #fmt "\r\n", __func__, __LINE__, \
           ##args)  // by QIU

// ALIGNAS
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define ATOM_ALIGNAS(x) __declspec(align(x))
#define ATOM_PACKED __pragma(pack(push, 1))
#define ATOM_PACKED_END __pragma(pack(pop))
#else
#define ATOM_ALIGNAS(x) __attribute__((aligned(x)))
#define ATOM_PACKED __attribute__((packed))
#define ATOM_PACKED_END
#endif
