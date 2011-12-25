#ifndef lint
static char *rcsid = "$Id: imconv.c,v 1.25 2002/01/24 09:07:19 ishisone Exp $";
#endif
/*
 * Copyright (c) 1991, 1994  Software Research Associates, Inc.
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

#define COMMIT_SYNC
#define STATUS_SYNC
#include "im.h"
#include "ConvMgr.h"
#include "OverConv.h"
#include "OffConv.h"
#include "OnConv.h"
#include "InputConv.h"

#ifdef USE_WINIME
extern int winim_clients;
#endif
static void fillDefaultAttributesForStartup _Pt_((IMIC *icp));
static unsigned long makeConvAttributesForStartup _Pt_((IMIC *icp,
							ConversionAttributes *attrp));
static void commitString _Pt_((IMIC *icp, char *str, int len, int sync));
static void fixCallback _Pt_((Widget w, XtPointer client_data,
			      XtPointer call_data));
static void detachConverter _Pt_((IMIC *icp));
static void endCallback _Pt_((Widget w, XtPointer client_data,
			      XtPointer call_data));
static void unusedEventCallback _Pt_((Widget w, XtPointer client_data,
				      XtPointer call_data));
static void preeditStartCallback _Pt_((Widget w, XtPointer client_data,
				       XtPointer call_data));
static void preeditDoneCallback _Pt_((Widget w, XtPointer client_data,
				      XtPointer call_data));
static void preeditDrawCallback _Pt_((Widget w, XtPointer client_data,
				      XtPointer call_data));
static void preeditCaretCallback _Pt_((Widget w, XtPointer client_data,
				       XtPointer call_data));
static void statusStartCallback _Pt_((Widget w, XtPointer client_data,
				      XtPointer call_data));
static void statusDoneCallback _Pt_((Widget w, XtPointer client_data,
				     XtPointer call_data));
static void statusDrawCallback _Pt_((Widget w, XtPointer client_data,
				     XtPointer call_data));
static void preeditStart _Pt_((IMIC *icp));
static void preeditDone _Pt_((IMIC *icp));
static void preeditDraw _Pt_((IMIC *icp, OCCPreeditDrawArg *data));
static void preeditCaret _Pt_((IMIC *icp, int caret));
static void statusStart _Pt_((IMIC *icp));
static void statusDone _Pt_((IMIC *icp));
static void statusDraw _Pt_((IMIC *icp, OCCPreeditDrawArg *data));
static void setEventMask _Pt_((IMIC *icp, unsigned long forward_mask,
			       unsigned long synchronous_mask));


/*- fillDefaultAttributesForStartup: put default necessary for conv. start -*/
static void
fillDefaultAttributesForStartup(icp)
IMIC *icp;
{
    unsigned long cmask, pmask, smask;

    cmask = ATTR_MASK_FOCUS | ATTR_MASK_PREEDIT_STATE | ATTR_MASK_RESET_STATE;

    switch (icp->style) {
    case IMSTYLE_OVER_THE_SPOT:
	pmask = ATTR_MASK_FOREGROUND | ATTR_MASK_BACKGROUND |
	    ATTR_MASK_FONT_SET;
	smask = 0;
	break;
    case IMSTYLE_OFF_THE_SPOT:
	pmask = ATTR_MASK_FOREGROUND | ATTR_MASK_BACKGROUND |
	    ATTR_MASK_FONT_SET | ATTR_MASK_AREA;
	smask = ATTR_MASK_AREA;
	break;
    case IMSTYLE_ON_THE_SPOT:
	pmask = 0;
	smask = 0;
	break;
    default:
	pmask = 0;
	smask = 0;
    }
    IMFillDefault(icp, cmask, pmask, smask);
}

/*- makeConvAttributesForStartup: get conv. attrs needed for startup -*/
static unsigned long
makeConvAttributesForStartup(icp, attrp)
IMIC *icp;
ConversionAttributes *attrp;
{
    icp->common_attr.change_mask = icp->common_attr.set_mask;
    icp->preedit_attr.change_mask = icp->preedit_attr.set_mask;
    icp->status_attr.change_mask = icp->status_attr.set_mask;
    return IMMakeConvAttributes(icp, attrp);
}

