/* $Id: IOECall.h,v 1.1 1994/11/22 01:13:01 ishisone Exp $ */
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

#ifndef _IOECall_h
#define _IOECall_h

/*
 * X I/O error callback handler
 */

/*
 * XIOEHandle -- an opaque type used as a 'handle' in this library
 */
typedef struct ioe_cb_desc_ *XIOEHandle;

extern void XIOEInit(
#if NeedFunctionPrototypes
	void
#endif
);

extern XIOEHandle XIOESet(
#if NeedFunctionPrototypes
	void (*)(),
	XPointer
#endif
);

extern void XIOEUnset(
#if NeedFunctionPrototypes
	XIOEHandle handle
#endif
);

#endif /* _IOECall_h */
