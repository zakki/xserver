/*
 *Copyright (C) 2004-2005 Kensuke Matsuzaki. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL KENSUKE MATSUZAKI BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of Kensuke Matsuzaki
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from Kensuke Matsuzaki.
 *
 * Authors:	Y.Arai
 */

#include "dixstruct.h"
#include "inputstr.h"
/* from winclipboard.h */
/* Standard library headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <setjmp.h>
#include <pthread.h>

/* X headers */
#include "X11/X.h"
#include "X11/Xatom.h"
/* NOTE: For some unknown reason, including Xproto.h solves
 * tons of problems with including windows.h.  Unknowns reasons
 * are usually bad, so someone should investigate this.
 */
#include "X11/Xproto.h"
#include "X11/Xutil.h"
#include "X11/Xlocale.h"

/* Fixups to prevent collisions between Windows and X headers */
#define ATOM			DWORD

/* Windows headers */
#ifndef XFree86Server
#define XFree86Server
#endif
#include "winms.h">
#undef XFree86Server

/* for WINIME extension */
#define wchar wchar_t
#include <X11/extensions/winime.h>
#include <X11/extensions/winimestr.h>
#include "winkeynames.h"
#include <imm.h>


/* Typedef for DIX wrapper functions */
typedef int (*winDispatchProcPtr) (ClientPtr);


#define CYGIME_DEBUG TRUE

/* external symbols */ 
extern int initKinput2(const char *display);
extern void winWinIMESendEvent (int type, unsigned int mask, int kind, int arg, int context, HWND hwnd);
extern pthread_t	g_ptImServerProc;
extern Bool		g_fIME;
extern CARD32		g_c32LastInputEventTime;
extern DWORD		g_TriggerKeycode;
extern long		g_TriggerModifier;
extern char *display;

/*
 *
 * for Wrapper
 *
 */
DISPATCH_PROC(winimeProcEstablishConnection);
DISPATCH_PROC(winimeProcQueryTree);

extern Bool			g_fXdmcpEnabled;
extern Bool			g_fIMELaunched;
extern winDispatchProcPtr	winimeProcEstablishConnectionOrig;
extern winDispatchProcPtr	winimeProcQueryTreeOrig;

extern HWND g_hwndLastKeyPress;

//#define USE_WINTHREAD
extern DWORD			g_ThreadID;
extern HANDLE			g_ThreadHandle;

/*
 * Intialize the Winime module
 */

#ifdef USE_WINTHREAD
DWORD WINAPI
ki2ServerProc(LPVOID lpParameter)
{
ErrorF("ki2ServerProc() start.\n");
    initKinput2(display);
ErrorF("ki2ServerProc() exit.\n");
    ExitThread(0);
}
#else
static void
ki2ServerProc (void *pvNotUsed)
{
ErrorF("ki2ServerProc() start.\n");
    initKinput2(display);
ErrorF("ki2ServerProc() exit.\n");
    pthread_exit (NULL);
}
#endif

static Bool
winInitImServer ()
{
  ErrorF ("winInitImServer ()- init internal Kinput2\n");

#ifdef USE_WINTHREAD
  /* Spawn a thread for the Clipboard module */
  g_ThreadHandle = CreateThread(NULL , 0 , ki2ServerProc , NULL , 0 , &g_ThreadID);
  if (!g_ThreadHandle)
    {
      /* Bail if thread creation failed */
      ErrorF ("winInitImServer - pthread_create failed.\n");
      g_ThreadID = 0;
      return FALSE;
    }
#else
  /* Spawn a thread for the Clipboard module */
  if (pthread_create (&g_ptImServerProc,
		      NULL,
		      ki2ServerProc,
		      NULL))
    {
      /* Bail if thread creation failed */
      ErrorF ("winInitImServer - pthread_create failed.\n");
      g_ptImServerProc = 0;
      return FALSE;
    }
#endif

  return TRUE;
}

/*
 * Wrapper functions
 *
 *  from winclipboardwrappers.c
 *
 */
/*
 * Wrapper for internal QueryTree function.
 * Hides the clipboard client when it is the only client remaining.
 */

