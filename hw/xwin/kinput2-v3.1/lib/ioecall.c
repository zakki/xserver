#ifndef lint
static char *rcsid = "$Id: ioecall.c,v 1.2 1996/09/30 09:19:52 ishisone Exp $";
#endif
/*
 * Copyright (C) 1994  Software Research Associates, Inc.
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

/*
 *	X I/O error callback
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xfuncproto.h>
#include "IOECall.h"
#ifdef USE_WINIME
#include <stdlib.h>
#endif

#ifndef NULL
#define NULL	0
#endif

typedef struct ioe_cb_desc_ {
    void (*callback)();
    XPointer client_data;
    struct ioe_cb_desc_ *next;
} CBDesc;

static CBDesc *cb_list;

static int (*original_handler)();

static int
XIOEHandler(dpy)
Display *dpy;
{
    CBDesc *cb;

    cb = cb_list;
    while (cb != NULL) {
	(*cb->callback)(cb->client_data);
	cb = cb->next;
    }
    return (*original_handler)(dpy);
}

void
XIOEInit()
{
    int (*oldhandler)() = XSetIOErrorHandler(XIOEHandler);

    if (oldhandler != XIOEHandler) original_handler = oldhandler;
}

XIOEHandle
XIOESet(handler, client_data)
void (*handler)();
XPointer client_data;
{
    CBDesc *cb;

    cb = (CBDesc *)malloc(sizeof(CBDesc));
    if (cb == NULL) return NULL;

    cb->callback = handler;
    cb->client_data = client_data;
    cb->next = NULL;

    if (cb_list == NULL) {
	cb_list = cb;
    } else {
	CBDesc *p = cb_list;

	while (p->next != NULL) p = p->next;
	p->next = cb;
    }

    return cb;
}

void
XIOEUnset(handle)
XIOEHandle handle;
{
    CBDesc *cb, *cb0;

    cb = cb_list;
    cb0 = NULL;

    while (cb != NULL) {
	if (cb == handle) {
	    if (cb0 == NULL) {
		cb_list = cb->next;
	    } else {
		cb0->next = cb->next;
	    }
	    (void)free((char *)cb);
	    return;
	}
	cb = cb->next;
    }
}
