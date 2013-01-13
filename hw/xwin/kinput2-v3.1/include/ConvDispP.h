/* $Id: ConvDispP.h,v 1.7 1991/09/17 10:04:46 ishisone Rel $ */
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

#ifndef _ConvDisplayP_h
#define _ConvDisplayP_h

#include <X11/ObjectP.h>
#include "ConvDisp.h"

typedef struct {
    int		(*StringWidth)(Widget, ICString*, int, int);
    int		(*LineHeight)(Widget, Position*);
    void	(*DrawString)(Widget, Widget, ICString*, int, int, int, int);
    int		(*MaxChar)(Widget, ICString*, int, int);
    void	(*DrawCursor)(Widget, Widget, int, int, int);
    void	(*GetCursorBounds)(Widget,XRectangle*);
} ConvDisplayClassPart;

typedef struct _ConvDisplayClassRec {
    ObjectClassPart object_class;
    ConvDisplayClassPart convDisplay_class;
} ConvDisplayClassRec;

typedef struct {
    /* resources */
    Pixel	foreground;
    Pixel	background;
    Pixmap	cursor;		/* really a bitmap */
    Position	hotx;
    Position	hoty;
    /* private */
    Boolean	cursorcreated;
    XRectangle	cursorbounds;
    GC		cursorgc;
    Boolean	cursorvisible;
} ConvDisplayPart;

typedef struct _ConvDisplayRec {
    ObjectPart  object;
    ConvDisplayPart convDisplay;
} ConvDisplayRec;

extern ConvDisplayClassRec	convDisplayClassRec;

#define XtInheritStringWidth	(int(*)())_XtInherit
#define XtInheritLineHeight	(int(*)())_XtInherit
#define XtInheritDrawString	(void(*)())_XtInherit
#define XtInheritMaxChar	(int(*)())_XtInherit
#define XtInheritDrawCursor	(void(*)())_XtInherit
#define XtInheritGetCursorBounds	(void(*)())_XtInherit

#endif
