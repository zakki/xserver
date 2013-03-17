/*
 *	kinput2
 */

/*
 * Copyright (C) 1991 by Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

#ifndef lint
static char	*rcsid = "$Id: kinput2.c,v 1.46 2002/01/06 15:13:38 ishisone Exp $";
#endif

#include <stdio.h>
#include <signal.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef USE_WINIME
#include "WinIMMDefs.h"
#endif
#include "patchlevel.h"
#include "AsyncErr.h"
#include "MyDispatch.h"
#include "IOECall.h"
#include "ConvMgr.h"
#include "IMProto.h"

#ifdef USE_WINIME
//#include "WinIMMDefs.h"
#include "WinIMM32.h"
#include "WcharDisp.h"
#include <X11/extensions/winime.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <X11/Xlibint.h>

#define WIN_CONNECT_RETRIES			40
#define WIN_CONNECT_DELAY			4
extern Bool		g_fIMEStarted;
extern unsigned long	serverGeneration;

extern char dispatchException;

#define USE_TRIGGER
//#define USE_WINTHREAD

extern void DeleteAllExtContext(void);
extern int GetAllContextConnection(fd_set *fdsSelect);

extern void freeProcessKeyLists(void);

XtWorkProcId procID;

#define SELECT_TIMEOUT_SEC 0
#define SELECT_TIMEOUT_MICROSEC 10000

#endif	// #ifdef USE_WINIME

#include "DebugPrint.h"

int	debug_all;

/* application resource */
typedef struct {
    char *conversionEngine;
    int debugLevel;
    Boolean useXIMProtocol;
    Boolean appdefs_loaded;
} AppResRec, *AppResP;

static AppResRec appres;

static XtResource app_resources[] = {
    { "conversionEngine", "ConversionEngine", XtRString, sizeof(String),
      XtOffset(AppResP, conversionEngine), XtRString, (XtPointer)"" },
    { "debugLevel", "DebugLevel", XtRInt, sizeof(int),
      XtOffset(AppResP, debugLevel), XtRImmediate, (XtPointer)0 },
    { "useXIMProtocol", "UseXIMProtocol", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResP, useXIMProtocol), XtRImmediate, (XtPointer)True },
    { "ki2AppDefsLoaded", "Ki2AppDefsLoaded", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResP, appdefs_loaded), XtRImmediate, (XtPointer)True },
};

static String fallback_resources[] = {
    "*ki2AppDefsLoaded: false",
    "Kinput2.mappedWhenManaged: false",
    "Kinput2.width: 1",
    "Kinput2.height: 1",
    NULL,
};

static XrmOptionDescRec	options[] = {
    {"-bc",		"*KinputProtocol.backwardCompatible", XrmoptionNoArg,	"True"},
    {"-font",		"*JpWcharDisplay.font",	XrmoptionSepArg,	NULL},
    {"-kanjifont",	"*JpWcharDisplay.kanjiFont", XrmoptionSepArg,	NULL},
    {"-kanafont",	"*JpWcharDisplay.kanaFont", XrmoptionSepArg,	NULL},
    {"+ximp",		".useXimpProtocol",	XrmoptionNoArg,		"off"},
    {"-xim",		".useXIMProtocol",	XrmoptionNoArg,		"on"},
    {"+xim",		".useXIMProtocol",	XrmoptionNoArg,		"off"},
    {"-tbheight",	"*ConversionControl.titlebarHeight",
     XrmoptionSepArg,	NULL},
#ifdef DEBUG
    {"-debug",		".debugLevel",		XrmoptionNoArg,		"1"},
    {"-trace",		".debugLevel",		XrmoptionNoArg,		"10"},
    {"-debuglevel",	".debugLevel",		XrmoptionSepArg,	NULL},
#endif
};

XtAppContext	apc;
Widget		toplevel;

static int	numProtocols;
static int	(*DefaultErrorHandler)();

#if XtSpecificationRelease > 5
static XtSignalId	interrupt;
static void	interruptCallback();
#else
static void	exitTimer();
#endif

static WidgetClass	getInputObjClass();
static int	IgnoreBadWindow();
#ifdef SIGNALRETURNSINT
static int	scheduleExit();
#else
static void	scheduleExit();
#endif
static void	Destroyed();
static void	Exit();
static void	realExit();
#ifndef USE_WINIME
static void	usage();
static void	print_version();
#endif	// #ifndef USE_WINIME

