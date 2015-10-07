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
#endif

#endif
