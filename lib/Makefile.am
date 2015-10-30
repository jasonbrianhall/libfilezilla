lib_LTLIBRARIES = libfilezilla.la

libfilezilla_la_SOURCES = \
	event_handler.cpp \
	event_loop.cpp \
	file.cpp \
	mutex.cpp \
	process.cpp \
	string.cpp \
	thread.cpp \
	time.cpp \
	util.cpp \
	version.cpp

nobase_include_HEADERS = \
	libfilezilla/apply.hpp \
	libfilezilla/event.hpp \
	libfilezilla/event_handler.hpp \
	libfilezilla/event_loop.hpp \
	libfilezilla/file.hpp \
	libfilezilla/libfilezilla.hpp \
	libfilezilla/mutex.hpp \
	libfilezilla/process.hpp \
	libfilezilla/string.hpp \
	libfilezilla/thread.hpp \
	libfilezilla/time.hpp \
	libfilezilla/util.hpp \
	libfilezilla/version.hpp \
	libfilezilla/private/defs.hpp \
	libfilezilla/private/visibility.hpp \
	libfilezilla/private/windows.hpp

libfilezilla_la_CPPFLAGS = $(AM_CPPFLAGS)
libfilezilla_la_CPPFLAGS += -DBUILDING_LIBFILEZILLA

# Needed for version.hpp in out-of-tree builds
libfilezilla_la_CPPFLAGS += -I$(srcdir)/libfilezilla

libfilezilla_la_CXXFLAGS = $(AM_CXXFLAGS)
libfilezilla_la_CXXFLAGS += -fvisibility=hidden

libfilezilla_la_LDFLAGS = $(AM_LDFLAGS)
libfilezilla_la_LDFLAGS += -no-undefined

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfilezilla.pc

DISTCLEANFILES = libfilezilla.pc

dist_noinst_DATA = libfilezilla.vcxproj
