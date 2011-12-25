#ifndef lint
static char *rcsid = "$Id: dispatch.c,v 1.14 1994/05/31 07:48:42 ishisone Rel $";
#endif

/*
 * a very simple event dispatch library for non-widget windows
 *
 *	'non-widget window' means windows that have no associated widget,
 *	e.g. windows created by Xlib directly.
 */

/*
 * Copyright (c) 1990  Software Research Associates, Inc.
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
 */

#include <X11/Intrinsic.h>
#include "MyDispatch.h"
#include "AsyncErr.h"

#ifdef USE_WINIME
#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <X11/Xfuncs.h>
#include <X11/Xos.h>

#include "X11/Xproto.h"
#define ATOM			DWORD
#include "winms.h"

typedef unsigned short	wchar;
#include <X11/extensions/winime.h>

#include "imlib/im.h"
extern int convJWStoCT();
#endif

#define DEBUG_VAR debug_dispatch
#include "DebugPrint.h"

typedef struct _handler_ {
    int			type;		/* event type */
    unsigned long	mask;		/* event mask */
    void		(*handler)();
    XtPointer		data;
    struct _handler_	*next;
} HandlerRec;

typedef struct {
    Boolean		dispatching;	/* now dispatching */
    Boolean		toberemoved;	/* this list is to be removed later */
    unsigned long	mask;		/* event mask */
    HandlerRec		*handlers;
} WindowRec;

static int Initialized = 0;
static XContext Context;

static void
initialize()
{
    Context = XUniqueContext();
    Initialized = 1;
}

static void
resetEventMask(dpy, window, wp)
Display *dpy;
Window window;
WindowRec *wp;
{
    register HandlerRec *hp = wp->handlers;
    register unsigned long mask = 0L;

    while (hp != NULL) {
	mask |= hp->mask;
	hp = hp->next;
    }

    if (mask != wp->mask) {
	XAEHandle h = XAESetIgnoreErrors(dpy);	/* keep the operation safe */
	XSelectInput(dpy, window, mask);
	XAEUnset(h);
	wp->mask = mask;
    }
}

static void
removeAll(dpy, window, wp)
Display *dpy;
Window window;
WindowRec *wp;
{
    register HandlerRec *hp = wp->handlers;

    while (hp != NULL) {
	register HandlerRec *hp0 = hp;
	hp = hp->next;
	XtFree((char *)hp0);
    }

    if (wp->mask != 0L) {
	XAEHandle h = XAESetIgnoreErrors(dpy);

	/* keep it safe (because the window might not exist any more) */
	XSelectInput(dpy, window, 0L);
	XAEUnset(h);
    }

    XtFree((char *)wp);
    (void)XDeleteContext(dpy, window, Context);
}

static void
doDispatch(event, list)
XEvent *event;
register HandlerRec *list;
{
    void (*handler)();
    XtPointer data;
    register int type = event->type;
#ifdef USE_WINIME
TRACE(("* doDispatch *\n"));
#endif

    /*
     * we must be careful here. the invoked handler might remove
     * itself, or remove other handler to be invoked next.
     * so we use this somewhat strange recursive call.
     */
    while (list != NULL) {
	if (list->type == type) {
	    handler = list->handler;
	    data = list->data;
	    doDispatch(event, list->next);
	    (*handler)(event, data);
	    return;
	}
	list = list->next;
    }
}

void
MyDispatchEvent(event)
XEvent *event;
{
    caddr_t data;

#ifdef USE_WINIME
{
XKeyEvent* kev = (XKeyEvent*)event;

if (event->type == KeyPress)
	TRACE(("* MyDispatchEvent *: (type = KeyPress, keycode = 0x%X)\n", kev->keycode));
else if (event->type == KeyRelease)
	TRACE(("* MyDispatchEvent *: (type = KeyRelease, keycode = 0x%X)\n", kev->keycode));
else
	TRACE(("* MyDispatchEvent *: (type = %d)\n", event->type));
}
#endif
    if (!Initialized) initialize();

#ifdef USE_WINIME
    pthread_testcancel();
#endif
    if (!XFindContext(event->xany.display, event->xany.window,
		      Context, &data)) {
	WindowRec *wrec = (WindowRec *)data;

	wrec->dispatching = True;
	wrec->toberemoved = False;

	doDispatch(event, wrec->handlers);

	wrec->dispatching = False;
	if (wrec->toberemoved) {
	    removeAll(event->xany.display, event->xany.window, wrec);
	}
#ifdef USE_WINIME
    } else
    {
TRACE((" no context found.\n"));
    }
#else
    }
