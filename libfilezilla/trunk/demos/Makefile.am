noinst_PROGRAMS = timer_fizzbuzz process events

timer_fizzbuzz_SOURCES = timer_fizzbuzz.cpp

timer_fizzbuzz_CPPFLAGS = $(AM_CPPFLAGS)
timer_fizzbuzz_CPPFLAGS += -I$(top_srcdir)/lib

timer_fizzbuzz_LDFLAGS = $(AM_LDFLAGS)
timer_fizzbuzz_LDFLAGS += -no-install

timer_fizzbuzz_LDADD = ../lib/libfilezilla.la
timer_fizzbuzz_LDADD += $(libdeps)

timer_fizzbuzz_DEPENDENCIES = ../lib/libfilezilla.la


process_SOURCES = process.cpp

process_CPPFLAGS = $(AM_CPPFLAGS)
process_CPPFLAGS += -I$(top_srcdir)/lib

process_LDFLAGS = $(AM_LDFLAGS)
process_LDFLAGS += -no-install

process_LDADD = ../lib/libfilezilla.la
process_LDADD += $(libdeps)

process_DEPENDENCIES = ../lib/libfilezilla.la



events_SOURCES = events.cpp

events_CPPFLAGS = $(AM_CPPFLAGS)
events_CPPFLAGS += -I$(top_srcdir)/lib

events_LDFLAGS = $(AM_LDFLAGS)
events_LDFLAGS += -no-install

events_LDADD = ../lib/libfilezilla.la
events_LDADD += $(libdeps)

events_DEPENDENCIES = ../lib/libfilezilla.la


dist_noinst_DATA = demo_events.vcxproj examples.txt
