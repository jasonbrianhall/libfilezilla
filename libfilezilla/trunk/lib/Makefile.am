lib_LTLIBRARIES = libfilezilla.la

libfilezilla_la_SOURCES = \
	event.cpp \
	event_handler.cpp \
	event_loop.cpp \
	file.cpp \
	iputils.cpp \
	local_filesys.cpp \
	mutex.cpp \
	process.cpp \
	recursive_remove.cpp \
	string.cpp \
	thread.cpp \
	thread_pool.cpp \
	time.cpp \
	util.cpp \
	version.cpp

nobase_include_HEADERS = \
	libfilezilla/apply.hpp \
	libfilezilla/event.hpp \
	libfilezilla/event_handler.hpp \
	libfilezilla/event_loop.hpp \
	libfilezilla/file.hpp \
	libfilezilla/format.hpp \
	libfilezilla/iputils.hpp \
	libfilezilla/libfilezilla.hpp \
	libfilezilla/local_filesys.hpp \
	libfilezilla/mutex.hpp \
	libfilezilla/optional.hpp \
	libfilezilla/process.hpp \
	libfilezilla/recursive_remove.hpp \
	libfilezilla/shared.hpp \
	libfilezilla/string.hpp \
	libfilezilla/thread.hpp \
	libfilezilla/thread_pool.hpp \
	libfilezilla/time.hpp \
	libfilezilla/util.hpp \
	libfilezilla/version.hpp \
	libfilezilla/private/defs.hpp \
	libfilezilla/private/visibility.hpp \
	libfilezilla/private/windows.hpp \
	libfilezilla/glue/wx.hpp

libfilezilla_la_CPPFLAGS = $(AM_CPPFLAGS)
libfilezilla_la_CPPFLAGS += -DBUILDING_LIBFILEZILLA

# Needed for version.hpp in out-of-tree builds
libfilezilla_la_CPPFLAGS += -I$(srcdir)/libfilezilla

libfilezilla_la_CXXFLAGS = $(AM_CXXFLAGS)
libfilezilla_la_CXXFLAGS += -fvisibility=hidden

libfilezilla_la_LDFLAGS = $(AM_LDFLAGS)
libfilezilla_la_LDFLAGS += -no-undefined
libfilezilla_la_LIBADD = $(libdeps)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfilezilla.pc

DISTCLEANFILES = libfilezilla.pc

dist_noinst_DATA = libfilezilla.vcxproj
