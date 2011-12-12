/* $Id: OnConvP.h,v 10.5 1999/05/18 08:53:19 ishisone Exp $ */
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
 * Copyright (c) 1999  Kazuki YASUMATSU
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
 * Author:  Kazuki YASUMATSU (Kazuki.Yasumatsu@fujixerox.co.jp)
 */


#ifndef _OnTheSpotConversionP_h
#define _OnTheSpotConversionP_h

#include "ConvCtrlP.h"
#include "OnConv.h"

#define XtRFeedbackAttributes	"FeedbackAttributes"

typedef struct {
    ICString seg;		/* セグメント */
} DisplaySegment;

typedef struct {
    unsigned long feedbacks[4];
#define FEEDBACK_NOCONV		0	/* for non-converted segment */
#define FEEDBACK_CONV		1	/* converted but not current */
#define FEEDBACK_CURRENT	2	/* converted and current */
#define FEEDBACK_CURRENTSUB	3	/* converted and current subsegment */
} FeedbackAttributes;

/*
 *	on-the-spot conversion control widget data structure
 */

typedef struct {
    int empty;
} OnTheSpotConversionClassPart;

typedef struct _OnTheSpotConversionClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ShellClassPart	shell_class;
    WMShellClassPart	wm_shell_class;
    VendorShellClassPart	vendor_shell_class;
    TransientShellClassPart	transient_shell_class;
    ConversionControlClassPart	conversionControl_class;
    OnTheSpotConversionClassPart onTheSpotConversion_class;
} OnTheSpotConversionClassRec;

extern OnTheSpotConversionClassRec onTheSpotConversionClassRec;

typedef struct {
    /* resources */
    XtCallbackList	preeditstartcallback;
    XtCallbackList	preeditdonecallback;
    XtCallbackList	preeditdrawcallback;
    XtCallbackList	preeditcaretcallback;
    XtCallbackList	statusstartcallback;
    XtCallbackList	statusdonecallback;
    XtCallbackList	statusdrawcallback;
    FeedbackAttributes	feedbackattrs;
    /* private state */
    Widget	selectionshell;
    Widget	selectionwidget;
    Widget	auxshell;
    Widget	auxwidget;
    DisplaySegment	*dispsegments;
    Cardinal	numsegments;
    Cardinal	dispsegmentsize;
    ICString	*candlist;
    Cardinal	numcands;
    Boolean	selectionpoppedup;
    Boolean	auxpoppedup;
    Cardinal	lastcaret;
    Boolean	fixnotify;
} OnTheSpotConversionPart;

typedef struct _OnTheSpotConversionRec {
    CorePart		core;
    CompositePart	composite;
    ShellPart		shell;
    WMShellPart		wm;
    VendorShellPart	vendor;
    TransientShellPart	transient;	
    ConversionControlPart	ccontrol;
    OnTheSpotConversionPart	onthespot;
} OnTheSpotConversionRec;

#endif

