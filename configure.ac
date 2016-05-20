AC_INIT([libfilezilla],[0.5.2],[tim.kosse@filezilla-project.org],[],[https://lib.filezilla-project.org/])
AC_CONFIG_HEADERS([lib/libfilezilla/private/config.hpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([dist-bzip2])

AC_CANONICAL_HOST

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL

LT_INIT([win32-dll])
LT_LANG([C++])

AC_CACHE_SAVE

if test "X$GCC" = Xyes; then
  CFLAGS="$CFLAGS -Wall -g"
  CXXFLAGS="$CXXFLAGS -Wall -g"
fi

# Check for C++14 support
CXX_COMPILE_STDCXX_14([],[mandatory])
CHECK_LIBCXX

# libstdc++ as shipped with GCC 4.7 has a steady clock that isn't steady (who implements such crap?)
CHECK_STEADY_CLOCK

# No thread_local in GCC 4.7 and it for some reason isn't supported on OS X either
CHECK_THREAD_LOCAL

AC_CHECK_DECLS([CLOCK_MONOTONIC], [], [], [[#include <time.h>]])


# Some platforms, e.g. OS X, lack posix_fadvise
AC_CHECK_FUNCS(posix_fadvise)

# Some platforms have no d_type entry in their dirent structure
gl_CHECK_TYPE_STRUCT_DIRENT_D_TYPE

CHECK_THREADSAFE_LOCALTIME
CHECK_THREADSAFE_GMTIME
CHECK_INVERSE_GMTIME

AC_CHECK_FUNCS([clock_gettime])
AC_CHECK_DECLS([pthread_condattr_setclock], [], [], [[#include <pthread.h>]])

# Check if we're on Windows
if echo $host_os | grep 'cygwin\|mingw\|^msys$' > /dev/null 2>&1; then
  windows=1
else
  windows=0
fi

if test "$windows" = "1"; then
  libdeps=
else
  libdeps=-lpthread
  CHECK_ICONV([libdeps="$libdeps -liconv"])
fi

AC_SUBST([libdeps])


# Add build information to config.hpp
# -----------------------------------

# Add host to config.hpp
AC_DEFINE_UNQUOTED(USED_HOST, ["$host"], [Host system under which the program will run.])
AC_DEFINE_UNQUOTED(USED_BUILD, ["$build"], [Build system under which the program was compiled on.])

# Add used CXXFLAGS to config.hpp
AC_DEFINE_UNQUOTED(USED_CXXFLAGS, ["$CXXFLAGS"], [Define to the used CXXFLAGS to compile this package.])

# Get compiler (version)
AH_TEMPLATE(USED_COMPILER, [Define to name and version of used compiler])
if COMPILER=`$CC --version | head -n 1`; then
  AC_DEFINE_UNQUOTED(USED_COMPILER, ["$COMPILER"])
fi



# Check for windres on MinGW builds
# ---------------------------------

WINDRESFLAGS=
use_resourcefile=
AC_MSG_CHECKING([whether to use windres])
if test "$windows" = "1"; then
  AC_MSG_RESULT([yes])
  AC_CHECK_TOOL([WINDRES], [windres])
  if test "x$WINDRES" = "x"; then
	AC_MSG_ERROR([windres could not be found, please make sure this program is within your path.])
  fi
  use_resourcefile=true
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(WINDRES)



# Set version macros
# ------------------

# version.rc.in needs a major, minor and micro version number
PACKAGE_VERSION_SUFFIX=${PACKAGE_VERSION##*-}
vnumber=${PACKAGE_VERSION%-*}
PACKAGE_VERSION_MAJOR=${vnumber%%.*}
vnumber=${vnumber#$PACKAGE_VERSION_MAJOR}
vnumber=${vnumber#.}
PACKAGE_VERSION_MINOR=${vnumber%%.*}
vnumber=${vnumber#$PACKAGE_VERSION_MINOR}
vnumber=${vnumber#.}
PACKAGE_VERSION_MICRO=${vnumber%%.*}
vnumber=${vnumber#$PACKAGE_VERSION_MICRO}
vnumber=${vnumber#.}
PACKAGE_VERSION_NANO=${vnumber%%.*}

if test -z "$PACKAGE_VERSION_MAJOR"; then
  PACKAGE_VERSION_MAJOR=0
fi
if test -z "$PACKAGE_VERSION_MINOR"; then
  PACKAGE_VERSION_MINOR=0
fi
if test -z "$PACKAGE_VERSION_MICRO"; then
  PACKAGE_VERSION_MICRO=0
fi
if test -z "$PACKAGE_VERSION_NANO"; then
  PACKAGE_VERSION_NANO=0
fi

AC_SUBST(PACKAGE_VERSION_MAJOR)
AC_SUBST(PACKAGE_VERSION_MINOR)
AC_SUBST(PACKAGE_VERSION_MICRO)
AC_SUBST(PACKAGE_VERSION_NANO)
AC_SUBST(PACKAGE_VERSION_SUFFIX)

# Enable DEP and ASLR linker flags
# --------------------------------

AX_CHECK_LINK_FLAG([-Wl,--dynamicbase], [LDFLAGS="$LDFLAGS -Wl,--dynamicbase"])
AX_CHECK_LINK_FLAG([-Wl,--nxcompat], [LDFLAGS="$LDFLAGS -Wl,--nxcompat"])

# Set Windows-specific linker flags
# ---------------------------------
if test "$windows" = 1 > /dev/null 2>&1; then
  # We currently require Vista minimum.
  AX_CHECK_LINK_FLAG([-Wl,--major-subsystem-version -Wl,6], [LDFLAGS="$LDFLAGS -Wl,--major-subsystem-version -Wl,6"])
  AX_CHECK_LINK_FLAG([-Wl,--minor-subsystem-version -Wl,0], [LDFLAGS="$LDFLAGS -Wl,--minor-subsystem-version -Wl,0"])
  AX_CHECK_LINK_FLAG([-Wl,--major-os-version -Wl,6], [LDFLAGS="$LDFLAGS -Wl,--major-os-version -Wl,6"])
  AX_CHECK_LINK_FLAG([-Wl,--minor-os-version -Wl,0], [LDFLAGS="$LDFLAGS -Wl,--minor-os-version -Wl,0"])

  # Set the Large Address Aware flag on 32bit Windows
  if ! echo $host_cpu | grep "x86_64" > /dev/null 2>&1; then
	AX_CHECK_LINK_FLAG([-Wl,--large-address-aware], [LDFLAGS="$LDFLAGS -Wl,--large-address-aware"])
  fi
fi


# CppUnit tests
# -------------

AM_PATH_CPPUNIT([1.10.2], [has_cppunit="yes"])


# Doxygen
# -------

DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(libfilezilla, Doxyfile)

# Output automake conditionals
# ----------------------------

AM_CONDITIONAL(USE_RESOURCEFILE, test "$use_resourcefile" = "true")
AM_CONDITIONAL(HAS_CPPUNIT, [test "$has_cppunit" = "yes"])

AC_CONFIG_FILES(
  Makefile
  lib/Makefile
  lib/libfilezilla.pc
  lib/libfilezilla/version.hpp
  demos/Makefile
  doc/Doxyfile
  doc/Makefile
  tests/Makefile
)

AC_OUTPUT
