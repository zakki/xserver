/* $Id: OnConv.h,v 10.6 1999/05/18 08:53:18 ishisone Exp $ */
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
 * Copyright (c) 1999  Kazuki YASUMATSU
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in adntising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 * Author:  Kazuki YASUMATSU (Kazuki.Yasumatsu@fujixerox.co.jp)
 */


#ifndef _OnTheSpotConversion_h
#define _OnTheSpotConversion_h

/* OnTheSpotConversion widget public header file */

#include "ConvCtrl.h"

#define XtNpreeditStartCallback	"preeditStartCallback"
#define XtNpreeditDoneCallback	"preeditDoneCallback"
#define XtNpreeditDrawCallback	"preeditDrawCallback"
#define XtNpreeditCaretCallback	"preeditCaretCallback"
#define XtNstatusStartCallback	"statusStartCallback"
#define XtNstatusDoneCallback	"statusDoneCallback"
#define XtNstatusDrawCallback	"statusDrawCallback"
#define XtNfeedbackAttributes	"feedbackAttributes"
#define XtCFeedbackAttributes	"FeedbackAttributes"

typedef struct {
    int		caret;		/* cursor offset within pre-edit string */
    int		chg_first;	/* starting change position */
    int		chg_length;	/* length of the change in character count */
    Atom	encoding;	/* text encoding e.g. COMPOUND_TEXT */
    int		format;		/* text format (8/16/32) */
    int		text_length;	/* text length */
    XtPointer	text;		/* text data */
    int		attrs_length;	/* attributes length */
    unsigned long *attrs;	/* attributes of the text */
} OCCPreeditDrawArg;

typedef struct _OnTheSpotConversionClassRec*	OnTheSpotConversionWidgetClass;
typedef struct _OnTheSpotConversionRec*		OnTheSpotConversionWidget;

extern WidgetClass onTheSpotConversionWidgetClass;

#endif