int
winimeProcQueryTree (ClientPtr client)
{
    int			iReturn;

ErrorF("%s - Hello.\n", __FUNCTION__);
    /*
     * This procedure is only used for initialization.
     * We can unwrap the original procedure at this point
     * so that this function is no longer called until the
     * server resets and the function is wrapped again.
     */
    ProcVector[X_QueryTree] = winimeProcQueryTreeOrig;

    /*
     * Call original function and bail if it fails.
     * NOTE: We must do this first, since we need XdmcpOpenDisplay
     * to be called before we initialize our ime client.
     */
    iReturn = (*winimeProcQueryTreeOrig) (client);
    if (iReturn != 0)
    {
	ErrorF ("winimeProcQueryTree - ProcQueryTree failed, bailing.\n");
	return iReturn;
    }

    /* Make errors more obvious */
    winimeProcQueryTreeOrig = NULL;

    /* Do nothing if ime is not specified */
    if (!g_fIME)
    {
	ErrorF ("winimeProcQueryTree - IME is not specified, returning.\n");
	return iReturn;
    }

    /* If the ime client has already been started, abort */
    if (g_fIMELaunched)
    {
	ErrorF ("winimeProcQueryTree - IME client already launched, returning.\n");
	return iReturn;
    }

    /* Startup the clipboard client if clipboard mode is being used */
    if (g_fXdmcpEnabled && g_fIME)
    {
	/*
	 * NOTE: The clipboard client is started here for a reason:
	 * 1) Assume you are using XDMCP (e.g. XWin -query %hostname%)
	 * 2) If the clipboard client attaches during X Server startup,
	 *    then it becomes the "magic client" that causes the X Server
	 *    to reset if it exits.
	 * 3) XDMCP calls KillAllClients when it starts up.
	 * 4) The clipboard client is a client, so it is killed.
	 * 5) The clipboard client is the "magic client", so the X Server
	 *    resets itself.
	 * 6) This repeats ad infinitum.
	 * 7) We avoid this by waiting until at least one client (could
	 *    be XDM, could be another client) connects, which makes it
	 *    almost certain that the clipboard client will not connect
	 *    until after XDM when using XDMCP.
	 * 8) Unfortunately, there is another problem.
	 * 9) XDM walks the list of windows with XQueryTree,
	 *    killing any client it finds with a window.
	 * 10)Thus, when using XDMCP we wait until the first call
	 *    to ProcQueryTree before we startup the clipboard client.
	 *    This should prevent XDM from finding the clipboard client,
	 *    since it has not yet created a window.
	 * 11)Startup when not using XDMCP is handled in
	 *    winimeProcEstablishConnection.
	 */

	/* Create the ime client thread */
	if (!winInitImServer ())
	{
	    ErrorF ("winimeProcQueryTree - winInitImServer failed.\n");
	    return iReturn;
	}

	ErrorF ("winimeProcQueryTree - winInitImServer returned.\n");
    }
  
    /* Flag that clipboard client has been launched */
    g_fIMELaunched = TRUE;

    return iReturn;
}

/*
 * Wrapper for internal EstablishConnection function.
 * Initializes internal clients that must not be started until
 * an external client has connected.
 */

int
winimeProcEstablishConnection (ClientPtr client)
{
    int				iReturn;
    static int			s_iCallCount = 0;
    static unsigned long	s_ulServerGeneration = 0;

    ErrorF ("winimeProcEstablishConnection - Hello\n");

    /* Do nothing if clipboard is not enabled */
    if (!g_fIME)
    {
	ErrorF ("winimeProcEstablishConnection - IME is not enabled, returning.\n");

	/* Unwrap the original function, call it, and return */
	InitialVector[2] = winimeProcEstablishConnectionOrig;
	iReturn = (*winimeProcEstablishConnectionOrig) (client);
	winimeProcEstablishConnectionOrig = NULL;
	return iReturn;
    }

    /* Watch for server reset */
    if (s_ulServerGeneration != serverGeneration)
    {
	/* Save new generation number */
	s_ulServerGeneration = serverGeneration;

	/* Reset call count */
	s_iCallCount = 0;
    }

    /* Increment call count */
    ++s_iCallCount;

    /* Wait for second call when Xdmcp is enabled */
    if (g_fXdmcpEnabled && !g_fIMELaunched && s_iCallCount < 4)
    {
	ErrorF ("winimeProcEstablishConnection - Xdmcp enabled, waiting to start clipboard client until fourth call.\n");
	return (*winimeProcEstablishConnectionOrig) (client);
    }

    /*
     * This procedure is only used for initialization.
     * We can unwrap the original procedure at this point
     * so that this function is no longer called until the
     * server resets and the function is wrapped again.
     */
    InitialVector[2] = winimeProcEstablishConnectionOrig;

    /*
     * Call original function and bail if it fails.
     * NOTE: We must do this first, since we need XdmcpOpenDisplay
     * to be called before we initialize our clipboard client.
     */
    iReturn = (*winimeProcEstablishConnectionOrig) (client);
    if (iReturn != 0)
    {
	ErrorF ("winimeProcEstablishConnection - ProcEstablishConnection failed, bailing.\n");
	return iReturn;
    }

    /* Clear original function pointer */
    winimeProcEstablishConnectionOrig = NULL;

    /* If the clipboard client has already been started, abort */
    if (g_fIMELaunched)
    {
	ErrorF ("winimeProcEstablishConnection - Clipboard client already launched, returning.\n");
	return iReturn;
    }

    /* Startup the clipboard client if clipboard mode is being used */
    if (g_fIME)
    {
	/*
	 * NOTE: The clipboard client is started here for a reason:
	 * 1) Assume you are using XDMCP (e.g. XWin -query %hostname%)
	 * 2) If the clipboard client attaches during X Server startup,
	 *    then it becomes the "magic client" that causes the X Server
	 *    to reset if it exits.
	 * 3) XDMCP calls KillAllClients when it starts up.
	 * 4) The clipboard client is a client, so it is killed.
	 * 5) The clipboard client is the "magic client", so the X Server
	 *    resets itself.
	 * 6) This repeats ad infinitum.
	 * 7) We avoid this by waiting until at least one client (could
	 *    be XDM, could be another client) connects, which makes it
	 *    almost certain that the clipboard client will not connect
	 *    until after XDM when using XDMCP.
	 * 8) Unfortunately, there is another problem.
	 * 9) XDM walks the list of windows with XQueryTree,
	 *    killing any client it finds with a window.
	 * 10)Thus, when using XDMCP we wait until the second call
	 *    to ProcEstablishCeonnection before we startup the clipboard
	 *    client.  This should prevent XDM from finding the clipboard
	 *    client, since it has not yet created a window.
	 */

	/* Create the clipboard client thread */
	if (!winInitImServer ())
	{
	    ErrorF ("winimeProcEstablishConnection - winInitImServer failed.\n");
	    return iReturn;
	}

	ErrorF ("winimeProcEstablishConnection - winInitImServer returned.\n");
    }

    /* Flag that clipboard client has been launched */
    g_fIMELaunched = TRUE;

    return iReturn;
}