#endif
}

void
MyAddEventHandler(dpy, window, type, mask, func, data)
Display *dpy;
Window window;
int type;
unsigned long mask;
void (*func)();
XtPointer data;
{
    WindowRec *wp;
    HandlerRec *hp;
    caddr_t cdata;

    TRACE(("MyAddEventHandler(window=%08lx,type=%d,dpy=0x%08X)\n", window, type, dpy));
    if (!Initialized) initialize();

    hp = XtNew(HandlerRec);
    hp->type = type;
    hp->mask = mask;
    hp->handler = func;
    hp->data = data;
    hp->next = NULL;

    if (!XFindContext(dpy, window, Context, &cdata)) {
	wp = (WindowRec *)cdata;
	hp->next = wp->handlers;
	wp->handlers = hp;
    } else {
	wp = XtNew(WindowRec);
	wp->mask = 0L;
	wp->dispatching = False;
	wp->handlers = hp;
	(void)XSaveContext(dpy, window, Context, (caddr_t)wp);
    }
    resetEventMask(dpy, window, wp);
}

void
MyRemoveEventHandler(dpy, window, type, func, data)
Display *dpy;
Window window;
int type;
void (*func)();
XtPointer data;
{
    caddr_t cdata;
    WindowRec *wp;
    HandlerRec *hp, *hp0;

    TRACE(("MyRemoveEventHandler(window=%08lx,type=%d)\n", window, type));
    if (!Initialized) initialize();
    if (XFindContext(dpy, window, Context, &cdata) || cdata == NULL) return;

    wp = (WindowRec *)cdata; 
    hp0 = NULL;
    hp = wp->handlers;

    while (hp != NULL) {
	if (hp->type == type && hp->handler == func && hp->data == data) {
	    HandlerRec *tmp = hp;

	    hp = hp->next;
	    if (hp0 == NULL) {
		wp->handlers = hp;
	    } else {
		hp0->next = hp;
	    }
	    XtFree((char *)tmp);
	} else {
	    hp0 = hp;
	    hp = hp->next;
	}
    }

    resetEventMask(dpy, window, wp);
    
    if (wp->handlers == NULL) {
	if (wp->dispatching) {
	    /* now dispatching. we just mark it to be removed later. */
	    wp->toberemoved = True;
	} else {
	    XtFree((char *)wp);
	    (void)XDeleteContext(dpy, window, Context);
	}
    }
}

void
MyRemoveAllEventHandler(dpy, window)
Display *dpy;
Window window;
{
    caddr_t cdata;
    WindowRec *wp;

    TRACE(("MyRemoveAllEventHandler(window=%08lx)\n", window));
    if (!Initialized) initialize();

    if (XFindContext(dpy, window, Context, &cdata) || cdata == NULL) return;

    wp = (WindowRec *)cdata;

    if (wp->dispatching) {
	/* now dispatching. we just mark it to be removed later. */
	wp->toberemoved = True;
	return;
    } else {
	removeAll(dpy, window, wp);
    }
}


