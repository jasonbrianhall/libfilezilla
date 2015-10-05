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
	util.cpp

nobase_include_HEADERS = \
	libfilezilla/apply.hpp \
	libfilezilla/event.hpp \
	libfilezilla/event_handler.hpp \
	libfilezilla/event_loop.hpp \
	libfilezilla/file.hpp \
	libfilezilla/mutex.hpp \
	libfilezilla/process.hpp \
	libfilezilla/string.hpp \
	libfilezilla/thread.hpp \
	libfilezilla/time.hpp \
	libfilezilla/util.hpp \
	libfilezilla/libfilezilla.hpp \
	libfilezilla/private/defs.hpp \
	libfilezilla/private/windows.hpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfilezilla.pc

DISTCLEANFILES = libfilezilla.pc

#dist_noinst_DATA = libfilezilla.vcxproj