#ifdef USE_WINIME
#include "../../winimedefs.h"

IMEProcessedKey* g_key_list = NULL;
IMEProcessedKey* g_unreg_list = NULL;
BOOL g_ignore_key = FALSE;

int winim_clients = 0;

#ifdef USE_WINTHREAD
Boolean
FlushThread (XtPointer client_data)
{
//TRACE(("  ** work proc dispatchException(2) = %d **\n", dispatchException));
// ↑これが入ってるとログがえらい行数になる・・・
///TRACE(("%s() enter\n", __FUNCTION__));
    int iReturn;
    int iConnectionNumber = 0;
    fd_set fdsSelect;
    struct timeval tv;

    if (dispatchException != 0)
    {
	TRACE(("kinput2 shutdown.\n"));
	XtRemoveWorkProc(procID);
	freeProcessKeyLists();
	g_fIMEStarted = FALSE;
        ExitThread(0);
    }
///TRACE(("%s() leave\n", __FUNCTION__));
    return FALSE;
}
#else
Boolean
FlushThread (XtPointer client_data)
{
//TRACE(("  ** work proc dispatchException(2) = %d **\n", dispatchException));
// ↑これが入ってるとログがえらい行数になる・・・
///TRACE(("%s() enter\n", __FUNCTION__));
    int iReturn;
    int iConnectionNumber = 0;
    fd_set fdsSelect;
    struct timeval tv;

    if (dispatchException != 0)
    {
	TRACE(("kinput2 shutdown.\n"));
	XtRemoveWorkProc(procID);
	freeProcessKeyLists();
	g_fIMEStarted = FALSE;
	pthread_exit(NULL);
    }
///TRACE(("%s() leave\n", __FUNCTION__));
    return FALSE;
}
#endif