/*- commitString: commmit converted string to client -*/
static void
commitString(icp, str, len, sync)
IMIC *icp;
char *str;
int len;
int sync;
{
    int offset;
    IMConnection *conn = icp->im->connection;
    unsigned int flag;

    TRACE(("imlib:commitString(len = %d, sync = %d)\n", len, sync));

#ifdef USE_WINIME
{
#else
    if (DDEBUG_CONDITION(5)) {
#endif
	unsigned char *p = (unsigned char *)str;
	int i;

	/*
	 * Dump commiting string.
	 */
#ifdef USE_WINIME
	TRACE(("* commit string:\n\t"));
	for (i = 0; i < len; i++, p++) {
	    if (*p == '\033') {
		TRACE(("ESC "));
	    } else if (*p < ' ') {
		TRACE(("^%c ", *p + '@'));
	    } else if (*p == ' ') {
		TRACE(("sp "));
	    } else if (*p >= 0x7f) {
		TRACE(("%x ", *p));
	    } else {
		TRACE(("%c ", *p));
	    }
	}
	TRACE(("\n"));
#else
	printf("* commit string:\n\t");
	for (i = 0; i < len; i++, p++) {
	    if (*p == '\033') {
		printf("ESC ");
	    } else if (*p < ' ') {
		printf("^%c ", *p + '@');
	    } else if (*p == ' ') {
		printf("sp ");
	    } else if (*p >= 0x7f) {
		printf("%x ", *p);
	    } else {
		printf("%c ", *p);
	    }
	}
	printf("\n");
#endif
    }

    flag = XIM_FLAG_X_LOOKUP_CHARS;
    if (sync) flag |= XIM_FLAG_SYNCHRONOUS;

    offset = IMPutHeader(conn, XIM_COMMIT, 0, 0);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMPutC16(conn, flag);
    IMPutC16(conn, (unsigned int)len);
    IMPutString(conn, str, len);
    IMFinishRequest(conn, offset);
}

/*- fixCallback: fix callback -*/
/* ARGSUSED */
static void
fixCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;
    IMConnection *conn = icp->im->connection;
    Widget proto = conn->proto_widget;
    Atom ctext = IMCtextAtom(proto);
    CCTextCallbackArg *arg = (CCTextCallbackArg *)call_data;

    TRACE(("imlib:fixCallback()\n"));

    /* check encoding and format */
    if (arg->encoding != ctext || arg->format != 8) {
	/*
	 * since every conversion object must support COMPOUND_TEXT,
	 * it is a serious error.
	 */
	String params[2];
	Cardinal num_params;
	WidgetClass ioc = icp->im->converter->input_object_class;

	params[0] = XtClass(proto)->core_class.class_name;
	params[1] = ioc->core_class.class_name;
	num_params = 2;

	XtAppErrorMsg(XtWidgetToApplicationContext(proto),
		      "encodingError", "convertedString", "WidgetError",
		      "%s: encoding of the converted string is not COMPOUND_STRING. check inputObject %s",
		      params, &num_params);
    }

    /*
     * Send fixed string via XIM_COMMIT message.
     * Since kinput2 uses full-synchronous mode,
     * synchronous flag must be turned off.
     */
    commitString(icp, arg->text, arg->length, 0);

#ifdef COMMIT_SYNC
    /*
     * Send XIM_SYNC_REPLY so that synchronize with clients here.
     */
    if (icp->state & IC_FORWARDING) {
	icp->state &= ~IC_FORWARDING;
#ifdef USE_WINIME
TRACE(("  XIM_SYNC_REPLY from fixCallback()\n"));
#endif
	IMSendRequestWithIC(conn, XIM_SYNC_REPLY, 0, icp);
    }
#endif /* COMMIT_SYNC */
}

/*- detachConverter: detach conversion widget from specified IC -*/
static void
detachConverter(icp)
IMIC *icp;
{
    Widget conv;

    TRACE(("imlib:detachConverter()\n"));

    conv = icp->conversion;
    XtRemoveCallback(conv, XtNtextCallback, fixCallback, (XtPointer)icp);
    XtRemoveCallback(conv, XtNendCallback, endCallback, (XtPointer)icp);
    XtRemoveCallback(conv, XtNunusedEventCallback, unusedEventCallback, (XtPointer)icp);
    if (icp->style == IMSTYLE_ON_THE_SPOT) {
	XtRemoveCallback(conv, XtNpreeditStartCallback, preeditStartCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNpreeditDoneCallback, preeditDoneCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNpreeditDrawCallback, preeditDrawCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNpreeditCaretCallback, preeditCaretCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNstatusStartCallback, statusStartCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNstatusDoneCallback, statusDoneCallback,
			 (XtPointer)icp);
	XtRemoveCallback(conv, XtNstatusDrawCallback, statusDrawCallback,
			 (XtPointer)icp);
    }

    CMReleaseConverter(XtParent(icp->im->connection->proto_widget), conv);
    icp->conversion = NULL;
}

