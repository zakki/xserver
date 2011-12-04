AM_CFLAGS = $(DIX_CFLAGS) $(XSERVER_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/hw/xfree86/os-support

SUBDIRS = lib cmd

CCDEF_DIR = $(LIBDIR)/ccdef
WINIMESRC = /usr/include/w32api
WINIMELIB = /lib/w32api
TRANSPORTDEFS = -DNO_UNIX_TRANSPORT -DNO_TCP_TRANSPORT

noinst_LTLIBRARIES = libkinput2.la
libkinput2_la_SOURCES = \
	rootlessCommon.c \
	rootlessGC.c \
	rootlessScreen.c \
	rootlessValTree.c \
	rootlessWindow.c

EXTRA_DIST = \
	README.txt \
	rootless.h \
	rootlessCommon.h \
	rootlessConfig.h \
	rootlessWindow.h 
