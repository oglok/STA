noinst_LIBRARIES = libcelutil.a
noinst_HEADERS = $(wildcard *.h)
dosstuff = util.mak celutil.dsp
noinst_DATA = $(dosstuff)

libcelutil_a_SOURCES = \
	bigfix.cpp \
	color.cpp \
	debug.cpp \
	directory.cpp \
	filetype.cpp \
	formatnum.cpp \
	utf8.cpp \
	util.cpp \
	unixdirectory.cpp \
	unixtimer.cpp

WINSOURCES = \
	wintimer.cpp \
	winutil.cpp \
        windirectory.cpp

EXTRA_DIST = $(noinst_DATA)

EXTRA_libcelutil_a_SOURCES= $(WINSOURCES)
