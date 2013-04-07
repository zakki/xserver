AM_CFLAGS = $(DIX_CFLAGS) $(XSERVER_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/hw/xfree86/os-support

SUBDIRS = imlib

CCDEF_DIR = $(LIBDIR)/ccdef
WINIMESRC = /usr/include/w32api
WINIMELIB = /lib/w32api
TRANSPORTDEFS = -DNO_UNIX_TRANSPORT -DNO_TCP_TRANSPORT

WIDGETSRCS = AdoptedShe.c AuxPanel.c CandPanel.c Canvas.c \
            CanvasShel.c ConvCtrl.c ConvDisp.c ConvMgr.c \
            ICLabel.c InputConv.c KIProto.c OffConv.c \
            OverConv.c OnConv.c WcharDisp.c XimpProto.c IMProto.c

UTILSRCS = cachedatom.c cachedfont.c dispatch.c \
            fontset.c xtwstr.c xwstr.c \
            asyncerr.c fontbank.c parsekey.c ioecall.c

WINIMESRCS = WinIMM32.c

WINIMEINCS = -I$(SERVERSRC)/hw/xwin \
		    -I$(XBUILDINCDIR) -I$(FONTINCSRC) \
		    -I$(SERVERSRC)/fb -I$(SERVERSRC)/mi \
		    -I$(SERVERSRC)/miext/shadow \
		    -I$(SERVERSRC)/miext/rootless \
		    -I$(SERVERSRC)/miext/rootless/safeAlpha \
		    -I$(SERVERSRC)/include -I$(SERVERSRC)/os  \
	            -I$(EXTINCSRC) -I$(XINCLUDESRC) \
		    -I$(SERVERSRC)/render -I$(SERVERSRC)/randr \
		    -I$(WINDOWSWMLIBSRC) -I$(WINIMELIBSRC) \
		    -I$(WINIMESRC)
#if HasShm
SHMDEF = -DHAS_SHM
#endif

MMAPDEF = -DHAS_MMAP

WINIMEDEFS = -DUSE_WINIME $(OS_DEFINES) $(SHMDEF) $(MMAPDEF) -DPROJECTROOT="\"$(PROJECTROOT)\"" -DETCX11DIR="\"$(ETCX11DIR)\""
#endif

             SRCS = 
             OBJS = $(WIDGETOBJS) $(UTILOBJS) \
                    $(WNNOBJS) $(CANNAOBJS) $(SJ3OBJS) $(ATOKOBJS) $(WINIMEOBJS)

   LOCAL_INCLUDES = -I../include -Iimlib
         INCLUDES = $(WNNINCS) $(LOCAL_INCLUDES) $(CANNAINCS) $(SJ3INCS) $(ATOKINCS) $(WINIMEINCS)

          DEFINES = $(WNNDEFS) $(SJ3DEFS) $(TRANSPORTDEFS) $(WINIMEDEFS)


noinst_LTLIBRARIES = libKi2.la
libKi2_la_SOURCES = $(WIDGETSRCS) $(UTILSRCS) \
                    $(WNNSRCS) $(CANNASRCS) $(SJ3SRCS) $(ATOKSRCS) $(WINIMESRCS)

EXTRA_DIST = \
	README.txt \
	rootless.h \
	rootlessCommon.h \
	rootlessConfig.h \
	rootlessWindow.h 