/*- endCallback: conversion end callback -*/
/* ARGSUSED */
static void
endCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;

    TRACE(("imlib:endCallback()\n"));

    if (icp->state & IC_CONVERTING) {
	detachConverter(icp);
	icp->state &= ~IC_CONVERTING;
    }
}

/*- unusedEventCallback: unused key event callback -*/
/* ARGSUSED */
static void
unusedEventCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;
    IMConnection *conn = icp->im->connection;
    XKeyEvent *ev = (XKeyEvent *)call_data;
    int offset;

    TRACE(("imlib:unusedEventCallback()\n"));

    if (icp->im->mask & XIM_EXT_FORWARD_KEYEVENT_MASK) {
	offset = IMPutHeader(conn, XIM_EXT_FORWARD_KEYEVENT, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMPutC16(conn, 0);
	IMPutC16(conn, (unsigned int)(ev->serial & 0xffff));
	IMPutC8(conn, ev->type);
	IMPutC8(conn, (int)ev->keycode);
	IMPutC16(conn, (unsigned int)ev->state);
	IMPutC32(conn, ev->time);
	IMPutC32(conn, ev->window);
	IMFinishRequest(conn, offset);
    } else {
	offset = IMPutHeader(conn, XIM_FORWARD_EVENT, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMPutC16(conn, 0);	/* ?? */
	IMPutC16(conn, (unsigned int)((ev->serial >> 16) & 0xffff));

	IMPutC8(conn, ev->type);
	IMPutC8(conn, (int)ev->keycode);
	IMPutC16(conn, (unsigned int)(ev->serial & 0xffff));
	IMPutC32(conn, ev->time);
	IMPutC32(conn, ev->root);
	IMPutC32(conn, ev->window);
	IMPutC32(conn, ev->subwindow);
	IMPutI16(conn, ev->x_root);
	IMPutI16(conn, ev->y_root);
	IMPutI16(conn, ev->x);
	IMPutI16(conn, ev->y);
	IMPutC16(conn, ev->state);
	IMPutC8(conn, ev->same_screen);

	IMFinishRequest(conn, offset);
    }
}

/*- preeditStartCallback: preedit start -*/
/* ARGSUSED */
static void
preeditStartCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;

    TRACE(("preeditStartCallback(icp=0x%lx)\n", icp));

    if (!(icp->common_attr.input_style & XIMPreeditCallbacks))
	return;

    preeditStart(icp);
}

/*- preeditDoneCallback: preedit done -*/
/* ARGSUSED */
static void
preeditDoneCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;

    TRACE(("preeditDoneCallback(icp=0x%lx)\n", icp));

    if (!(icp->common_attr.input_style & XIMPreeditCallbacks))
	return;

    preeditDone(icp);
}

/*- preeditDrawCallback: preedit draw -*/
/* ARGSUSED */
static void
preeditDrawCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;
    OCCPreeditDrawArg *arg = (OCCPreeditDrawArg *)call_data;
    IMConnection *conn = icp->im->connection;
    Widget proto = conn->proto_widget;
    Atom ctext = IMCtextAtom(proto);

    TRACE(("preeditDrawCallback(icp=0x%lx, length=%d)\n",icp,arg->text_length));

    if (!(icp->common_attr.input_style & XIMPreeditCallbacks))
	return;

    /* check encoding and format */
    if (arg->encoding != ctext || arg->format != 8) {
	/*
	 * since every conversion object must support COMPOUND_TEXT,
	 * it is a serious error.
	 */
	String params[2];
	Cardinal num_params;
	WidgetClass ioc = icp->im->converter->input_object_class;

	params[0] = XtClass(proto)->core_class.class_name;
	params[1] = ioc->core_class.class_name;
	num_params = 2;

	XtAppErrorMsg(XtWidgetToApplicationContext(proto),
		      "encodingError", "preeditString", "WidgetError",
		      "%s: encoding of the preedit string is not COMPOUND_STRING. check inputObject %s",
		      params, &num_params);
    }

    preeditDraw(icp, arg);
}

