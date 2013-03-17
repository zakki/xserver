/*
 *	xtwstr.c
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
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
static char	*rcsid = "$Id: xtwstr.c,v 2.3 1991/10/02 04:27:04 ishisone Rel $";
#endif

#include <X11/Intrinsic.h>
#include "WStr.h"
#include "XWStr.h"

#define IS2B(f)	(((f)->max_byte1 > 0) || ((f)->max_char_or_byte2 > 255))

XWSGC
XtWSGetGCSet(Widget w, unsigned long mask, XGCValues *values, XFontSet font)
{
    XGCValues	gcval;
    XWSGC		gcset;

    gcset = (XWSGC)XtMalloc(sizeof(XWSGCSet));
    gcset->fe.font = font;

    gcval = *values;
    if (gcset->fe.font != NULL) {
        gcset->fe.gc = XtGetGC(w, mask, &gcval);
        gcset->fe.flag = GCCREAT;
    } else {
        gcset->fe.gc = NULL;
    }

    return gcset;
}

void
XtWSDestroyGCSet(gcset)
XWSGC gcset;
{
    int	flag;

    if (gcset->fe.gc != NULL) {
        flag = gcset->fe.flag;
        if (flag & GCCREAT)
            XtDestroyGC(gcset->fe.gc);
        /* can't free XFontStruct data allocated by XWSSetGCSet()
         * because I can't figure out which display is used.
         * if (flag & FONTQUERY)
         *	XFreeFont(???, gcset->fe[i].font);
         */
    }
    XtFree((char *)gcset);
}

void
XtWSReleaseGCSet(Widget w, XWSGC gcset)
{
    int	flag;

    if (gcset->fe.gc != NULL) {
        flag = gcset->fe.flag;
        if (flag & GCCREAT)
            XtReleaseGC(w, gcset->fe.gc);
        if (flag & FONTQUERY)
            XFreeFontSet(XtDisplay(w), gcset->fe.font);
    }
    XtFree((char *)gcset);
}