#ifdef USE_WINIME
static void IMPreeditStart(XWinIMENotifyEvent *ev)
{
//    caddr_t data;
//    WindowRec *wrec;
    IMIC *icp;
    int nRet;
    XKeyEvent keyevent;

TRACE(("IMPreeditStart()\n"));
    if (!Initialized) initialize();

/*
    if (XFindContext(ev->display, ev->window, Context, &data))
    {
	TRACE(("  Can't Find context.\n"));
	return;
    }
    wrec = (WindowRec *)data;
*/

    icp = IMContext2IMIC(ev->context);
    if (icp->state & IC_CONVERTING) return;

    nRet = IMStartConversion(icp);

//    IMSendRequestWithIC(icp->im->connection, XIM_SYNC, 0, icp);

#if 0
    /* make a fake keypress event */
    keyevent.type = KeyPress;
    keyevent.serial = 0;	/* 何をいれよう Y.Arai */
    keyevent.send_event = True;
    keyevent.display = ev->display;
    keyevent.window = ev->window;
    keyevent.root = DefaultRootWindow(ev->display);
			/* hope conversion object won't check this field */
    keyevent.subwindow = None;
			/* hope conversion object won't check this field */
    keyevent.time = 4;	/* hope conversion object won't check this field */
    keyevent.x = 0;
    keyevent.y = 0;
    keyevent.x_root = 0;
    keyevent.y_root = 0;
    keyevent.state = 0;		/* 何をいれよう Y.Arai */
    keyevent.keycode = 0;	/* 何をいれよう Y.Arai */
    keyevent.same_screen = True;

    IMForwardEvent(icp, (XEvent *)&keyevent);
#endif
}

static void IMPreeditStop(XWinIMENotifyEvent *ev)
{
//    caddr_t data;
//    WindowRec *wrec;
    IMIC *icp;
//    int nRet;
    XKeyEvent keyevent;

TRACE(("IMPreeditStop()\n"));
    if (!Initialized) initialize();

/*
    if (XFindContext(ev->display, ev->window, Context, &data))
    {
	TRACE(("  Can't Find context.\n"));
	return;
    }
    wrec = (WindowRec *)data;
*/

    icp = IMContext2IMIC(ev->context);

    if (!(icp->state & IC_CONVERTING)) return;

//    IMStopConversion(icp);
//    IMDestroyIC(icp);
    /* make a fake keypress event */
    keyevent.type = KeyPress;
    keyevent.serial = 0;	/* 何をいれよう Y.Arai */
    keyevent.send_event = True;
    keyevent.display = ev->display;
    keyevent.window = ev->window;
    keyevent.root = DefaultRootWindow(ev->display);
			/* hope conversion object won't check this field */
    keyevent.subwindow = None;
			/* hope conversion object won't check this field */
    keyevent.time = 2;	/* hope conversion object won't check this field */
    keyevent.x = 0;
    keyevent.y = 0;
    keyevent.x_root = 0;
    keyevent.y_root = 0;
    keyevent.state = 0;		/* 何をいれよう Y.Arai */
    keyevent.keycode = 0;	/* 何をいれよう Y.Arai */
    keyevent.same_screen = True;

    IMForwardEvent(icp, (XEvent *)&keyevent);

//    IMSendRequestWithIC(icp->im->connection, XIM_SYNC, 0, icp);
#if 0
    /*
     * Currently, kinput2 supports only full synchronous method.
     */
    if (!(icp->state & IC_CONVERTING)) {
	/*
	 * Stop forwarding key events.
	 */
	IMStopForwarding(icp);
    }
#endif
}

static void IMForwardInputObj(XWinIMENotifyEvent *ev)
{
    IMIC *icp;
    XKeyEvent keyevent;

TRACE(("IMForwardInputObj()\n"));
    if (!Initialized) initialize();

    icp = IMContext2IMIC(ev->context);

    /* make a fake keypress event */
    keyevent.type = KeyPress;
    keyevent.serial = 0;	/* 何をいれよう Y.Arai */
    keyevent.send_event = True;
    keyevent.display = ev->display;
    keyevent.window = ev->window;
    keyevent.root = DefaultRootWindow(ev->display);
			/* hope conversion object won't check this field */
    keyevent.subwindow = None;
			/* hope conversion object won't check this field */
    keyevent.time = 0;	/* hope conversion object won't check this field */
    keyevent.x = 0;
    keyevent.y = 0;
    keyevent.x_root = 0;
    keyevent.y_root = 0;
    keyevent.state = 0;		/* 何をいれよう Y.Arai */
    keyevent.keycode = 0;	/* 何をいれよう Y.Arai */
    keyevent.same_screen = True;

    IMForwardEvent(icp, (XEvent *)&keyevent);

//    IMSendRequestWithIC(icp->im->connection, XIM_SYNC, 0, icp);

}

