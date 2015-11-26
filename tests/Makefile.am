# Rules for the test code (use `make check` to execute)

TESTS = test
check_PROGRAMS = $(TESTS)

test_SOURCES =  test.cpp \
		dispatch.cpp \
		eventloop.cpp \
		iputils.cpp \
		time.cpp

test_CPPFLAGS = $(AM_CPPFLAGS)
test_CPPFLAGS += -I$(top_srcdir)/lib
test_CPPFLAGS += $(CPPUNIT_CFLAGS)

test_LDFLAGS = $(AM_LDFLAGS)
test_LDFLAGS += -no-install

test_LDADD = ../lib/libfilezilla.la
test_LDADD += $(CPPUNIT_LIBS)
test_LDADD += $(libdeps)

test_DEPENDENCIES = ../lib/libfilezilla.la

noinst_HEADERS = test_utils.hpp
