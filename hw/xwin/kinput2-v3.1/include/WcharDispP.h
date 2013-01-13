/* $Id: WcharDispP.h,v 1.6 1991/09/17 10:08:59 ishisone Rel $ */
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

#ifndef _WcharDisplayP_h
#define _WcharDisplayP_h

#include "ConvDispP.h"
#include "WcharDisp.h"
#include "XWStr.h"


typedef struct _WcharDisplayClassRec {
    ObjectClassPart object_class;
    ConvDisplayClassPart convDisplay_class;
} WcharDisplayClassRec;

typedef struct {
    /* resources */
    XFontSet defaultfont;	/* default fonts */
    /* private state */
    Cardinal num_specs;
    XFontSet font;		/* fonts now in use */
    Pixmap stipple;			/* Stipple Bitmap */
    XWSGC gcset_normal;
    XWSGC gcset_rev;
    GC gc_normal;
    GC gc_stipple;
    int fontheight;
    int ascent;
} WcharDisplayPart;

typedef struct _WcharDisplayRec {
    ObjectPart  object;
    ConvDisplayPart convDisplay;
    WcharDisplayPart wcharDisplay;
} WcharDisplayRec;

extern WcharDisplayClassRec wcharDisplayClassRec;


typedef struct {
    int empry;
} JpWcharDisplayClassPart;

typedef struct _JpWcharDisplayClassRec {
    ObjectClassPart object_class;
    ConvDisplayClassPart convDisplay_class;
} JpWcharDisplayClassRec;

typedef struct {
    int empty;
} JpWcharDisplayPart;

typedef struct _JpWcharDisplayRec {
    ObjectPart  object;
    ConvDisplayPart convDisplay;
    WcharDisplayPart wcharDisplay;
    JpWcharDisplayPart jpWcharDisplay;
} JpWcharDisplayRec;

extern JpWcharDisplayClassRec jpWcharDisplayClassRec;

#endif