static void IMCallCallback(XWinIMENotifyEvent *ev)
{
/*
    caddr_t data;
    WindowRec *wrec;
    IMIC *icp;

    if (!Initialized) initialize();

    if (XFindContext(ev->display, ev->window, Context, &data))
    {
	TRACE(("  Can't Find context.\n"));
	return;
    }

    wrec = (WindowRec *)data;

    icp = IMContext2IMIC(ev->context);
*/
}

#if 1
static void IMCommitConversion(XWinIMENotifyEvent *ev)
{
    IMIC *icp;
    XKeyEvent keyevent;

TRACE(("IMCommitConversion()\n"));
    if (!Initialized) initialize();

    icp = IMContext2IMIC(ev->context);

    /* make a fake keypress event */
    keyevent.type = KeyPress;
    keyevent.serial = 0;	/* 何をいれよう Y.Arai */
    keyevent.send_event = True;
    keyevent.display = ev->display;
    keyevent.window = ev->window;
    keyevent.root = DefaultRootWindow(ev->display);
			/* hope conversion object won't check this field */
    keyevent.subwindow = None;
			/* hope conversion object won't check this field */
    keyevent.time = 1;	/* hope conversion object won't check this field */
    keyevent.x = 0;
    keyevent.y = 0;
    keyevent.x_root = 0;
    keyevent.y_root = 0;
    keyevent.state = 0;		/* 何をいれよう Y.Arai */
    keyevent.keycode = 0;	/* 何をいれよう Y.Arai */
    keyevent.same_screen = True;

    IMForwardEvent(icp, (XEvent *)&keyevent);

//    IMSendRequestWithIC(icp->im->connection, XIM_SYNC, 0, icp);
}
#else
#define STRING_BUFFER_SIZE 1024
static void IMCommitConversion(XWinIMENotifyEvent *ev)
{
    caddr_t data;
    WindowRec *wrec;
    int len;
    static wchar szCompositionString[STRING_BUFFER_SIZE];
    IMIC *icp;
    XtPointer string;

    if (!Initialized) initialize();

/*
    if (XFindContext(ev->display, ev->window, Context, &data))
    {
	TRACE(("  Can't Find context.\n"));
	return;
    }
*/

    wrec = (WindowRec *)data;

    icp = IMContext2IMIC(ev->context);

    len = XWinIMEGetCompositionString(ev->display, ev->context, WinIMECMPCompStr, STRING_BUFFER_SIZE, szCompositionString);
    szCompositionString[len] = (wchar)0;
    if (len < 0)
    {
	TRACE(("  XWinIMEGetCompositionString failed.\n"));
	return;
    }

    // wchar -> compound text
    len = convJWStoCT(szCompositionString, (unsigned char *)NULL, 0);
    string = XtMalloc(len + 1);
    (void)convJWStoCT(szCompositionString, (unsigned char *)string, 0);

    IMCommitString(icp, string, len, 0);
}
#endif

static void IMOpenCandidate(XWinIMENotifyEvent *ev)
{
    IMIC *icp;
    XKeyEvent keyevent;

TRACE(("IMOpenCandidate()\n"));
    if (!Initialized) initialize();

    icp = IMContext2IMIC(ev->context);

    /* make a fake keypress event */
    keyevent.type = KeyPress;
    keyevent.serial = 0;	/* 何をいれよう Y.Arai */
    keyevent.send_event = True;
    keyevent.display = ev->display;
    keyevent.window = ev->window;
    keyevent.root = DefaultRootWindow(ev->display);
			/* hope conversion object won't check this field */
    keyevent.subwindow = ev->arg;	// リストの番号
			/* hope conversion object won't check this field */
    keyevent.time = 3;	/* hope conversion object won't check this field */
    keyevent.x = 0;
    keyevent.y = 0;
    keyevent.x_root = 0;
    keyevent.y_root = 0;
    keyevent.state = 0;		/* 何をいれよう Y.Arai */
    keyevent.keycode = 0;	/* 何をいれよう Y.Arai */
    keyevent.same_screen = True;

    IMForwardEvent(icp, (XEvent *)&keyevent);

//    IMSendRequestWithIC(icp->im->connection, XIM_SYNC, 0, icp);
}

