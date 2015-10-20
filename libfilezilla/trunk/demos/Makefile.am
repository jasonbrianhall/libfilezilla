noinst_PROGRAMS = timer_fizzbuzz

timer_fizzbuzz_SOURCES = timer_fizzbuzz.cpp

timer_fizzbuzz_CPPFLAGS = $(AM_CPPFLAGS)
timer_fizzbuzz_CPPFLAGS += -I$(top_srcdir)/lib

timer_fizzbuzz_LDFLAGS = $(AM_LDFLAGS)
timer_fizzbuzz_LDFLAGS += -no-install

timer_fizzbuzz_LDADD = ../lib/libfilezilla.la
timer_fizzbuzz_LDADD += $(libdeps)

timer_fizzbuzz_DEPENDENCIES = ../lib/libfilezilla.la