/*- preeditCaretCallback: preedit caret -*/
/* ARGSUSED */
static void
preeditCaretCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;
    int caret = (int)call_data;

    TRACE(("preeditCaretCallback(icp=0x%lx, caret=%d)\n", icp, caret));

    if (!(icp->common_attr.input_style & XIMPreeditCallbacks))
	return;

    preeditCaret(icp, caret);
}

/*- statusStartCallback: status start -*/
/* ARGSUSED */
static void
statusStartCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;

    TRACE(("statusStartCallback(icp=0x%lx)\n", icp));

    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
	return;

    statusStart(icp);
}

/*- statusDoneCallback: status done -*/
/* ARGSUSED */
static void
statusDoneCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;

    TRACE(("statusDoneCallback(icp=0x%lx)\n", icp));

    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
	return;

    statusDone(icp);
}

/*- statusDrawCallback: status draw -*/
/* ARGSUSED */
static void
statusDrawCallback(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    IMIC *icp = (IMIC *)client_data;
    OCCPreeditDrawArg *arg = (OCCPreeditDrawArg *)call_data;
    IMConnection *conn = icp->im->connection;
    Widget proto = conn->proto_widget;
    Atom ctext = IMCtextAtom(proto);

    TRACE(("statusDrawCallback(icp=0x%lx, length=%d)\n", icp,arg->text_length));

    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
	return;

    /* check encoding and format */
    if (arg->encoding != ctext || arg->format != 8) {
	/*
	 * since every conversion object must support COMPOUND_TEXT,
	 * it is a serious error.
	 */
	String params[2];
	Cardinal num_params;
	WidgetClass ioc = icp->im->converter->input_object_class;

	params[0] = XtClass(proto)->core_class.class_name;
	params[1] = ioc->core_class.class_name;
	num_params = 2;

	XtAppErrorMsg(XtWidgetToApplicationContext(proto),
		      "encodingError", "statusString", "WidgetError",
		      "%s: encoding of the status string is not COMPOUND_STRING. check inputObject %s",
		      params, &num_params);
    }

    statusDraw(icp, arg);
}