#define LOCALEVENT_MAX 4

void
SendImeKey (void)
{
    xEvent	xCurrentEvent;
    CARD32	cur_time = 0;
    DWORD	dwKeyCode = 0;
    long	modifier = 0;
    int i;

    ErrorF("%s()\n", __FUNCTION__);

    ZeroMemory (&xCurrentEvent, sizeof (xCurrentEvent));

    cur_time = GetTickCount ();
    if (cur_time <= LOCALEVENT_MAX)	// LOCALEVENT_MAX
	cur_time = LOCALEVENT_MAX + 1;

    xCurrentEvent.u.keyButtonPointer.time =
	g_c32LastInputEventTime = cur_time;

    if (g_TriggerKeycode == 0)
    {
	winDebug("  Trigger key is not specified.\n");
	return;
    }

    xCurrentEvent.u.u.type = KeyPress;
    if (g_TriggerModifier & ShiftMask)
    {
	winDebug("  hit Shift\n");
/*
	xCurrentEvent.u.u.detail = 50;
	//mieqEnqueue (&xCurrentEvent);
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (KEY_ShiftL, TRUE);
    }

    if (g_TriggerModifier & ControlMask)
    {
	winDebug("  hit Control\n");
/*
	xCurrentEvent.u.u.detail = 37;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (KEY_LCtrl, TRUE);
    }

    if (g_TriggerModifier & AltMask)
    {
	winDebug("  hit Alt\n");
/*
	xCurrentEvent.u.u.detail = 64;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (KEY_Alt, TRUE);
    }

    {
/*
	xCurrentEvent.u.u.detail = g_TriggerKeycode;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
*/
	winSendKeyEvent (g_TriggerKeycode - MIN_KEYCODE, TRUE);
    }
/*
    cur_time = GetTickCount ();
    if (cur_time <= LOCALEVENT_MAX)	// LOCALEVENT_MAX
	cur_time = LOCALEVENT_MAX + 1;

    xCurrentEvent.u.keyButtonPointer.time =
	g_c32LastInputEventTime = cur_time;

    xCurrentEvent.u.u.type = KeyRelease;
*/
    {
/*
	xCurrentEvent.u.u.detail = g_TriggerKeycode;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (g_TriggerKeycode - MIN_KEYCODE, FALSE);
    }

    if (g_TriggerModifier & AltMask)
    {
/*
	xCurrentEvent.u.u.detail = 64;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
*/
	winSendKeyEvent (KEY_Alt, FALSE);
    }

    if (g_TriggerModifier & ControlMask)
    {
/*
	xCurrentEvent.u.u.detail = 37;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (KEY_LCtrl, FALSE);
    }

    if (g_TriggerModifier & ShiftMask)
    {
/*
	xCurrentEvent.u.u.detail = 50;
	DeviceIntPtr pDev;
	for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
	{
	    if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
	    {
		mieqEnqueue (pDev, &xCurrentEvent);
	    }
	}
*/
	winSendKeyEvent (KEY_ShiftL, FALSE);
    }
}

int
winimeImeOff(void)
{
    int nRet = 0;
    HIMC hIMC;
    BOOL fOpen;

    winDebug("%s()\n", __FUNCTION__);

    if (g_hwndLastKeyPress == 0)
    {
	winDebug("   g_hwndLastKeyPress is NULL\n");
	return nRet;
    }

    hIMC = ImmGetContext(g_hwndLastKeyPress);
    fOpen = ImmGetOpenStatus(hIMC);

    if (fOpen == TRUE)
    {
	int n = ImmSetOpenStatus(hIMC, FALSE);
	winDebug("   IME is open = %d\n", n);
    } else
    {
	winDebug("   IME is close\n");
    }

    return nRet;
}