// XWINIME用のイベントディスパッチャ
/// IMICがらみは不必要なはずなのではずす
int
winXIMEXEventHandler (Display * pDisplay, XEvent * event, int ime_event_base, int ime_error_base)
{
    int i;
    int nRet = 0;	// Trueならここで処理済み
//    XClientMessageEvent *ev = (XClientMessageEvent *)event;

    TRACE (("* %s * (%d)\n", __FUNCTION__, event->type - ime_event_base));

    pthread_testcancel();

    if (event->type == ime_event_base + WinIMEControllerNotify)
    {
	nRet = 1;
	XWinIMENotifyEvent *ime_event = (XWinIMENotifyEvent *) event;
///	IMIC *pIC;

///	pIC = FindIMICbyContext (ime_event->context);

	switch (ime_event->kind)
	{
	    case WinIMEOpenStatus:
	    {
		TRACE (("WinIMEOpenStatus %d %s\n", ime_event->context,
			ime_event->arg ? "Open" : "Close"));

		if (ime_event->arg)
		{
		    IMPreeditStart (ime_event);
		} else
		{
		    IMPreeditStop (ime_event);
		}

	    }
		break;

	    case WinIMEComposition:
	    {
		TRACE (("WinIMEComposition %d %d\n", ime_event->context,
			ime_event->arg));

		switch (ime_event->arg)
		{
		    case WinIMECMPCompStr:
		    {
			IMForwardInputObj (ime_event);
		    }
			break;

		    case WinIMECMPResultStr:
		    {
			IMCommitConversion (ime_event);
//			IMForwardInputObj(ime_event);	// 実験
		    }
			break;

		    default:
			break;
		}
	    }
		break;

	    case WinIMEStartComposition:
	    {
		TRACE (("WinIMEStartComposition %d\n", ime_event->context));

		IMCallCallback (ime_event);
	    }
		break;

	    case WinIMEEndComposition:
	    {
		TRACE (("WinIMEEndComposition %d\n", ime_event->context));

///		if (!pIC)
///		{
///		    TRACE (("context %d IC is not found", ime_event->context));
///		    break;
///		}


///		if (pIC->length > 0)
///		{
///		    XIMText text;
///		    XIMFeedback aFeedback[1] = { 0 };
///
///		    text.length = 0;
///		    text.string.multi_byte = "";
///		    text.feedback = aFeedback;
///
///		    /* Clear preedit. */
///		    pIC->pCallData.major_code = XIM_PREEDIT_DRAW;
///		    pIC->pCallData.preedit_callback.icid = pIC->id;
///		    pIC->pCallData.preedit_callback.todo.draw.caret = 0;
///		    pIC->pCallData.preedit_callback.todo.draw.chg_first = 0;
///		    pIC->pCallData.preedit_callback.todo.draw.chg_length = pIC->length;
///		    pIC->pCallData.preedit_callback.todo.draw.text = &text;

///		    pIC->length = 0;

//		    IMCallCallback (pIms, (XPointer) & pIC->pCallData);
///		    IMCallCallback (ime_event);
///		}

///		pIC->pCallData.major_code = XIM_PREEDIT_DONE;
//		IMCallCallback (pIms, (XPointer) & pIC->pCallData);
		IMCallCallback (ime_event);
	    }
		break;

	    case WinIMEOpenCand:
	    {
		TRACE (("WinIMEStartComposition %d\n", ime_event->context));

		IMOpenCandidate (ime_event);
	    }
		break;

	    default:
TRACE(("Other Message\n"));

		break;
	}
    }
    return nRet;
}

#endif
