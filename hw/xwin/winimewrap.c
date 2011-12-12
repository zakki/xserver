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
#include <windows.h>
#undef XFree86Server

/* for WINIME extension */
#define wchar wchar_t
#include <X11/extensions/winime.h>
#include <X11/extensions/winimestr.h>
#include "winkeynames.h"
#include <imm.h>

#ifndef USE_XWIN_FULLEXTENSION
#include <iconv.h>
#endif

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
//extern Display*			g_pImServerDpy;
//extern Window			g_imServerWindow;

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

int
winimeSendDummyEvent(void)
{
    winWinIMESendEvent(WinIMEControllerNotify, WinIMENotifyMask, WinIMEDummyEvent, 0, 0, NULL);

    return 1;
#if 0	// $BE~Ce$;$:(B
    XClientMessageEvent ev;
    Display *dpy = g_pImServerDpy;
    Window wind = g_imServerWindow;

    ev.display = dpy;
    ev.window = wind;
    ev.type = ClientMessage;
    ev.message_type = XInternAtom(dpy, "_MY_PROTOCOLS", False);
    ev.format = 32;

    return XSendEvent(dpy, wind, FALSE, NoEventMask, (XEvent*)&ev);
#endif
}

#define LOCALEVENT_MAX 4

void
SendImeKey (void)
{
  xEvent	xCurrentEvent;
  CARD32	cur_time = 0;
  DWORD		dwKeyCode = 0;
  long		modifier = 0;
#if 0
  DWORD	ModKeys[8];
  XModifierKeymap	*modmap;
#endif
  int i;

ErrorF("%s()\n", __FUNCTION__);
//ErrorF("%s(), display = %d\n", __FUNCTION__, g_pImServerDpy);

#if 0	// XGetModifierMapping()$B8F$V$H5"$C$FMh$J$$$N$G$$$C$?$s$d$a$F7h$aBG$A$G$d$k(B
  modmap = XGetModifierMapping(g_pImServerDpy);
  if (modmap != NULL)
  {
    for (i=0; i<8; i++)
    {
//	ModKeys[i] = XKeysymToKeycode(g_pImServerDpy, ModKeySyms[i]);
ErrorF("  ModKeycode[%d] = %d\n", i, modmap->modifiermap[i]);
    }
    XFreeModifiermap(modmap);
  }
#endif

  ZeroMemory (&xCurrentEvent, sizeof (xCurrentEvent));

  cur_time = GetTickCount ();
  if (cur_time <= LOCALEVENT_MAX)	// LOCALEVENT_MAX
    cur_time = LOCALEVENT_MAX + 1;

  xCurrentEvent.u.keyButtonPointer.time =
    g_c32LastInputEventTime = cur_time;

#if 1
  if (g_TriggerKeycode == 0)
  {
    winDebug("  Trigger key is not specified.\n");
    return;
  }

  xCurrentEvent.u.u.type = KeyPress;
  if (g_TriggerModifier & ShiftMask)
  {
winDebug("  hit Shift\n");
    xCurrentEvent.u.u.detail = 50;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  if (g_TriggerModifier & ControlMask)
  {
winDebug("  hit Control\n");
    xCurrentEvent.u.u.detail = 37;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  if (g_TriggerModifier & AltMask)
  {
winDebug("  hit Alt\n");
    xCurrentEvent.u.u.detail = 64;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  {
    xCurrentEvent.u.u.detail = g_TriggerKeycode;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  cur_time = GetTickCount ();
  if (cur_time <= LOCALEVENT_MAX)	// LOCALEVENT_MAX
    cur_time = LOCALEVENT_MAX + 1;

  xCurrentEvent.u.keyButtonPointer.time =
    g_c32LastInputEventTime = cur_time;

  xCurrentEvent.u.u.type = KeyRelease;

  {
    xCurrentEvent.u.u.detail = g_TriggerKeycode;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  if (g_TriggerModifier & AltMask)
  {
    xCurrentEvent.u.u.detail = 64;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  if (g_TriggerModifier & ControlMask)
  {
    xCurrentEvent.u.u.detail = 37;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }

  if (g_TriggerModifier & ShiftMask)
  {
    xCurrentEvent.u.u.detail = 50;
    //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
  }
#else
  dwKeyCode = 50;
  xCurrentEvent.u.u.type = KeyPress;
  xCurrentEvent.u.u.detail = dwKeyCode;

  //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }

  dwKeyCode = 65;
  xCurrentEvent.u.u.detail = dwKeyCode;

  //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }

  cur_time = GetTickCount ();
  if (cur_time <= LOCALEVENT_MAX)	// LOCALEVENT_MAX
    cur_time = LOCALEVENT_MAX + 1;

  xCurrentEvent.u.keyButtonPointer.time =
    g_c32LastInputEventTime = cur_time;

  dwKeyCode = 65;
  xCurrentEvent.u.u.type = KeyRelease;
  xCurrentEvent.u.u.detail = dwKeyCode;

  //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }

  dwKeyCode = 50;
  xCurrentEvent.u.u.detail = dwKeyCode;

  //mieqEnqueue (&xCurrentEvent);
    DeviceIntPtr pDev;
    for (pDev = inputInfo.devices; pDev; pDev = pDev->next)
      if ((pDev->coreEvents && pDev != inputInfo.keyboard) && pDev->key)
        {
          mieqEnqueue (pDev, &xCurrentEvent);
        }
#endif
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

#ifndef USE_XWIN_FULLEXTENSION
extern void* ProcWinIMEGetCompositionString (int context, int index, int *strLength);
extern Bool ProcWinIMEGetCursorPosition (int context, int *cursor, int *numClause, int *curClause, int *offset);
extern Bool ProcWinIMEGetConversionStatus (int context, Bool* fopen, DWORD* conversion, DWORD* sentence, Bool* fmodechange);
extern void* ProcWinIMEGetTargetClause (int context, int target, int *attr, int *bytes, int *numchar, int *curClause);
extern void* ProcWinIMEGetTargetString (int context, int target, int offset, int *bytes, int *numchar);
extern int ProcWinIMESetCandidateWindow (int context, int x, int y, int n);

// >> 
/*
 * ICString.attr $B$NCM(B
 */

/* 1. $BJ8;zNs$,JQ49ESCf$N%F%-%9%H%;%0%a%s%H$rI=$7$F$$$k;~(B
 * ($B2<5-$NCM$N(B bitwise-or)
 */
#define ICAttrNotConverted	0	/* $B$^$@JQ49$5$l$F$$$J$$%;%0%a%s%H(B */
#define ICAttrConverted		1	/* $BJQ49:Q$_%;%0%a%s%H(B */
#define ICAttrCurrentSegment	2	/* $B8=:_CmL\$7$F$$$k%;%0%a%s%H(B */
#define ICAttrCurrentSubSegment 4	/* $B8=:_CmL\$7$F$$$k%5%V%;%0%a%s%H(B */

/* 2. $B$=$l0J30!"Nc$($P8=:_$NF~NO%b!<%I$J$I$rI=$7$F$$$k;~(B ($B>o$K$3$NCM(B) */
#define ICAttrNormalString	(-1)
// << 

// Xatoklib$B$+$i<ZMQ(B
/*
 * [$B4X?tL>(B]
 *		euc2wcs( )
 * [$BI=Bj(B]
 *              EUC$BJ8;zNs$+$i(B wchar $B7?J8;zNs$X$NJQ49(B
 * [$B8F=P7A<0(B]
 *		int	euc2wcs( unsigned char *euc, int elen, wchar *wbuf )
 *
 * [$B0z?t(B]
 *              $B7?(B            : $BL>(B  $B>N(B  : I O : $B@b(B      $BL@(B
 *		unsigned char : *euc   : i   : EUC $BJ8;zNs(B
 *		int           : elen   : i   : EUC $BJ8;zNs$ND9$5(B
 *		wchar         : *wbuf  :   o : wchar $B7?J8;zNs3JG<NN0h(B
 *
 * [$BJV$jCM(B]
 *		1 : $B>o$K#1(B
 *
 * [$B;HMQ4X?t(B]
 *		$B$J$7(B
 * [$B5!G=(B]
 *		unsigined char $B7?$N(BEUC $BJ8;zNs$r(Bwchar $B7?$KJQ49$9$k!#(B
 *		EUC $BJ8;zNs$K$O!"(B0x8f $B$NFCJL$J%3!<%I$,4^$^$l$F$$$k$N$G(B
 *		wchar $B$KJQ49$9$k;~$K8DJL=hM}$r$9$k!#(B
 */
static int euc2wcs(euc, elen, wbuf)
unsigned char	*euc;
int		elen;
wchar		*wbuf;
{
	int	lb = 0, hb = 0 ;
	int	i ;
	int	n = 0 ;
	int	isSkip ;

	for( i = 0 ; i < elen ; i++ ) {
		isSkip = 0 ;
		if (  *euc == 0x8e ) {
			euc++ ;
			hb = *euc ;
			lb = 0 ;
			i++ ;
		}
		else if (  *euc & 0x80 ) {
			if ( *euc == 0x8f ) {
				isSkip=1 ;
			}
			else {
				lb = *euc ;
				euc++ ;
				hb = *euc ;
				i++ ;
			}
		}
		else {
			hb = *euc ;
			lb = 0 ;
		}
		euc++ ;
		if ( !isSkip ) {
			*wbuf = (( lb << 8 ) | hb ) & 0xffff ;
			wbuf++ ;
			n++ ;
		}
	}

	*wbuf = 0 ;
	return n ;
}

static int
_Local_wcstombs(
    iconv_t cd,
    const wchar *str,
    int flen,
    char *ustr,
    int len)
{
    XPointer from, to;
    int from_left, to_left, ret, status;

winDebug("  _Local_wcstombs()\n");
    from = (XPointer) str;
    from_left = flen;
    to = (XPointer) ustr;
    to_left = len;

    while(from_left > 0)
    {
winDebug("  from_left = %d byte\n", from_left);
	status = iconv(cd, (const char **)&from, &from_left, &to, &to_left);
	if (status < 0)
	{
	    /* $BI=8=$G$-$J$$J8;z$,$-$?$N$G(B'$B!)(B'$B$GKd$a$k(B */
winDebug("    from = 0x%X to ?\n", *from);
	    *to++ = 0xA1;
	    *to++ = 0xA9;
	    from += 2;
	    from_left -= 2;
	    to_left -= 2;
	}
    }

    ret = len - to_left;
winDebug("    converted length = %d\n", ret);
    if (ustr && to_left > 0)
	ustr[ret] = '\0';

{
int i;
winDebug("    euc strings:\n");
for (i=0; i<ret; i++)
	winDebug("      0x%X", ustr[i]);
winDebug("\n");
}

    return ret;
}

#define SELECTBUFSIZE 1024
// $B0l2s(BEUC$B$KJQ49$7$F$+$i(BWideChar$B$KJQ49$9$k%P!<%8%g%s(B
static int UCS2toWideChar(int nSize, wchar *pwszUnicodeStr, wchar* pDest)
{
    static wchar wszWideChar[SELECTBUFSIZE];
    static char szString[SELECTBUFSIZE];
    iconv_t cd = (iconv_t)-1;
    int nLen = -1;	// $BJ8;z?t(B

    char		*language;
    char		*territory;
    char		*codeset;

winDebug("WinIME:UCS2toWideChar(%d Bytes), pDest = 0x%08X\n", nSize, pDest);

    // $B%3!<%IJQ49%3%s%P!<%?$N=`Hw(B
    cd = iconv_open("EUC-JP", "UCS-2-INTERNAL");
    if (cd == (iconv_t)-1)
    {
	winDebug("  No Converter\n");
	return nLen;
    }

    nLen = _Local_wcstombs(cd, pwszUnicodeStr, nSize, szString, SELECTBUFSIZE);
    iconv_close(cd);

    nLen = euc2wcs(szString, nLen, wszWideChar);

    if (nLen == -1)
    {
	winDebug("  Convert Error\n");
    } else
    {
	winDebug("  Save data, pDest = 0x%08X, wszWideChar = 0x%08X, nLen = %d, wchar = %d\n", pDest, wszWideChar, nLen, sizeof(wchar));

	memcpy((XPointer)pDest, (XPointer)wszWideChar, nLen * sizeof(wchar));
{
	int i;
	winDebug("    WideChar: ");
	for (i=0; i<nLen; i++)
	{
		winDebug("0x%X ", wszWideChar[i]);
	}
	winDebug("\n");
}
    }
    return nLen;
}

// UCS2$B$G;}$D$h$&$K$7$?$N$GBP1~(B Y.Arai
// $BEO$9J8;zNs$N%U%)!<%^%C%H$r!"(BCompound Text $B$+$i(B WideChar $B$KJQ99(B
// $BLa$jCM(B: $BJ8;z?t(B
int
XWinIMEGetCompositionString (void *dpy, int context,
			     int index,
			     int count,
			     wchar* str_return)
{
    char *str;
    int nLen, strLength = 0;
    int nContext;
    void* pVal;

    winDebug("GetCompositionString...\n");

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);
//    str_return = NULL;

    pVal = (wchar*)ProcWinIMEGetCompositionString(nContext, index, &strLength);
winDebug("GetCompositionString: strLength = %d\n", strLength);
    if (pVal == NULL)
    {
        winDebug("GetCompositionString... return False\n");
	return -1;
    }

    str = (char *) malloc(strLength+2);
winDebug("GetCompositionString: str = 0x%08X\n", str);
    if (!str)
    {
        winDebug("GetCompositionString... return False\n");
	return -1;
    }

    memcpy((XPointer)str, (XPointer)pVal, strLength);
    str[strLength] = 0;
    str[strLength+1] = 0;

    // $B$3$3$G(BWideChar$B$XJQ49(B
    nLen = UCS2toWideChar(strLength, str, str_return);

    Xfree(str);

    winDebug("GetCompositionString... return True\n");

    return nLen;
}

Bool
XWinIMEGetCursorPosition (void* dpy, int context, int *cursor, int *numClause, int *curClause, int *offset)
{
    int nContext;
    Bool bRet;

    winDebug("GetCursorPosition...\n");

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);

    bRet = ProcWinIMEGetCursorPosition(nContext, cursor, numClause, curClause, offset);
    if (bRet == False)
    {
        winDebug("GetCursorPosition... return False\n");
    } else
    {
	winDebug("GetCursorPosition... return True\n");
    }
    return bRet;
}

Bool
XWinIMEGetConversionStatus (void *dpy, int context, Bool* fopen, DWORD* conversion, DWORD* sentence, Bool* fmodify)
{
    int nContext;
    Bool bRet;

    winDebug("GetConversionStatus...\n");

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);

    bRet = ProcWinIMEGetConversionStatus(nContext, fopen, conversion, sentence, fmodify);
    if (bRet)
    {
	winDebug("GetConversionStatus... return False\n");
    } else
    {
	winDebug("GetConversionStatus... return True\n");
    }
    return True;
}

// GetCompositionString$B$H0c$$!"(BUCS-2$B$G<u$1EO$7$9$k$3$H$K$9$k(B
/// UCS-2$B$O$d$a!"(BWideChar$B$G$d$k(B
// $BLa$jCM(B: $BJ8;z?t(B
int
XWinIMEGetTargetClause (void *dpy,
			int context,
			int target,
			wchar *data,
			int *attr)
{
    char *str;
    int nLen;
    int nContext;
    int nAttr = 0;
    int bytes = 0;
    int numchar = 0;
    int curClause = 0;
    void* pVal;

    winDebug("GetTargetClause...\n");
    *attr = ICAttrNormalString;

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);

    pVal = ProcWinIMEGetTargetClause (nContext, target, &nAttr, &bytes, &numchar, &curClause);
    if (pVal == NULL)
    {
	winDebug("GetTargetClause... return False\n");
	data = NULL;
	return -1;
    }

    if ((str = (char *) malloc(bytes + 2)))
    {
winDebug("  %d bytes.\n", bytes);
	memcpy((XPointer)str, (XPointer)pVal, bytes);
	str[bytes] = 0;
	str[bytes+1] = 0;
    } else
    {
winDebug("  malloc error.\n");
	data = NULL;
	return -1;
    }

    // $B$3$3$G(BWideChar$B$XJQ49(B
    nLen = UCS2toWideChar(bytes, str, data);

    Xfree(str);

winDebug("    check attr: target = %d, current = %d\n", target, curClause);
    switch(nAttr)
    {
	case ATTR_INPUT:
winDebug("    attr = ATTR_INPUT\n");
	    *attr = ICAttrNotConverted;
	    break;
	case ATTR_TARGET_CONVERTED:
winDebug("    attr = ATTR_TARGET_CONVERTED\n");
	    *attr = ICAttrConverted | ICAttrCurrentSegment;
	    break;
	case ATTR_CONVERTED:
winDebug("    attr = ATTR_CONVERTED\n");
	    *attr = ICAttrConverted;
	    break;
	case ATTR_TARGET_NOTCONVERTED:
winDebug("    attr = ATTR_TARGET_NOTCONVERTED\n");
	    *attr = ICAttrConverted | ICAttrCurrentSegment;	// ICAttrNotConverted -> ICAttrConverted, from A.Yamanaka
	    break;
	default:
winDebug("WinIME:    attr = ( %d )\n", nAttr);
	    *attr = ICAttrNotConverted;
	    break;
    }

    winDebug("GetTargetClause... return True\n");

    return nLen;
}

// $B;XDj$5$l$?(Btarget$B$N(Boffset$B0J9_!"A4JT=8J8;zNs$N:G8e$^$GJV$9(B
// offset: $BJ8;z?t(B
// $BLa$jCM(B: $BJ8;z?t(B
int
XWinIMEGetTargetString (void *dpy,
			int context,
			int target,
			int offset,
			wchar *data)
{
    char *str;
    int nLen;
    int nContext;
    int bytes = 0;
    int numchar = 0;
    void* pVal;

    winDebug("GetTargetString...\n");

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);

    pVal = ProcWinIMEGetTargetString (nContext, target, offset, &bytes, &numchar);
    if (pVal == NULL)
    {
	winDebug("GetTargetString... return False\n");
	return -1;
    }

    if ((str = (char *) malloc(bytes + 2)))
    {
	memcpy((XPointer)str, (XPointer)pVal, bytes);
	str[bytes] = 0;
	str[bytes+1] = 0;
    } else
    {
	data = NULL;
	return -1;
    }

    // $B$3$3$G(BWideChar$B$XJQ49(B
    nLen = UCS2toWideChar(bytes, str, data);

    Xfree(str);

    winDebug("GetTargetString... return True\n");

    return nLen;
}

Bool
XWinIMESetCandidateWindow (Display *dpy, int context, int x, int y, int listnum)
{
    int nContext;
    winDebug("IMESetCandidateWindow...\n");

    if (context != -1)
	nContext = context;
    else
	XWinIMEGetLastContext(dpy, &nContext);

    ProcWinIMESetCandidateWindow (nContext, x, y, listnum);
    winDebug("IMESetCandidateWindow... return True\n");
    return True;
}
#endif