/*- preeditStart: do preedit start -*/
static void
preeditStart(icp)
IMIC *icp;
{
    if (!(icp->state & IC_IN_PREEDIT)) {
	int offset;
	IMConnection *conn = icp->im->connection;

	TRACE(("imlib:preeditStart()\n"));

	offset = IMPutHeader(conn, XIM_PREEDIT_START, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMFinishRequest(conn, offset);
	icp->state |= IC_IN_PREEDIT;
    }
}

/*- preeditDone: do preedit done -*/
static void
preeditDone(icp)
IMIC *icp;
{
    if (icp->state & IC_IN_PREEDIT) {
	int offset;
	IMConnection *conn = icp->im->connection;

	TRACE(("imlib:preeditDone()\n"));

	offset = IMPutHeader(conn, XIM_PREEDIT_DONE, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMFinishRequest(conn, offset);
	icp->state &= ~IC_IN_PREEDIT;
    }
}

/*- preeditDraw: do actual preedit draw -*/
static void
preeditDraw(icp, data)
IMIC *icp;
OCCPreeditDrawArg *data;
{
    IMConnection *conn = icp->im->connection;
    int offset;
    unsigned int status;
    XIMFeedback feedback;
    int i;

    if (icp->state & IC_RESETTING) return;

    preeditStart(icp);

    TRACE(("imlib:preeditDraw()\n"));

#ifdef USE_WINIME
    {
	unsigned char *p = (unsigned char *)data->text;

	/*
	 * Dump preedit string.
	 */
	TRACE(("* preedit string: length = %d\n\t", data->text_length));
	for (i = 0; i < data->text_length; i++, p++) {
	    if (*p == '\033') {
		TRACE(("ESC "));
	    } else if (*p < ' ') {
		TRACE(("^%c ", *p + '@'));
	    } else if (*p == ' ') {
		TRACE(("sp "));
	    } else if (*p >= 0x7f) {
		TRACE(("%x ", *p));
	    } else {
		TRACE(("%c ", *p));
	    }
	}
	TRACE(("\n"));
    }
#else
    if (DDEBUG_CONDITION(5)) {
	unsigned char *p = (unsigned char *)data->text;

	/*
	 * Dump preedit string.
	 */
	printf("* preedit string:\n\t");
	for (i = 0; i < data->text_length; i++, p++) {
	    if (*p == '\033') {
		printf("ESC ");
	    } else if (*p < ' ') {
		printf("^%c ", *p + '@');
	    } else if (*p == ' ') {
		printf("sp ");
	    } else if (*p >= 0x7f) {
		printf("%x ", *p);
	    } else {
		printf("%c ", *p);
	    }
	}
	printf("\n");
    }
#endif
    offset = IMPutHeader(conn, XIM_PREEDIT_DRAW, 0, 0);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMPutC32(conn, data->caret);
    IMPutC32(conn, data->chg_first);
    IMPutC32(conn, data->chg_length);
    status = 0;
    if (data->text_length == 0)  status |= 0x1; /* no string */
    if (data->attrs_length == 0) status |= 0x2; /* no feedback */
    IMPutC32(conn, status);
    IMPutC16(conn, (unsigned int)data->text_length);
    if (data->text_length > 0) {
	IMPutString(conn, data->text, data->text_length);
    }
    IMPutPad(conn);
    IMPutC16(conn, (unsigned int)(data->attrs_length * 4));
    IMPutC16(conn, 0L); /* unused */
    if (data->attrs_length > 0) {
	for (i = 0; i < data->attrs_length; i++) {
	    IMPutC32(conn, data->attrs[i]);
	}
    }
    IMFinishRequest(conn, offset);
}

/*- preeditCaret: do actual preedit caret -*/
static void
preeditCaret(icp, caret)
IMIC *icp;
int caret;
{
    IMConnection *conn = icp->im->connection;
    int offset;

    if (icp->state & IC_RESETTING) return;

    preeditStart(icp);

    TRACE(("imlib:preeditCaret()\n"));

    offset = IMPutHeader(conn, XIM_PREEDIT_CARET, 0, 0);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMPutC32(conn, caret);
    IMPutC32(conn, (long)XIMAbsolutePosition);
    IMPutC32(conn, (long)XIMPrimary);
    IMFinishRequest(conn, offset);
}

/*- statusStart: do status start -*/
static void
statusStart(icp)
IMIC *icp;
{
    if (!(icp->state & IC_IN_STATUS)) {
	int offset;
	IMConnection *conn = icp->im->connection;

	TRACE(("imlib:statusStart()\n"));

	offset = IMPutHeader(conn, XIM_STATUS_START, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMFinishRequest(conn, offset);
	icp->state |= IC_IN_STATUS;
#ifdef STATUS_SYNC
        IMFlush(conn);
#endif /* STATUS_SYNC */
    }
}

/*- statusDone: do status done -*/
static void
statusDone(icp)
IMIC *icp;
{
    if (icp->state & IC_IN_STATUS) {
	int offset;
	IMConnection *conn = icp->im->connection;

	TRACE(("imlib:statusDone()\n"));

	offset = IMPutHeader(conn, XIM_STATUS_DONE, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMFinishRequest(conn, offset);
	icp->state &= ~IC_IN_STATUS;
#ifdef STATUS_SYNC
        IMFlush(conn);
#endif /* STATUS_SYNC */
    }
}

/*- statusDraw: do actual status draw -*/
static void
statusDraw(icp, data)
IMIC *icp;
OCCPreeditDrawArg *data;
{
    IMConnection *conn = icp->im->connection;
    int offset;
    unsigned int status;

    if (icp->state & IC_RESETTING) return;

    statusStart(icp);

    TRACE(("imlib:statusDraw()\n"));

    offset = IMPutHeader(conn, XIM_STATUS_DRAW, 0, 0);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMPutC32(conn, 0L); /* text type */
    status = 0;
    if (data->text_length == 0)  status |= 0x1; /* no string */
    if (data->attrs_length == 0) status |= 0x2; /* no feedback */
    IMPutC32(conn, status);
    IMPutC16(conn, (unsigned int)data->text_length);
    if (data->text_length > 0) {
	IMPutString(conn, data->text, data->text_length);
    }
    IMPutPad(conn);
    IMPutC16(conn, (unsigned int)(data->attrs_length * 32));
    IMPutC16(conn, 0L); /* unused */
    if (data->attrs_length > 0) {
	int i;
	for (i = 0; i < data->attrs_length; i++) {
	    IMPutC32(conn, 0L);
	}
    }
    IMFinishRequest(conn, offset);
#ifdef STATUS_SYNC
    IMFlush(conn);
#endif /* STATUS_SYNC */
}

/*- setEventMask: put XIM_SET_EVENT_MASK request on the output stream -*/
static void
setEventMask(icp, forward_mask, synchronous_mask)
IMIC *icp;
unsigned long forward_mask;
unsigned long synchronous_mask;
{
    IMConnection *conn = icp->im->connection;

    (void)IMPutHeader(conn, XIM_SET_EVENT_MASK, 0, 12);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMPutC32(conn, forward_mask);
    IMPutC32(conn, synchronous_mask);
    IMSchedule(conn, SCHED_WRITE);
}


/*
 * Public functions
 */

int
IMStartConversion(icp)
IMIC *icp;
{
    IMIM *imp = icp->im;
    Widget proto = imp->connection->proto_widget;
    Widget converter;
    WidgetClass class;
    unsigned long attrmask;
    ConversionAttributes attrs;

    TRACE(("IMStartConversion()\n"));

    if (icp->state & IC_CONVERTING) return 0;

    /*
     * Check required attributes i.e. client window.
     */
    if (!(icp->common_attr.set_mask & ATTR_MASK_CLIENT)) {
	IMSendError(icp->im->connection, IMBadSomething, icp->im->id, icp->id,
		    "client window required");
	return -1;
    }

    /*
     * Fill in default values for unspecified attributes.
     */
    fillDefaultAttributesForStartup(icp);

    /*
     * Get appropriate converter class.
     */
TRACE(("    input style: %d\n", icp->style));
    if (icp->style == IMSTYLE_OVER_THE_SPOT) {
#ifdef USE_WINIME
TRACE(("      USE OverTheSpot\n"));
#endif
	class = overTheSpotConversionWidgetClass;
    } else if (icp->style == IMSTYLE_OFF_THE_SPOT) {
#ifdef USE_WINIME
TRACE(("      USE OffTheSpot\n"));
#endif
	class = offTheSpotConversionWidgetClass;
    } else if (icp->style == IMSTYLE_ON_THE_SPOT) {
#ifdef USE_WINIME
TRACE(("      USE OnTheSpot\n"));
#endif
	class = onTheSpotConversionWidgetClass;
    } else {
#ifdef USE_WINIME
TRACE(("      USE SeparateSpot\n"));
#endif
	class = separateConversionWidgetClass;
    }

    /*
     * Compute conversion attributes to be passed to the converter.
     */
    attrmask = makeConvAttributesForStartup(icp, &attrs);

    icp->state &= ~IC_RESETTING;

    /*
     * Attach converter to this IC.
     */
    converter = CMGetConverter(XtParent(proto),
			       icp->common_attr.client, class,
			       imp->converter->input_object_class,
			       imp->converter->display_object_class);
    if (converter == NULL) {
	IMSendError(imp->connection, IMBadSomething, imp->id, icp->id,
		    "can't attach converter to this IC");
	return -1;
    }
    icp->conversion = converter;

    /*
     * Add callback functions.
     */
    XtAddCallback(converter, XtNtextCallback, fixCallback, (XtPointer)icp);
    XtAddCallback(converter, XtNendCallback, endCallback, (XtPointer)icp);
    XtAddCallback(converter, XtNunusedEventCallback, unusedEventCallback, (XtPointer)icp);
    if (icp->style == IMSTYLE_ON_THE_SPOT) {
	XtAddCallback(converter, XtNpreeditStartCallback, preeditStartCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNpreeditDoneCallback, preeditDoneCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNpreeditDrawCallback, preeditDrawCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNpreeditCaretCallback, preeditCaretCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNstatusStartCallback, statusStartCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNstatusDoneCallback, statusDoneCallback,
		      (XtPointer)icp);
	XtAddCallback(converter, XtNstatusDrawCallback, statusDrawCallback,
		      (XtPointer)icp);
    }

    /*
     * Start conversion
     */
    /* !!! if front-end method is used, ESMethodSelectFocus should be used */
    XtVaSetValues(converter, XtNeventSelectMethod, ESMethodNone, NULL);
    CControlStartConversion(converter, icp->common_attr.client,
			    attrmask, &attrs);

    icp->state |= IC_CONVERTING;

    if (icp->common_attr.input_style & XIMPreeditCallbacks)
	preeditStart(icp);

    /*
     * Send XIM_SET_EVENT_MASK to let the client forward the key events.
     */
    IMStartForwarding(icp);

    return 0;
}

void
IMStopConversion(icp)
IMIC *icp;
{
    TRACE(("IMStopConversion()\n"));

    if (!(icp->state & IC_CONVERTING)) return;

    /*
     * Terminate conversion.
     */
    CControlEndConversion(icp->conversion);

    if (icp->common_attr.input_style & XIMPreeditCallbacks)
	preeditDone(icp);
    IMStatusDone(icp);

    /*
     * Detach converter.
     */
    detachConverter(icp);

    /*
     * Stop forwarding key events unless this IC is being destroyed.
     */
    if (!(icp->state & IC_DESTROYING)) {
	IMStopForwarding(icp);
    }

    icp->state &= ~IC_CONVERTING;
}

int
IMResetIC(icp, preedit_strp)
IMIC *icp;
char **preedit_strp;
{
    int num_bytes = 0;

    TRACE(("IMResetIC()\n"));

    *preedit_strp = NULL;

    if (icp->state & IC_CONVERTING) {
	/*
	 * get input object by asking conversion widget of XtNinputObject
	 * resource. however, it is not recommended since protocol widget
	 * should interact with input object only through conversion
	 * widget.
	 */
	CCTextCallbackArg arg;
	Widget input_obj;
	Widget w = icp->im->connection->proto_widget;

	XtVaGetValues(icp->conversion, XtNinputObject, &input_obj, NULL);
	arg.encoding = IMCtextAtom(w);
#ifdef notdef
	if (ICGetConvertedString(input_obj, &arg.encoding, &arg.format,
				 &arg.length, &arg.text) >= 0) {
	    num_bytes = arg.length;
	    *preedit_strp = (char *)arg.text;
	}
#else
	/*
	 * Canna seems to have some problem with ICGetConvertedString().
	 * Use ICGetPreeditString instead.
	 */
	if (ICGetPreeditString(input_obj, 0, 0, &arg.encoding, &arg.format,
			       &arg.length, &arg.text) >= 0) {
	    num_bytes = arg.length;
	    *preedit_strp = (char *)arg.text;
	}
#endif
	ICClearConversion(input_obj);
	TRACE(("\twas converting. %d bytes left\n", num_bytes));

	if (icp->common_attr.reset_state == XIMInitialState) {
	    /* Force to end the conversion. */
	    TRACE(("\tback to the initial state\n"));
	    IMStopConversion(icp);
	}
    }
    return num_bytes;
}

//#ifdef USE_WINIME
#if 0
static void
unusedEventForward(IMIC *icp, XKeyEvent *ev)
{
    IMConnection *conn = icp->im->connection;
    int offset;

    TRACE(("imlib:unusedEventForward()\n"));

    if (icp->im->mask & XIM_EXT_FORWARD_KEYEVENT_MASK) {
	offset = IMPutHeader(conn, XIM_EXT_FORWARD_KEYEVENT, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMPutC16(conn, 0);
	IMPutC16(conn, (unsigned int)(ev->serial & 0xffff));
	IMPutC8(conn, ev->type);
	IMPutC8(conn, (int)ev->keycode);
	IMPutC16(conn, (unsigned int)ev->state);
	IMPutC32(conn, ev->time);
	IMPutC32(conn, ev->window);
	IMFinishRequest(conn, offset);
    } else {
	offset = IMPutHeader(conn, XIM_FORWARD_EVENT, 0, 0);
	IMPutC16(conn, icp->im->id);
	IMPutC16(conn, icp->id);
	IMPutC16(conn, 0);	/* ?? */
	IMPutC16(conn, (unsigned int)((ev->serial >> 16) & 0xffff));

	IMPutC8(conn, ev->type);
	IMPutC8(conn, (int)ev->keycode);
	IMPutC16(conn, (unsigned int)(ev->serial & 0xffff));
	IMPutC32(conn, ev->time);
	IMPutC32(conn, ev->root);
	IMPutC32(conn, ev->window);
	IMPutC32(conn, ev->subwindow);
	IMPutI16(conn, ev->x_root);
	IMPutI16(conn, ev->y_root);
	IMPutI16(conn, ev->x);
	IMPutI16(conn, ev->y);
	IMPutC16(conn, ev->state);
	IMPutC8(conn, ev->same_screen);

	IMFinishRequest(conn, offset);
    }
}
#endif

void
IMForwardEvent(icp, ev)
IMIC *icp;
XEvent *ev;
{
    TRACE(("IMForwardEvent()\n"));

#ifdef USE_WINIME
    if (icp->conversion == NULL)
    {
//	unusedEventForward(icp, (XKeyEvent*)ev);
#if 0	// –³ŒÀƒ‹[ƒv‚É‚È‚é
	Window savewin;
	Window savesubwin;

	TRACE(("sendback event to window 0x%lx\n", icp->common_attr.focus));
	savewin = ev->xkey.window;
	savesubwin = ev->xkey.subwindow;
	ev->xkey.window = icp->common_attr.focus;
	ev->xkey.subwindow = None;

	/*
	 * here we use NoEventMask as the eventmask, not
	 * KeyPressMask.  that means the event will be sent only
	 * to the client who created the destination window.
	 */
	XSendEvent(XtDisplay(icp->im->connection->proto_widget), ev->xkey.window, False, NoEventMask, ev);

	ev->xkey.window = savewin;	/* restore */
	ev->xkey.subwindow = savesubwin;	/* restore */
#endif
	return;
    }
#else
    if (icp->conversion == NULL) return;
#endif
    XtCallActionProc(icp->conversion, "to-inputobj", ev,
		     (String *)NULL, (Cardinal)0);
}

/* ARGSUSED */
void
IMSetFocus(icp)
IMIC *icp;
{
    TRACE(("IMSetFocus(ic%d)\n", icp->id));
    if (icp->conversion != NULL) {
	CControlChangeFocus(icp->conversion, 1);
    }
}

/* ARGSUSED */
void
IMUnsetFocus(icp)
IMIC *icp;
{
    TRACE(("IMUnsetFocus(ic%d)\n", icp->id));
    if (icp->conversion != NULL) {
	CControlChangeFocus(icp->conversion, 0);
    }
}

/* ARGSUSED */
void
IMStatusStart(icp)
IMIC *icp;
{
    TRACE(("IMStatusStart(ic%d)\n", icp->id));
    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
	return;
    statusStart(icp);
}

/* ARGSUSED */
void
IMStatusDone(icp)
IMIC *icp;
{
    TRACE(("IMStatusDone(ic%d)\n", icp->id));
    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
	return;
    statusDone(icp);
}

void
IMStartForwarding(icp)
IMIC *icp;
{
    /*
     * Make the client forward key events to us.
     */
    TRACE(("IMStartForwarding(ic%d)\n", icp->id));

#define FORWARD_MASK (KeyPressMask|KeyReleaseMask)

#ifdef notdef
    if (synchronous) {
	setEventMask(icp, FORWARD_MASK, FORWARD_MASK);
    } else {
	setEventMask(icp, FORWARD_MASK, NoEventMask);
    }
#else
    /* using full-synchronous method */
    setEventMask(icp, FORWARD_MASK, FORWARD_MASK);
#endif

#ifdef USE_WINIME
    winim_clients++;
#endif
#undef FORWARD_MASK
}

void
IMStopForwarding(icp)
IMIC *icp;
{
    /*
     * Make the client stop sending key events.
     */
    TRACE(("IMStopForwarding(ic%d)\n", icp->id));
    setEventMask(icp, NoEventMask, NoEventMask);
#ifdef USE_WINIME
    winim_clients--;
#endif
}

// >> Add Y.Arai
#ifdef USE_WINIME
void
IMCommitString(IMIC *icp, char *str, int len, int sync)
{
    IMConnection *conn = icp->im->connection;

    TRACE(("IMCommitString(ic%d)\n", icp->id));

    if (!(icp->common_attr.input_style & XIMStatusCallbacks))
    {
	TRACE(("    return with no action.\n"));
	return;
    }

    commitString(icp, str, len, sync);

#ifdef COMMIT_SYNC
    /*
     * Send XIM_SYNC_REPLY so that synchronize with clients here.
     */
    if (icp->state & IC_FORWARDING)
    {
	TRACE(("    forward committed string...\n", icp->id));
	icp->state &= ~IC_FORWARDING;
#ifdef USE_WINIME
TRACE(("  XIM_SYNC_REPLY from IMCommitString()\n"));
#endif
	IMSendRequestWithIC(conn, XIM_SYNC_REPLY, 0, icp);
    }
#endif /* COMMIT_SYNC */
}

#if 0
void
IMDummyTriggerNotifyReply(IMIC *icp)
{
    IMConnection *conn = icp->im->connection;

    /*
     * Send XIM_TRIGGER_NOTIFY_REPLY request.
     */
    (void)IMPutHeader(conn, XIM_TRIGGER_NOTIFY_REPLY, 0, 4);
    IMPutC16(conn, icp->im->id);
    IMPutC16(conn, icp->id);
    IMSchedule(conn, SCHED_WRITE);
}
#endif
#endif
// << Add Y.Arai
