bin_PROGRAMS = XWin

if XWIN_WINIME
WINIME_DIR = winime
endif

if XWIN_CLIPBOARD
SRCS_CLIPBOARD = \
	winclipboardinit.c \
	winclipboardtextconv.c \
	winclipboardthread.c \
	winclipboardunicode.c \
	winclipboardwndproc.c \
	winclipboardwrappers.c \
	winclipboardxevents.c
DEFS_CLIPBOARD = -DXWIN_CLIPBOARD
endif

if XWIN_GLX_WINDOWS
GLX_DIR = glx
DEFS_GLX_WINDOWS = -DXWIN_GLX_WINDOWS
XWIN_GLX_LIBS = $(top_builddir)/hw/xwin/glx/libXwinGLX.la
XWIN_GLX_LINK_FLAGS = -lopengl32
endif

if XWIN_MULTIWINDOW
SRCS_MULTIWINDOW = \
	winmultiwindowshape.c \
	winmultiwindowwindow.c \
	winmultiwindowwm.c \
	winmultiwindowwndproc.c
DEFS_MULTIWINDOW = -DXWIN_MULTIWINDOW
MULTIWINDOW_LIBS = -lshlwapi
endif

if XWIN_MULTIWINDOWEXTWM
SRCS_MULTIWINDOWEXTWM = \
	winwin32rootless.c \
	winwin32rootlesswindow.c \
	winwin32rootlesswndproc.c \
	winwindowswm.c
DEFS_MULTIWINDOWEXTWM = -DXWIN_MULTIWINDOWEXTWM
MULTIWINDOWEXTWM_LIBS = $(top_builddir)/miext/rootless/librootless.la
endif

if XWIN_NATIVEGDI
SRCS_NATIVEGDI = \
	winclip.c \
	winfillsp.c \
	winfont.c \
	wingc.c \
	wingetsp.c \
	winnativegdi.c \
	winpixmap.c \
	winpolyline.c \
	winrop.c \
	winsetsp.c
DEFS_NATIVEGDI = -DXWIN_NATIVEGDI
endif

if XWIN_PRIMARYFB
SRCS_PRIMARYFB = \
	winpfbdd.c
DEFS_PRIMARYFB = -DXWIN_PRIMARYFB
endif

if XWIN_RANDR
SRCS_RANDR = \
	winrandr.c
DEFS_RANDR = -DXWIN_RANDR
endif

if XWIN_XV
SRCS_XV = \
	winvideo.c
DEFS_XV = -DXWIN_XV
endif

if XWIN_WINIME
SRCS_WINIME = \
	winime.c \
	winimewrap.c \
	kinput2-v3.1/cmd/kinput2.c \
	kinput2-v3.1/lib/AdoptedShe.c \
	kinput2-v3.1/lib/asyncerr.c \
	kinput2-v3.1/lib/AuxPanel.c \
	kinput2-v3.1/lib/cachedatom.c \
	kinput2-v3.1/lib/cachedfont.c \
	kinput2-v3.1/lib/CandPanel.c \
	kinput2-v3.1/lib/Canvas.c \
	kinput2-v3.1/lib/CanvasShel.c \
	kinput2-v3.1/lib/ConvCtrl.c \
	kinput2-v3.1/lib/ConvDisp.c \
	kinput2-v3.1/lib/ConvMgr.c \
	kinput2-v3.1/lib/ctext.c \
	kinput2-v3.1/lib/dispatch.c \
	kinput2-v3.1/lib/fontbank.c \
	kinput2-v3.1/lib/fontset.c \
	kinput2-v3.1/lib/ICLabel.c \
	kinput2-v3.1/lib/imlib/imattr.c \
	kinput2-v3.1/lib/imlib/imbuf.c \
	kinput2-v3.1/lib/imlib/imconv.c \
	kinput2-v3.1/lib/imlib/imdata.c \
	kinput2-v3.1/lib/imlib/imdispatch.c \
	kinput2-v3.1/lib/imlib/imic.c \
	kinput2-v3.1/lib/imlib/improto.c \
	kinput2-v3.1/lib/imlib/imrequest.c \
	kinput2-v3.1/lib/imlib/imxport.c \
	kinput2-v3.1/lib/KIMProto.c \
	kinput2-v3.1/lib/InputConv.c \
	kinput2-v3.1/lib/ioecall.c \
	kinput2-v3.1/lib/KIProto.c \
	kinput2-v3.1/lib/OffConv.c \
	kinput2-v3.1/lib/OnConv.c \
	kinput2-v3.1/lib/OverConv.c \
	kinput2-v3.1/lib/parsekey.c \
	kinput2-v3.1/lib/WcharDisp.c \
	kinput2-v3.1/lib/WinIMM32.c \
	kinput2-v3.1/lib/wstring.c \
	kinput2-v3.1/lib/XimpProto.c \
	kinput2-v3.1/lib/xtwstr.c \
	kinput2-v3.1/lib/xwstr.c