int
initKinput2(const char *display)
{
    Widget manager, protocol;
    int i;
    WidgetClass inputobjclass, displayobjclass;
    Widget inputobj;
    int ac = 0;
    char **av = NULL;
    int iIMEEventBase, iIMEErrorBase;
    char szDisplay[512];
//    Display *pDisplay = NULL;
    Display *pDisplay;
//    struct _XDisplay *pDisplay;
    int iRetries = 0;
    char* pszDisplay;
    static unsigned long s_ulServerGeneration = 0;

    int iReturn;
    int iConnectionNumber = 0;
    fd_set fdsSelect;

    TRACE (("internalKinput2Proc - initKinput2()\n"));

    /* Watch for server reset */
    if (s_ulServerGeneration != serverGeneration)
    {
        /* Save new generation number */
        s_ulServerGeneration = serverGeneration;
    }

    /* Allow multiple threads to access Xlib */
    if (XInitThreads () == 0)
    {
        TRACE (("internalKinput2Proc - XInitThreads failed.\n"));
        return 1;
    }

// >> -clipboard をつけないと「Error: Can't open display: 127.0.0.1:0.0」になってしまうのでその対策（clipboardから持ってきた）
    /* Setup the display connection string x */
    /*
     * NOTE: Always connect to screen 0 since we require that screen
     * numbers start at 0 and increase without gaps.  We only need
     * to connect to one screen on the display to get events
     * for all screens on the display.  That is why there is only
     * one clipboard client thread.
     */
    snprintf (szDisplay, 512, "127.0.0.1:%s.0", display);

    /* Print the display connection string */
    TRACE (("internalKinput2Proc - DISPLAY=%s\n", szDisplay));

    /* Open the X display */
    do
    {
        pDisplay = XOpenDisplay (szDisplay);
        if (pDisplay == NULL)
	{
	    TRACE (("internalKinput2Proc - Could not open display, "
		    "try: %d, sleeping: %d\n",
		    iRetries + 1, WIN_CONNECT_DELAY));
	    ++iRetries;
	    sleep (WIN_CONNECT_DELAY);
	    continue;
	} else
	    break;
    } while (pDisplay == NULL && s_ulServerGeneration == serverGeneration);

    /* Make sure that the display opened */
    if (pDisplay == NULL)
    {
        TRACE (("internalKinput2Proc - Failed opening the display, giving up\n"));
        return 1;
    }
// << -clipboard をつけないと「Error: Can't open display: 127.0.0.1:0.0」になってしまうのでその対策（clipboardから持ってきた）

    TRACE(("winInitImServer ()- call XtToolkitThreadInitialize\n"));
    if (!XtToolkitThreadInitialize())
    {
        TRACE (("internalKinput2Proc - XtToolkitThreadInitialize failed.\n"));
        return 1;
    }
    TRACE(("ok.\n"));

    XtSetLanguageProc (NULL, NULL, NULL);

    /* See if X supports the current locale */
    if (XSupportsLocale () == False)
    {
        TRACE (("internalKinput2Proc - Locale not supported by X.  Exiting.\n"));
        return 1;
    }

//TRACE(("  initKinput2 - a\n"));
    toplevel = XtAppInitialize(&apc, "Kinput2imm32",
                               options, XtNumber(options),
                               &ac, av,
                               fallback_resources, (ArgList)NULL, 0);


    /* initialize asynchronous error handler */
    XAEInit();

    /* initialize I/O error callback handler */
    XIOEInit();

    XtGetApplicationResources(toplevel, &appres,
			      app_resources, XtNumber(app_resources),
			      NULL, 0);

    /*
     * If the application-specific class resource file
     * (the "app-defaults" file) is not found,
     * print a warning message.
     */
    if (!appres.appdefs_loaded)
    {
	TRACE(("initKinput2 - Warning: Cannot load app-defaults file.\n"));
	TRACE(("initKinput2 -   Kinput2 may not work properly without it.\n"));
	TRACE(("initKinput2 -   Maybe kinput2 is not installed correctly,\n"));
	TRACE(("initKinput2 -   or your file search path (specified by\n"));
	TRACE(("initKinput2 -   environment variable 'XFILESEARCHPATH')\n"));
	TRACE(("initKinput2 -   is wrong.\n"));
    }

    /* set debug level */
    debug_all = appres.debugLevel;

    // cheat
    debug_all = 10;	// force Trace mode	Y.Arai

#ifdef RANDOM_ID
    /*
     * one nasty hack here:
     *
     * kinput clients often use server's window ID for the only key
     * value to identify their conversion server (kinput), and they
     * think it is dead and take appropriate action (eg connecting to
     * the new server) when they notice the ID has changed.
     *
     * but it is likely that another kinput has the same resource ID
     * base (because X servers always choose the smallest unused ID
     * base for new clients). and if it is the same, so is the owner
     * window ID, and the clients don't notice the change.
     *
     * to get rid of the problem, we add some small random offset to
     * the resource ID so that every time we get different owner ID
     * even if the resource ID base is the same.
     *
     * of course it heavily depends on the current implementaion of
     * the resource ID allocation in Xlib, so I call it 'nasty'.
     */
    XtDisplay(toplevel)->resource_id += getpid() % 1024;
#endif

    inputobjclass = getInputObjClass();

    inputobj = XtCreateWidget("inputObj", inputobjclass,
				toplevel, 0, 0);
    XtRealizeWidget(inputobj);
#ifdef USE_TRIGGER
    ICRegisterTriggerKeys(inputobj);
#endif
    /*
       Destroying the `inputobj' is postponed until all the widgets
       are realized in order to prevent duplicate initialization of
       input object, that is, to prevent making connection twice to
       input conversion server.
     */

    displayobjclass = jpWcharDisplayObjectClass;

    manager = XtVaCreateManagedWidget("convmanager",
				      conversionManagerWidgetClass,
				      toplevel,
				      XtNwidth, 1,
				      XtNheight, 1,
				      NULL);

    numProtocols = 0;

    if (appres.useXIMProtocol)
    {
	TRACE(("internalKinput2: Use XIM.\n"));
	protocol = XtVaCreateWidget("improtocol",
				    imProtocolWidgetClass,
				    manager,
				    XtNlanguage, "ja_JP",
				    XtNinputObjectClass, inputobjclass,
				    XtNdisplayObjectClass, displayobjclass,
				    XtNwidth, 1,
				    XtNheight, 1,
				    NULL);
	XtAddCallback(protocol, XtNdestroyCallback,
		      Destroyed, (XtPointer)NULL);
	numProtocols++;
    }

    if (numProtocols == 0)
    {
	fprintf(stderr, "no protocols activated\n");
	return 1;	// error exit
    }

    /* set my error handler */
    DefaultErrorHandler = XAESetErrorHandler(IgnoreBadWindow);

    XtRealizeWidget(toplevel);

    XtDestroyWidget(inputobj); /* Don't move this before XtRealizeWidget() */

    pDisplay = XtDisplay(toplevel);
    TRACE(("    DISPLAY=%s\n", DisplayString(pDisplay)));
    TRACE(("    Call XWinIMEQueryExtension...\n"));	/*YA*/
    if (!XWinIMEQueryExtension (pDisplay, &iIMEEventBase, &iIMEErrorBase))
    {
        TRACE (("winImServerProc - No IME Extension\n"));
        return 1;
    }

TRACE(("    Call XWinIMESelectInput...\n"));	/*YA*/
    XWinIMESelectInput (pDisplay, WinIMENotifyMask);
#if 0
    pIMWindow = XCreateSimpleWindow (pDisplay, DefaultRootWindow (pDisplay),
				     0, 0, 1, 1, 1, 0, 0);
    if (pIMWindow == (Window) NULL)
    {
        TRACE (("winImServerProc - Can't Create Window\n"));
        return 1;
    }
    XStoreName (pDisplay, pIMWindow, "Windows IME Input Method Server");

    if ((pInputStyles = (XIMStyles *) malloc (sizeof (XIMStyles))) == NULL)
    {
        TRACE (("winImServerProc - Can't allocate\n"));
        return 1;
    }
    pInputStyles->count_styles = sizeof (Styles) / sizeof (XIMStyle) - 1;
    pInputStyles->supported_styles = Styles;

    if ((pEncodings = (XIMEncodings *) malloc (sizeof (XIMEncodings))) == NULL)
    {
        TRACE (("winImServerProc - Can't allocate\n"));
        return 1;
    }
    pEncodings->count_encodings = sizeof (jaEncodings) / sizeof (XIMEncoding) - 1;
    pEncodings->supported_encodings = jaEncodings;

    strcpy (szTransport, "X/");

    pIms = IMOpenIM (pDisplay,
		     IMModifiers, "Xi18n",
		     IMServerWindow, pIMWindow,
		     IMServerName, pszIMName,
		     IMLocale, pszLocale,
		     IMServerTransport, szTransport,
		     IMInputStyles, pInputStyles, NULL);
    if (pIms == (XIMS) NULL)
    {
	TRACE (("winImServerProc - Can't Open Input Method Service:\n"
	        "\tInput Method Name :%s\n"
	        "\tTranport Address:%s\n",
	        pszIMName, szTransport));
	return 1;
    }
    IMSetIMValues (pIms,
		   IMEncodingList, pEncodings,
		   IMProtocolHandler, winXIMEProtoHandler,
		   IMFilterEventMask, lFilterMask, NULL);
    XSelectInput (pDisplay, pIMWindow, StructureNotifyMask | ButtonPressMask);
    //XMapWindow (pDisplay, pIMWindow);
    XFlush (pDisplay);			/* necessary flush for tcp/ip connection */
#endif

//    procID = XtAppAddWorkProc(XtWidgetToApplicationContext(toplevel), FlushThread, (XtPointer)NULL);
//#ifdef USE_WINTHREAD
    procID = XtAppAddWorkProc(apc, FlushThread, (XtPointer)NULL);
//#endif

    g_fIMEStarted = TRUE;

    for (;;)
    {
        XEvent event;
        int nStat;
        struct timeval tv;

#ifndef USE_WINTHREAD
        FD_ZERO (&fdsSelect);
        // まずアプリケーションシェルの奴
        iConnectionNumber = ConnectionNumber (pDisplay);
        FD_SET (iConnectionNumber, &fdsSelect);

        /* Wait for a Windows event or an X event */
        tv.tv_sec = SELECT_TIMEOUT_SEC;
        tv.tv_usec = SELECT_TIMEOUT_MICROSEC;
#endif	// #ifndef USE_WINTHREAD

        TRACE(("internal kinput2 loop::\n"));

#ifndef USE_WINTHREAD
        iReturn = select (iConnectionNumber + 1,/* Highest fds number */
                          &fdsSelect,		/* Read mask */
                          NULL,			/* No write mask */
                          NULL,			/* No exception mask */
                          &tv);			/* timeout = 1sec*/
        if (iReturn < 0 && errno != EINTR)
        {
            switch (errno) {
            case EBADF:
                TRACE (("EBADF"));
                break;
            case EINTR:
                TRACE (("EINTR"));
                break;
            case EINVAL:
                TRACE (("EINVAL"));
                break;
            case ENOMEM:
            TRACE (("ENOMEM"));
            break;
            }
            TRACE (("Call to select () failed: %d.  Bailing.\n", iReturn));
            break;
        }
#endif	// #ifndef USE_WINTHREAD

///	TRACE(("internal kinput2 loop::\n"));

        TRACE(("  dispatchException(1) = %d\n", dispatchException));
        if (dispatchException != 0)
        {
            TRACE(("kinput2 exit.\n"));
            break;
        }

#ifndef USE_WINTHREAD
        if (iReturn == 0)
        {	// 多分タイムアウト
            TRACE(("  ** TIMEOUT **, continue\n"));
            if (XEventsQueued(pDisplay, QueuedAfterReading) == 0)
            {
                XFlush(pDisplay);
                continue;
            }
        }
#endif	// #ifndef USE_WINTHREAD

        XtAppNextEvent(apc, &event);

        TRACE(("  a\n"));
        XtDispatchEvent(&event);

        TRACE(("  b\n"));
        nStat = winXIMEXEventHandler (pDisplay, &event, iIMEEventBase, iIMEErrorBase);

        if (nStat == 1)
            continue;
        else if (nStat == -1)
            break;
        MyDispatchEvent(&event); /* additional dispatcher */
    }
    TRACE(("@@ Kinput2 cleanup @@\n\n"));

    XtRemoveWorkProc(procID);
     freeProcessKeyLists();
//    DeleteAllExtContext();	// このタイミングではもうイベントが動かない可能性が高いのでやめ
    g_fIMEStarted = FALSE;
TRACE(("@@ internal Kinput2 exit @@\n"));
    return 0;	/* for lint */
}

