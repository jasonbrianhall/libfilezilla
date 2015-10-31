#ifndef LIBFILEZILLA_HEADER
#define LIBFILEZILLA_HEADER

#include "private/defs.hpp"
#include "private/visibility.hpp"

#include "string.hpp"

/** \file
 * \brief Sets some global macros and further includes \ref string.hpp
 */

/** \namespace fz
 * \brief The namespace used by libfilezilla
 *
 * All declarations in any libfilezilla header are in this namespace.
 */


#ifdef DOXYGEN
/// This macro is defined if libfilezilla has been compiled for the Windows platform.
#define FZ_WINDOWS

/// This macro is defined if libfilezilla has been compiled for OS X.
#define FZ_MAC

/// This macro is defined if libfilezilla has been compiled Unix(-like) platforms, but excluding OS X.
#define FZ_UNIX


/** \mainpage
 * \tableofcontents
 *
 * \section Introduction
 *
 * Welcome to the libfilezilla documentation.
 *
 * libfilezilla is a small and modern C++ library, offering some basic functionality to build high-performing, platform-independent programs. Some of the highlights include:
 *
 * \li A typesafe, multi-threaded event system that's very simple to use yet extremely efficient
 * \li Timers for periodic events
 * \li A \ref fz::datetime "datetime" class that not only tracks timestamp but also their accuracy, which simplifies dealing with timestamps originating from different sources
 * \li Simple \ref fz::process "process" handling for spawning child processes with redirected I/O
 *
 * libfilezilla works on most modern platforms that have a C++14 compiler, including, but not limited to, Linux, *BSD, OS X and Windows.
 *
 * \section License
 *
 * libfilezilla is distributed under the terms and conditions of the GNU General Public License version 2 or higher.
 *
 * \section using Using libfilezilla
 *
 * libfilezilla uses the GNU autotools as build system. See the INSTALL file for details.
 *
 * To use libfilezilla in your project, you can use <a href="http://www.freedesktop.org/wiki/Software/pkg-config/">pkg-config</a> to add the required compiler and linker flags.
 *
 * If your compiler does not enable C++14 (or higher) by default, you may need to add -std=c++14 or similar to your compiler flags. Check your compiler's manual for details.
 *
 * \subsection using_vs Using libfilezilla with Visual Studio
 *
 * You can compile libefilezilla using the provided Visual Studio solution.
 *
 * To use libfilezilla in your own project, add libfilezilla to the include and library directories and link against libfilezilla.lib
 * If you want to link against the DLL version of libfilezilla you must also add FZ_USING_DLL to your preprocessor defines.
 */
#endif

#endif