DEFS_WINIME = -DXWIN_WINIME -DUSE_WINIME -DFOR_ACS
XWIN_WINIME_LIBS = -Lwinime -lwinime -lXaw -lXext -lXt -lXmu -limm32 -liconv
XWIN_WINIME_INCLUDES = -I$(srcdir)/winime \
	-I$(srcdir)/kinput2-v3.1 \
	-I$(srcdir)/kinput2-v3.1/include \
	-I$(srcdir)/kinput2-v3.1/lib/imlib
endif

SRCS =	InitInput.c \
	InitOutput.c \
	winallpriv.c \
	winauth.c \
	winblock.c \
	wincmap.c \
	winconfig.c \
	wincreatewnd.c \
	wincursor.c \
	windialogs.c \
	winengine.c \
	winerror.c \
	winglobals.c \
	winkeybd.c \
	winkeyhook.c \
	winmisc.c \
	winmonitors.c \
	winmouse.c \
	winmsg.c \
	winmsgwindow.c \
	winmultiwindowclass.c \
	winmultiwindowicons.c \
	winprefs.c \
	winprefsyacc.y \
	winprefslex.l \
	winprocarg.c \
	winregistry.c \
	winscrinit.c \
	winshaddd.c \
	winshadddnl.c \
	winshadgdi.c \
	wintrayicon.c \
	winvalargs.c \
	winwakeup.c \
	winwindow.c \
	winwndproc.c \
	ddraw.h \
	winclipboard.h \
	winconfig.h \
	win.h \
	winkeybd.h \
	winkeynames.h \
	winlayouts.h \
	winmessages.h \
	winmsg.h \
	winms.h \
	winmultiwindowclass.h \
	winprefs.h \
	winresource.h \
	winwindow.h \
	windisplay.c \
	XWin.rc \
	$(top_srcdir)/Xext/dpmsstubs.c \
	$(top_srcdir)/Xi/stubs.c \
	$(top_srcdir)/mi/miinitext.c \
	$(top_srcdir)/fb/fbcmap_mi.c \
	$(SRCS_CLIPBOARD) \
	$(SRCS_MULTIWINDOW) \
	$(SRCS_MULTIWINDOWEXTWM) \
	$(SRCS_NATIVEGDI) \
	$(SRCS_PRIMARYFB) \
	$(SRCS_RANDR) \
	$(SRCS_XV) \
	$(SRCS_WINIME)

 DEFS = $(DEFS_CLIPBOARD) \
	$(DEFS_GLX_WINDOWS) \
	$(DEFS_MULTIWINDOW) \
	$(DEFS_MULTIWINDOWEXTWM) \
	$(DEFS_NATIVEGDI) \
	$(DEFS_PRIMARYFB) \
	$(DEFS_RANDR) \
	$(DEFS_XV) \
	$(DEFS_WINIME)

XWin_SOURCES = $(SRCS)

INCLUDES = -I$(top_srcdir)/miext/rootless $(XWIN_WINIME_INCLUDES)

XWin_DEPENDENCIES = $(MULTIWINDOWEXTWM_LIBS) $(XWIN_GLX_LIBS) $(XWIN_LIBS) $(MAIN_LIB) $(XSERVER_LIBS)
XWin_LDADD = $(MULTIWINDOW_LIBS) $(MULTIWINDOWEXTWM_LIBS) $(XWIN_GLX_LIBS) $(XWIN_GLX_LINK_FLAGS) $(XWIN_LIBS) $(MAIN_LIB) $(XSERVER_LIBS) $(XSERVER_SYS_LIBS) $(XWIN_SYS_LIBS) $(XWIN_WINIME_LIBS)
XWin_LDFLAGS = -mwindows -static

.rc.o:
	$(AM_V_GEN)$(WINDRES) --use-temp-file -i $< --input-format=rc -o $@ -O coff -I $(top_builddir)/include

winprefsyacc.h: winprefsyacc.c
winprefslex.c: winprefslex.l winprefsyacc.c winprefsyacc.h

BUILT_SOURCES = winprefsyacc.h winprefsyacc.c winprefslex.c
CLEANFILES = $(BUILT_SOURCES)

AM_YFLAGS = -d
AM_LFLAGS = -i
AM_CFLAGS = -DHAVE_XWIN_CONFIG_H $(DIX_CFLAGS) \
            $(XWINMODULES_CFLAGS) \
            -I$(top_srcdir) \
            -Wno-bad-function-cast

xwinconfigdir = $(sysconfdir)/X11
xwinconfig_DATA = system.XWinrc

if XWIN_WINIME
winimeconfigdir = $(sysconfdir)/X11/app-defaults
winimeconfig_DATA = kinput2-v3.1/Kinput2imm32
endif

install-exec-hook:
	(cd $(DESTDIR)$(bindir) && rm -f X && $(LN_S) XWin$(EXEEXT) X)

EXTRA_DIST = \
	$(xwinconfig_DATA) \
	$(winimeconfig_DATA) \
	X.ico \
	XWin.rc

relink:
	$(AM_V_at)rm -f XWin$(EXEEXT) && $(MAKE) XWin$(EXEEXT)

SUBDIRS = man $(GLX_DIR) $(WINIME_DIR) .
DIST_SUBDIRS = man glx winime .