// メモリを開放する
void
freeProcessKeyLists(void)
{
    IMEProcessedKey *pitem = g_key_list;
    IMEProcessedKey *pTemp;

    while (pitem != NULL)
    {
        pTemp = pitem->next;
        free(pitem);
        pitem = pTemp;
    }

    pitem = g_unreg_list;
    while (pitem != NULL)
    {
        pTemp = pitem->next;
        free(pitem);
        pitem = pTemp;
    }

    g_key_list = NULL;
    g_unreg_list = NULL;
}
#endif

static WidgetClass
getInputObjClass()
{
    WidgetClass class;

#ifdef USE_WINIME
    if (!strcmp(appres.conversionEngine, "winimm32")) {
	return winimm32ObjectClass;
    }
#endif

    /* set default input object */
#ifdef USE_WINIME
    class = winimm32ObjectClass;
#endif

    return class;
}

static int
IgnoreBadWindow(dpy, error)
Display *dpy;
XErrorEvent *error;
{
    /*
     * BadWindow events will be sent if any of the active clients die
     * during conversion.  Although I select DestroyNotify event on the
     * client windows to detect their destruction and take appropriate
     * actions, and I'm careful when doing critical operations, but still
     * there is a chance of getting unexpecting BadWindow error caused by
     * client death.
     * There are also chances of getting BadDrawable as well.
     * So I set the error handler to ignore BadWindow/BadDrawable errors.
     * Of course I'd better check if the resourceid field of the error
     * event is the window ID of a client, but I'm too lazy to do that...
     * P.S.  We should ignore BadColor also..
     */
    if (error->error_code != BadWindow &&
	error->error_code != BadDrawable &&
	error->error_code != BadColor) {
	/* invoke default error handler */
	(*DefaultErrorHandler)(dpy, error);
    }
    return 0;
}

/* ARGSUSED */
static void
Destroyed(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    static int cnt = 0;

#ifdef USE_WINIME
TRACE(("%s() cnt = %d\n", __FUNCTION__, cnt));
#endif
    /*
     * if all the protocol widgets are dead, kill myself.
     */
    if (++cnt >= numProtocols) Exit();
}

static void
Exit()
{
    static int exiting = 0;

    TRACE(("Exit()\n"));

    if (exiting) return;
    exiting = 1;

    /*
     * Destroy all widgets.
     */
    XtDestroyWidget(toplevel);

    /*
     * Postpone calling exit() until next XtNextEvent(),
     * in order to give each widget time to execute their
     * destroy procedure.
     */
    XtAppAddTimeOut(apc, 1L, realExit, (XtPointer)NULL);
}

/* ARGSUSED */
static void
realExit(cldata, timerp)
XtPointer cldata;
XtIntervalId *timerp;
{
#ifdef USE_WINIME
    TRACE(("return\n\n"));
    return;
#else
    exit(0);
#endif
}
