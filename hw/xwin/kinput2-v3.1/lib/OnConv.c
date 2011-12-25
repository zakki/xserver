#ifndef lint
static char *rcsid = "$Id: OnConv.c,v 10.9 1999/05/19 08:52:42 ishisone Exp $";
#endif
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


#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#if XtSpecificationRelease > 4
#include <X11/Xfuncs.h>
#endif
#include "CachedAtom.h"
#include "AsyncErr.h"
#include "OnConvP.h"
#include "InputConv.h"
#include "ConvDisp.h"
#include "CandPanel.h"
#include "AuxPanel.h"
#include "ICLabel.h"

#ifdef USE_WINIME
#include <X11/Xproto.h>
#define ATOM			DWORD
#include "winms.h"
#include <stdlib.h>
typedef unsigned short	wchar;
#include <X11/extensions/winime.h>
#endif

#define DEBUG_VAR debug_OnTheSpotConversion
#include "DebugPrint.h"

/*- resource table -*/
static XtResource resources[] = {
#define offset(field) XtOffset(OnTheSpotConversionWidget, onthespot.field)
    { XtNpreeditStartCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(preeditstartcallback), XtRCallback, (XtPointer)NULL },
    { XtNpreeditDoneCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(preeditdonecallback), XtRCallback, (XtPointer)NULL },
    { XtNpreeditDrawCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(preeditdrawcallback), XtRCallback, (XtPointer)NULL },
    { XtNpreeditCaretCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(preeditcaretcallback), XtRCallback, (XtPointer)NULL },
    { XtNstatusStartCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(statusstartcallback), XtRCallback, (XtPointer)NULL },
    { XtNstatusDoneCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(statusdonecallback), XtRCallback, (XtPointer)NULL },
    { XtNstatusDrawCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	offset(statusdrawcallback), XtRCallback, (XtPointer)NULL },
    { XtNfeedbackAttributes, XtCFeedbackAttributes,
	XtRFeedbackAttributes, sizeof(FeedbackAttributes),
	offset(feedbackattrs), XtRString, (XtPointer)"U,,R,H" },
#undef offset
};

/*- default translations -*/
static char translations[] = "<Key>: to-inputobj()";	/* same as superclass's	*/

/*- declarations of local structures -*/
typedef struct {
    int		attrs_length;
    int		attrs_limit;
    unsigned long *attrs_buffer;
} AttributeBuffer;

/*- declarations of static functions -*/
static void ClassInitialize();
static void Initialize();
static void Destroy();
static Boolean SetValues();

static void ConversionStartup();
static void ConversionFinish();
static void ChangeAttributes();
static void ChangeFocus();

static void StringToFeedbackAttrs();

static Widget CreateSelectionWidget();

static void UpdateText();
static void CommitText();
static void UpdateMode();
static void SelectionControl();

#ifdef USE_WINIME
static void SelectionCalc();
#endif
static void SelectionStart();
static void LocateSelectionPopup();
static void SelectionEnd();
static void SelectionSet();
static void SelectionGet();
static void SelectionMove();

static Widget CreateAuxWidget();
static void AuxControl();
static void AuxStart();
static void AuxEnd();
static void AuxChange();
static void LocateAuxPopup();

static void SelectionSelected();

static Boolean SafeGetWindowAttributes();
static void MoveShell();
static Window getToplevelWindow();
static void setTransientFor();
static void allocDisplaySegments();
static void freeDisplaySegment();
static void clearAllDisplaySegments();
static void copyString();
static void freeString();
static AttributeBuffer *allocAttributeBuffer();
static void destroyAttributeBuffer();
static void addAttributeBuffer();
static unsigned long attrToFeedback();
static void CBPreeditStart();
static void CBPreeditDone();
static void CBPreeditDraw();
static void CBPreeditCaret();
static void CBStatusStart();
static void CBStatusDone();
static void CBStatusDraw();

/*- composite-extension rec: for enabling non-widget children -*/
static CompositeClassExtensionRec CompositeExtension = {
    /* next_extension		*/	NULL,
    /* record_type		*/	NULLQUARK,
    /* version			*/	XtCompositeExtensionVersion,
    /* record_size		*/	sizeof(CompositeClassExtensionRec),
    /* accept_objects		*/	True,
};

/*- onTheSpotConversionClass record -*/
OnTheSpotConversionClassRec onTheSpotConversionClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass)&conversionControlClassRec,
    /* class_name		*/	"OnTheSpotConversion",
    /* widget_size		*/	sizeof(OnTheSpotConversionRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	XtInheritResize,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* composite fields */
    /* geometry_manager		*/	XtInheritGeometryManager,
    /* change_managed		*/	XtInheritChangeManaged,
    /* insert_child		*/	XtInheritInsertChild,
    /* delete_child		*/	XtInheritDeleteChild,
    /* extension		*/	(XtPointer)&CompositeExtension,
  },
  { /* shell fields */
    /* extension		*/	NULL
  },
  { /* wm_shell fields */
    /* extension		*/	NULL
  },
  { /* vendor_shell fields */
    /* extension		*/	NULL
  },
  { /* transient_shell fields */
    /* extension		*/	NULL
  },
  { /* conversionControl fields */
    /* Startup			*/	ConversionStartup,
    /* Finish			*/	ConversionFinish,
    /* ChangeAttributes		*/	ChangeAttributes,
    /* ChangeFocus		*/	ChangeFocus,
    /* TextChange		*/	UpdateText,
    /* Fix			*/	CommitText,
    /* ModeChange		*/	UpdateMode,
    /* SelectionControl		*/	SelectionControl,
    /* AuxControl		*/	AuxControl,
  },
  { /* onTheSpotConversion fields */
    /* empty			*/	0
  },
};

WidgetClass onTheSpotConversionWidgetClass = (WidgetClass)&onTheSpotConversionClassRec;

/*
 *+ Core class method
 */

/*- ClassInitialize: class initializer -*/
/* ARGSUSED */
static void
ClassInitialize()
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /* add String -> FeedbackAttributes converter */
    XtAddConverter(XtRString, XtRFeedbackAttributes, StringToFeedbackAttrs,
		   (XtConvertArgList)NULL, (Cardinal)0);
}

/*- Initialize: initalize method -*/
/* ARGSUSED */
static void
Initialize(req, new, args, num_args)
Widget req;
Widget new;
ArgList args;
Cardinal *num_args;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)new;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    (void)CreateSelectionWidget(ocw);
    (void)CreateAuxWidget(ocw);

    ocw->onthespot.dispsegments = NULL;
    ocw->onthespot.numsegments = 0;
    ocw->onthespot.dispsegmentsize = 0;
    ocw->onthespot.candlist = NULL;
    ocw->onthespot.numcands = 0;
    ocw->onthespot.selectionpoppedup = False;
    ocw->onthespot.auxpoppedup = False;
    ocw->onthespot.lastcaret = 0;
    ocw->onthespot.fixnotify = False;
}

/*- Destroy: destroy method -*/
static void
Destroy(w)
Widget w;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /* ディスプレイセグメントの領域を解放 */
    if (ocw->onthespot.dispsegments) {
	DisplaySegment *dsp = ocw->onthespot.dispsegments;
	int i;

	for (i = 0; i < ocw->onthespot.numsegments; i++) {
	    freeString(&dsp[i].seg);
	}
	XtFree((char *)dsp);
    }
}

/*- SetValues: setvalues method -*/
/* ARGSUSED */
static Boolean
SetValues(cur, req, new, args, num_args)
Widget cur;
Widget req;
Widget new;
ArgList args;
Cardinal *num_args;
{
    /* OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)new; */
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    return False;
}

/*
 *+ ConversionControl class method
 */

/*- ConversionStartup: class specific conversion startup -*/
static void
ConversionStartup(widget, valuemask, value)
Widget widget;
unsigned long valuemask;
ConversionAttributes *value;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)widget;
    Widget inputobj = ocw->ccontrol.inputobj;
    Window toplevel;

    TRACE(("OnTheSpot:ConversionStartup()\n"));

    /* 内部のバッファをクリアする */
    clearAllDisplaySegments(ocw);

    /* WM_TRANSIENT_FOR プロパティを正しくセットする */
    toplevel = getToplevelWindow(XtDisplay(widget),
				 ocw->ccontrol.clientwindow);
    setTransientFor(ocw->onthespot.selectionshell, toplevel);
    setTransientFor(ocw->onthespot.auxshell, toplevel);

    /*
     * OnTheSpotConvesion の widget 自体はポップアップさせないが、
     * バックエンドタイプの時にはクライアントがこの widget の
     * ウィンドウに対してイベントを送るので Realize だけしておく
     */
    if (!XtIsRealized(widget)) {
	Arg args[2];

	XtSetArg(args[0], XtNwidth, 1);
	XtSetArg(args[1], XtNheight, 1);
	XtSetValues(widget, args, 2);
	XtRealizeWidget(widget);
    }

    ocw->onthespot.lastcaret = 0;
    ocw->onthespot.fixnotify = False;

    /* ステータスを更新する */
    UpdateMode(widget);

    /* プレエディットを開始する */
    CBPreeditStart(widget);
}

/*- ConversionFinish: class specific conversion finish -*/
/* ARGSUSED */
static void
ConversionFinish(w)
Widget w;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    Atom encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    int format = 8;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (!ocw->onthespot.fixnotify) {
	/* 確定していない */
	DisplaySegment *dseg = ocw->onthespot.dispsegments;
	int i, len = 0;
	for (i = 0; i < ocw->onthespot.numsegments; i++, dseg++) {
	    len += dseg->seg.nchars;
	}
	if (len > 0) {
	    /* プレエディットバッファの消去 */
	    unsigned long attr = 0;
	    CBPreeditDraw(ocw, 0, 0, len, encoding, format, 0, (XPointer)"",
			  0, &attr);
	}
    }

    /* プレエディットを終了する */
    CBPreeditDone(w);

    /* ステータスを更新(初期化)する */
    CBStatusDraw(ocw, encoding, format, 0, (XPointer)"", 0);

    if (ocw->onthespot.selectionpoppedup) {
	XtPopdown(ocw->onthespot.selectionshell);
	ocw->onthespot.selectionpoppedup = False;
    }
    if (ocw->onthespot.auxpoppedup) {
	XtPopdown(ocw->onthespot.auxshell);
	ocw->onthespot.auxpoppedup = False;
    }
}

/*- ChangeAttributes: class specific conversion attribute change routine -*/
/* ARGSUSED */
static void
ChangeAttributes(w, valuemask, value)
Widget w;
unsigned long valuemask;
ConversionAttributes *value;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /* do nothing */
}

/*- ChangeFocus: class specific conversion attribute change routine -*/
static void
ChangeFocus(w, set)
Widget w;
int set;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (set) {
	UpdateMode(w);
    } else {
#if 0
	/* フォーカスを失った場合には何もしない */
	OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
	Atom encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
	int format = 8;

	CBStatusDraw(ocw, encoding, format, 0, (XPointer)"", 0);
#endif
    }
}

/*
 *+ resource converter
 */

/*- StringToFeedbackAttrs: string to feedback attiributes converter -*/
static void
StringToFeedbackAttrs(args, num_args, from, to)
XrmValue *args;
Cardinal *num_args;
XrmValue *from;
XrmValue *to;
{
    char *s = (char *)from->addr;
    int idx;
    int invalid = 0;
    static FeedbackAttributes fba;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    for (idx = 0; idx < 4; idx++) fba.feedbacks[idx] = 0;

    for (idx = 0; idx < 4; idx++) {
	int c;
	unsigned long fb = 0;

	while ((c = *s++) != ',' && c != '\0') {
	    switch (c) {
	    case 'R': case 'r': fb |= XIMReverse; break;
	    case 'U': case 'u':	fb |= XIMUnderline; break;
	    case 'H': case 'h': fb |= XIMHighlight; break;
	    case 'P': case 'p': fb |= XIMPrimary; break;
	    case 'S': case 's': fb |= XIMSecondary; break;
	    case 'T': case 't': fb |= XIMTertiary; break;
	    case ' ': case '\t':
		break;
	    default:
		invalid++;
		break;
	    }
	}
	fba.feedbacks[idx] = fb;
	if (c == '\0') break;
    }

    if (invalid) {
	XtStringConversionWarning((char *)from->addr, XtRFeedbackAttributes);
    }
    to->size = sizeof(fba);
    to->addr = (caddr_t)&fba;
}

/*
 *+ sub-widget creation
 */

/*- CreateSelectionWidget: create selection widget for selecting candidates -*/
static Widget
CreateSelectionWidget(ocw)
OnTheSpotConversionWidget ocw;
{
    Widget shell, sel;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /* set width/height so that XtRealizeWidget() doesn't cause error */
    shell = XtVaCreatePopupShell("selectionShell",
				 transientShellWidgetClass,
				 (Widget)ocw,
				 XtNwidth, 1,
				 XtNheight, 1,
				 NULL);
    ocw->onthespot.selectionshell = shell;

    sel = XtCreateManagedWidget("selection", candidatePanelWidgetClass,
				shell, NULL, 0);
    (void)XtCreateWidget("display", ocw->ccontrol.displayobjclass, sel,
			 NULL, 0);
    XtAddCallback(sel, XtNcallback, SelectionSelected, (XtPointer)ocw);
    XtInstallAccelerators(sel, (Widget)ocw);

    ocw->onthespot.selectionwidget = sel;

    return shell;
}

/*- CreateAuxWidget: create aux widget for display auxiliary data -*/
static Widget
CreateAuxWidget(ocw)
OnTheSpotConversionWidget ocw;
{
    Widget shell, sel;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /* set width/height so that XtRealizeWidget() doesn't cause error */
    shell = XtVaCreatePopupShell("auxShell",
				 transientShellWidgetClass,
				 (Widget)ocw,
				 XtNwidth, 1,
				 XtNheight, 1,
				 XtNallowShellResize, True,
				 NULL);
    ocw->onthespot.auxshell = shell;

    sel = XtCreateManagedWidget("aux", auxPanelWidgetClass,
				shell, NULL, 0);
    (void)XtCreateWidget("display", ocw->ccontrol.displayobjclass, sel,
			 NULL, 0);
    XtAddCallback(sel, XtNcallback, SelectionSelected, (XtPointer)ocw);
    XtInstallAccelerators(sel, (Widget)ocw);

    ocw->onthespot.auxwidget = sel;

    return shell;
}

/*
 *+ inputobject callback
 */

/*- UpdateText: update text -*/
static void
UpdateText(w)
Widget w;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    Widget inputobj = ocw->ccontrol.inputobj;
    int nnew = ICNumSegments(inputobj);
    int nold = ocw->onthespot.numsegments;
    FeedbackAttributes *fba = &ocw->onthespot.feedbackattrs;
    Cardinal cseg, caret;
    ICString *newseg;
    DisplaySegment *dseg;
    Boolean changed;
    int chgseg, chgoffset;
    int oldlen;
    AttributeBuffer *buffer;
    int i;
    int diff;
    Cardinal nsame;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s(nold = %d) nnew=%d\n", __FUNCTION__, nold, nnew));
#else
    TRACE(("OnTheSpotConversion:UpdateText() nnew=%d\n", nnew));
#endif

    ocw->onthespot.fixnotify = False;

    if (!ICCursorPos(inputobj, &cseg, &caret)) {
	cseg = nnew;
	caret = 0;
    }

    allocDisplaySegments(ocw, nnew);

    changed = False;
    chgseg = chgoffset = 0;
    oldlen = 0;
    buffer = allocAttributeBuffer();
    for (i = 0, dseg = ocw->onthespot.dispsegments; i < nnew; i++, dseg++) {
	newseg = ICGetSegment(inputobj, i);
	if (i < cseg) {
	    caret += newseg->nchars;
	}
	if (i >= nold) {
	    copyString(newseg, &dseg->seg);
	    addAttributeBuffer(buffer, inputobj, newseg, 0, fba);
	    if (!changed) {
		chgseg = i;
		chgoffset = 0;
		changed = True;
	    }
	} else {
	    oldlen += dseg->seg.nchars;
	    diff = ICCompareSegment(inputobj, newseg, &dseg->seg, &nsame);
	    switch (diff) {
	    case ICSame:
#ifdef USE_WINIME
TRACE(("    ICSame\n"));
#endif
		if (changed) {
		    addAttributeBuffer(buffer, inputobj, newseg, 0, fba);
		}
		break;
	    case ICAttrChanged:
#ifdef USE_WINIME
TRACE(("    ICAttrChanged\n"));
#endif
		dseg->seg.attr = newseg->attr;
		addAttributeBuffer(buffer, inputobj, newseg, 0, fba);
		if (!changed) {
		    chgseg = i;
		    chgoffset = 0;
		    changed = True;
		}
		break;
	    case ICStringChanged:
#ifdef USE_WINIME
TRACE(("    ICStringChanged\n"));
#endif
		freeString(&dseg->seg);
		copyString(newseg, &dseg->seg);
		if (!changed) {
		    addAttributeBuffer(buffer, inputobj, newseg, nsame, fba);
		    chgseg = i;
		    chgoffset = nsame;
		    changed = True;
		}
		else {
		    addAttributeBuffer(buffer, inputobj, newseg, 0, fba);
		}
		break;
	    default: /* ICAttrChanged | ICStringChanged */
#ifdef USE_WINIME
TRACE(("    ICAttrChanged | ICStringChanged\n"));
#endif
		freeString(&dseg->seg);
		copyString(newseg, &dseg->seg);
		addAttributeBuffer(buffer, inputobj, newseg, 0, fba);
		if (!changed) {
		    chgseg = i;
		    chgoffset = 0;
		    changed = True;
		}
		break;
	    }
	}
    }

    for (; i < nold; i++, dseg++) {
	oldlen += dseg->seg.nchars;
	if (!changed) {
	    chgseg = i;
	    chgoffset = 0;
	    changed = True;
	}
	freeDisplaySegment(dseg);
    }

    ocw->onthespot.numsegments = nnew;

    if (!changed) {
	if (ocw->onthespot.lastcaret != caret) {
	    CBPreeditCaret(ocw, caret);
	}
    }
    else { /* changed */
	Atom encoding = ocw->ccontrol.textencoding;
	int format;
	int length;
	XtPointer string;

	if (ICGetPreeditString(inputobj, chgseg, chgoffset, &encoding, &format,
			       &length, &string) == 0) {

	    dseg = ocw->onthespot.dispsegments;
	    for (i = 0; i < chgseg; i++, dseg++) {
		chgoffset += dseg->seg.nchars;
	    }
	    CBPreeditDraw(ocw, caret, chgoffset, oldlen - chgoffset,
			  encoding, format, length, string,
			  buffer->attrs_length, buffer->attrs_buffer);

	    /* string を free しておく */
	    XtFree((char *)string);
	}
    }

    ocw->onthespot.lastcaret = caret;

    /* buffer を destroy しておく */
    destroyAttributeBuffer(buffer);
}

/*- CommitText: commit text -*/
static void
CommitText(w, arg)
Widget w;
CCTextCallbackArg *arg;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    DisplaySegment *dseg;
    int i, len;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    dseg = ocw->onthespot.dispsegments;
    len = 0;
    for (i = 0; i < ocw->onthespot.numsegments; i++, dseg++) {
	len += dseg->seg.nchars;
    }
    if (len > 0) {
	/* プレエディットバッファの消去 */
	Atom encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
	int format = 8;
	unsigned long attr = 0;
	CBPreeditDraw(ocw, 0, 0, len, encoding, format, 0, (XPointer)"",
		      0, &attr);
    }
    /* 内部のバッファをクリアする */
    clearAllDisplaySegments(ocw);
    ocw->onthespot.lastcaret = 0;

    XtCallCallbackList((Widget)ocw, ocw->ccontrol.textcallback,
		       (XtPointer)arg);

    ocw->onthespot.fixnotify = True;
}

/*- UpdateMode: update mode -*/
static void
UpdateMode(w)
Widget w;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    Widget inputobj = ocw->ccontrol.inputobj;
    Atom encoding = ocw->ccontrol.textencoding;
    int format;
    int length;
    XtPointer string;
    int nchars;

    TRACE(("OnTheSpotConversion:UpdateMode()\n"));

    if (ICGetStatusString(inputobj, &encoding, &format, &length, &string,
			  &nchars) == -1)
	return;

    CBStatusDraw(ocw, encoding, format, length, string, nchars);

    /* string を free しておく */
    XtFree((char *)string);
}

/*- SelectionControl: selection control -*/
static void
SelectionControl(w, arg)
Widget w;
ICSelectionControlArg *arg;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    String params[1];
    Cardinal num_params;

    switch (arg->command) {
    case ICSelectionStart:
	SelectionStart(ocw, arg->u.selection_kind);
	break;
    case ICSelectionEnd:
	SelectionEnd(ocw, &arg->u.current_item);
	break;
    case ICSelectionSet:
	SelectionSet(ocw, arg->u.current_item);
	break;
    case ICSelectionMove:
	SelectionMove(ocw, arg->u.dir);
	break;
    case ICSelectionGet:
	SelectionGet(ocw, &arg->u.current_item);
	break;
#ifdef USE_WINIME
    case ICSelectionCalc:
	SelectionCalc(ocw, arg->u.selection_kind);
	break;
#endif
    default:
	params[0] = XtClass(w)->core_class.class_name;
	num_params = 1;
	XtAppWarningMsg(XtWidgetToApplicationContext(w),
			"parameterError", "SelectionControl", "WidgetError",
			"%s: unknown selection control command",
			params, &num_params);
	break;
    }
}

#ifdef USE_WINIME
/*- SelectionCalc: calc selection pos -*/
/* ARGSUSED */
static void
SelectionCalc(ocw, kind)
OnTheSpotConversionWidget ocw;
int kind;
{
    TRACE(("OnTheSpotConversion:SelectionCalc()\n"));

    LocateSelectionPopup(ocw, kind);
}
#endif

/*- SelectionStart: selection startup -*/
/* ARGSUSED */
static void
SelectionStart(ocw, kind)
OnTheSpotConversionWidget ocw;
int kind;
{
#ifndef USE_WINIME
    Cardinal ncand;
#endif

    TRACE(("OnTheSpotConversion:SelectionStart()\n"));
    if (ocw->onthespot.selectionpoppedup) {
	TRACE(("\tselection already started -- ignored\n"));
	return;
    }

#ifdef USE_WINIME
    LocateSelectionPopup(ocw, kind);
#else
    ocw->onthespot.candlist = ICGetItemList(ocw->ccontrol.inputobj, &ncand);
    ocw->onthespot.numcands = ncand;

    TRACE(("\tnumcands=%d\n", ocw->onthespot.numcands));
    CPanelSetList(ocw->onthespot.selectionwidget,
		  ocw->onthespot.candlist,
		  ocw->onthespot.numcands, 0, True);

    LocateSelectionPopup(ocw);
    XtPopup(ocw->onthespot.selectionshell, XtGrabNone);
#endif
    ocw->onthespot.selectionpoppedup = True;
}

/*- LocateSelectionPopup: put selection popup at an appropriate position -*/
static void
#ifdef USE_WINIME
LocateSelectionPopup(ocw, listnum)
OnTheSpotConversionWidget ocw;
int listnum;
#else
LocateSelectionPopup(ocw)
OnTheSpotConversionWidget ocw;
#endif
{
    Position x, y;
    int clx, cly;
    Dimension dpyWidth, dpyHeight;
    Widget panel = ocw->onthespot.selectionwidget;
    Widget shell = ocw->onthespot.selectionshell;
    Window junk;
    int barheight = ocw->ccontrol.titlebarheight;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    (void)XTranslateCoordinates(XtDisplay(ocw),
				ocw->ccontrol.clientwindow,
				RootWindowOfScreen(XtScreen(ocw)),
				0, 0, &clx, &cly, &junk);

    (void)SafeGetWindowAttributes(XtDisplay(ocw), ocw->ccontrol.clientwindow,
				  &(ocw->ccontrol.client_attr));

    x = clx;
    y = cly + ocw->ccontrol.client_attr.height;

    dpyWidth = WidthOfScreen(XtScreen(shell));
    dpyHeight = HeightOfScreen(XtScreen(shell));

    if (x + panel->core.width > (int)dpyWidth) x = dpyWidth - panel->core.width;
    if (x < 0) x = 0;
#if 0
    if (y + panel->core.height + barheight > (int)dpyHeight) {
	y = cly - panel->core.height - 8 - barheight - 20; /* XXX */
	if (y < 0) y = dpyHeight - panel->core.height - barheight;
    }
#endif
    if (y + panel->core.height + barheight > (int)dpyHeight)
	y = dpyHeight - panel->core.height - 8 - barheight - 20; /* XXX */
    if (y < 0) y = 0;
#ifdef USE_WINIME
    XWinIMESetCandidateWindow(XtDisplay(ocw), -1, x, y, listnum);
#else
    MoveShell(shell, x, y);
#endif
}

/*- SelectionEnd: selection finish -*/
static void
SelectionEnd(ocw, current)
OnTheSpotConversionWidget ocw;
int *current;
{
    TRACE(("OnTheSpotConversion:SelectionEnd()\n"));

    if (!ocw->onthespot.selectionpoppedup) {	/* for safe */
	TRACE(("\tnot in selection mode -- ignored\n"));
	return;
    }

#ifndef USE_WINIME
    XtVaGetValues(ocw->onthespot.selectionwidget,
		  XtNcurrentItem, current,
		  NULL);

    XtPopdown(ocw->onthespot.selectionshell);
#endif

    ocw->onthespot.selectionpoppedup = False;
}

/*- SelectionSet: set current selection item -*/
static void
SelectionSet(ocw, current)
OnTheSpotConversionWidget ocw;
int current;
{
    TRACE(("OnTheSpotConversion:SelectionSet()\n"));

    if (!ocw->onthespot.selectionpoppedup) {	/* for safe */
	TRACE(("\tnot in selection mode -- ignored\n"));
	return;
    }

#ifndef USE_WINIME
    XtVaSetValues(ocw->onthespot.selectionwidget,
		  XtNcurrentItem, current,
		  NULL);
#endif
}

/*- SelectionGet: get current selection item -*/
static void
SelectionGet(ocw, current)
OnTheSpotConversionWidget ocw;
int *current;
{
    TRACE(("OnTheSpotConversion:SelectionGet()\n"));

    if (!ocw->onthespot.selectionpoppedup) {	/* for safe */
	TRACE(("\tnot in selection mode -- ignored\n"));
	return;
    }

#ifndef USE_WINIME
    XtVaGetValues(ocw->onthespot.selectionwidget,
		  XtNcurrentItem, current,
		  NULL);
#endif
}

/*- SelectionMove: move crrent selection to specified direction -*/
static void
SelectionMove(ocw, dir)
OnTheSpotConversionWidget ocw;
int dir;
{
    TRACE(("OnTheSpotConversion:SelectionMove()\n"));

    if (!ocw->onthespot.selectionpoppedup) {	/* for safe */
	TRACE(("\tnot in selection mode -- ignored\n"));
	return;
    }

#ifndef USE_WINIME
    CPanelMoveCurrent(ocw->onthespot.selectionwidget, dir);
#endif
}

/*
 * Aux Callback
 */

static void
AuxControl(w, arg)
Widget w;
ICAuxControlArg *arg;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)w;
    String params[1];
    Cardinal num_params;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    switch (arg->command) {
    case ICAuxStart:
	AuxStart(ocw);
	break;
    case ICAuxEnd:
	AuxEnd(ocw);
	break;
    case ICAuxChange:
	AuxChange(ocw);
	break;
    default:
	params[0] = XtClass(w)->core_class.class_name;
	num_params = 1;
	XtAppWarningMsg(XtWidgetToApplicationContext(w),
			"parameterError", "AuxControl", "WidgetError",
			"%s: unknown aux control command",
			params, &num_params);
	break;
    }
}

/* ARGSUSED */
static void
AuxStart(ocw)
OnTheSpotConversionWidget ocw;
{
  ICString *auxstr;
  Cardinal ncand, curseg, cursorpos;
  
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
  if (ocw->onthespot.auxpoppedup) return;
  
  /* テキストコールバックの時のような処理をする
     のは AuxPanel.c にまかせよう */

  auxstr = ICGetAuxSegments(ocw->ccontrol.inputobj,
			    &ncand, &curseg, &cursorpos);

  APanelStart(ocw->onthespot.auxwidget, auxstr, ncand, curseg, cursorpos);

  /* ポップアップする場所を決める */
  LocateAuxPopup(ocw);

  XtPopup(ocw->onthespot.auxshell, XtGrabNone);
  ocw->onthespot.auxpoppedup = True;
}

/* ARGSUSED */
static void
AuxEnd(ocw)
OnTheSpotConversionWidget ocw;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
  if (!ocw->onthespot.auxpoppedup) return;	/* for safe */

/*  APanelEnd(ocw->onthespot.auxwidget); */

  XtPopdown(ocw->onthespot.auxshell);

  ocw->onthespot.auxpoppedup = False;
}

/* ARGSUSED */
static void
AuxChange(ocw)
OnTheSpotConversionWidget ocw;
{
  Cardinal ncand, curseg, cursorpos;
  ICString *auxstr;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
  if (!ocw->onthespot.auxpoppedup) return;	/* for safe */

  auxstr = ICGetAuxSegments(ocw->ccontrol.inputobj,
			    &ncand, &curseg, &cursorpos);

  APanelChange(ocw->onthespot.auxwidget, auxstr, ncand, curseg, cursorpos);
}

/*- LocateAuxPopup: put aux popup at an appropriate position -*/
static void
LocateAuxPopup(ocw)
OnTheSpotConversionWidget ocw;
{
    Position x, y;
    int clx, cly;
    Dimension dpyWidth, dpyHeight;
    Widget panel = ocw->onthespot.auxwidget;
    Widget shell = ocw->onthespot.auxshell;
    Window junk;
    int barheight = ocw->ccontrol.titlebarheight;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    (void)XTranslateCoordinates(XtDisplay(ocw),
				ocw->ccontrol.clientwindow,
				RootWindowOfScreen(XtScreen(ocw)),
				0, 0, &clx, &cly, &junk);

    (void)SafeGetWindowAttributes(XtDisplay(ocw), ocw->ccontrol.clientwindow,
				  &(ocw->ccontrol.client_attr));

    x = clx;
    y = cly + ocw->ccontrol.client_attr.height;

    dpyWidth = WidthOfScreen(XtScreen(shell));
    dpyHeight = HeightOfScreen(XtScreen(shell));

    if (x + panel->core.width > (int)dpyWidth) x = dpyWidth - panel->core.width;
    if (x < 0) x = 0;
#if 0
    if (y + panel->core.height + barheight > (int)dpyHeight) {
	y = cly - panel->core.height - 8 - barheight - 20; /* XXX */
	if (y < 0) y = dpyHeight - panel->core.height - barheight;
    }
#endif
    if (y + panel->core.height + barheight > (int)dpyHeight)
	y = dpyHeight - panel->core.height - 8 - barheight - 20; /* XXX */
    if (y < 0) y = 0;
    MoveShell(shell, x, y);
}

/*
 *+ Selection Widget callback
 */

/*- SelectionSelected: selection selected callback -*/
/* ARGSUSED */
static void
SelectionSelected(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    OnTheSpotConversionWidget ocw = (OnTheSpotConversionWidget)client_data;
    int current = (int)call_data;

    TRACE(("OnTheSpotConversion:SelectionSelected()\n"));
    XtPopdown(ocw->onthespot.selectionshell);
    ocw->onthespot.selectionpoppedup = False;
    ICSelectItem(ocw->ccontrol.inputobj, current);
}

/*
 *+ miscelaneous functions
 */

/*- SafeGetWindowAttributes: get window attributes -*/
static Boolean
SafeGetWindowAttributes(dpy, w, attr)
Display *dpy;
Window w;
XWindowAttributes *attr;
{
    XAEHandle h;
    unsigned long errbits = 0;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    h = XAESetRecordErrors(dpy, &errbits);
    (void)XGetWindowAttributes(dpy, w, attr);
    XAEUnset(h);

    return (errbits == 0);
}

/*- MoveShell: move shell widget -*/
static void
MoveShell(w, x, y)
Widget w;
Position x;
Position y;
{
    XtWidgetGeometry req;

    /*
     * calling XtMoveWidget() is NOT enough to move shell widgets.
     * we must use XtMakeGeometryRequest() or XtSetValues() to
     * invoke root-geometry-manager which modifies the size hint
     * appropriately.
     */
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    req.request_mode = CWX | CWY;
    req.x = x;
    req.y = y;
    (void)XtMakeGeometryRequest(w, &req, (XtWidgetGeometry *)NULL);
}

/*- getToplevelWindow: get top-level window of a given window -*/
static Window
getToplevelWindow(dpy, win)
Display *dpy;
Window win;
{
    Atom wm_state;
    Atom type;
    int format;
    unsigned long nitems, bytesafter;
    unsigned char *data;
    Window root, parent;
    Window *children;
    unsigned int nchildren;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    /*
     * find toplevel window which has WM_STATE property or if no exists,
     * direct subwindow of the root window. (ie I assume that if a
     * window manager is running, that is a ICCCM compliant one)
     */
    wm_state = CachedInternAtom(dpy, "WM_STATE", True);
    for (;;) {
	type = None;
	if (wm_state != None) {
	    data = NULL;
	    XGetWindowProperty(dpy, win, wm_state, 0L, 0L, False,
			       AnyPropertyType, &type, &format,
			       &nitems, &bytesafter, &data);
	    if (data != NULL) XtFree((char *)data);
	    if (type != None) break;
	}
	if (!XQueryTree(dpy, win, &root, &parent, &children, &nchildren)) break;
	if (nchildren > 0) XtFree((char *)children);
	if (root == parent) break;
	win = parent;
    }
    return win;
}

/*- setTransientFor: set WM_TRANSIENT_FOR property to specified widget -*/
static void
setTransientFor(w, win)
Widget w;
Window win;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (!XtIsRealized(w)) XtRealizeWidget(w);
    XSetTransientForHint(XtDisplay(w), XtWindow(w), win);
}

/*- allocDisplaySegments: prepare specified number of display segments -*/
static void
allocDisplaySegments(ocw, n)
OnTheSpotConversionWidget ocw;
Cardinal n;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.dispsegmentsize > n) return;
    n = ((n + 3) / 4) * 4 ;
    if (ocw->onthespot.dispsegments == NULL) {
	ocw->onthespot.dispsegments = (DisplaySegment *)XtMalloc(n * sizeof(DisplaySegment));
    } else {
	ocw->onthespot.dispsegments = (DisplaySegment *)XtRealloc((char *)ocw->onthespot.dispsegments, n * sizeof(DisplaySegment));
    }
    ocw->onthespot.dispsegmentsize = n;
}

/*- freeDisplaySegment: free display segment's contents -*/
static void
freeDisplaySegment(dsp)
DisplaySegment *dsp;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    freeString(&dsp->seg);
}

/*- clearAllDisplaySegments: clear all display segment's -*/
static void
clearAllDisplaySegments(ocw)
OnTheSpotConversionWidget ocw;
{
    DisplaySegment *dsp = ocw->onthespot.dispsegments;
    int i;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    for (i = 0; i < ocw->onthespot.numsegments; i++) {
	freeDisplaySegment(dsp++);
    }
    ocw->onthespot.numsegments = 0;
}

/*- copyString: copy ICString -*/
static void
copyString(from, to)
ICString *from;
ICString *to;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    *to = *from;
    to->data = XtMalloc(to->nbytes);
    (void)bcopy(from->data, to->data, to->nbytes);
}

/*- freeString: free ICString -*/
static void
freeString(seg)
ICString *seg;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    XtFree(seg->data);
    seg->data = NULL;
    seg->nbytes = 0;
}

/*- allocAttributeBuffer: allocate attribute buffer -*/
static AttributeBuffer *
allocAttributeBuffer()
{
    AttributeBuffer *buffer;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    buffer = (AttributeBuffer *)XtMalloc(sizeof(AttributeBuffer));
    buffer->attrs_length = 0;
    buffer->attrs_limit = 64;
    buffer->attrs_buffer = (unsigned long *)XtMalloc(buffer->attrs_limit * sizeof(unsigned long));
    return buffer;
}

/*- destroyAttributeBuffer: destroy draw string buffer's contents -*/
static void
destroyAttributeBuffer(buffer)
AttributeBuffer *buffer;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    XtFree((char *)buffer->attrs_buffer);
    XtFree((char *)buffer);
}

/*- addAttributeBuffer: add segment to draw string buffer's contents -*/
static void
addAttributeBuffer(buffer, inputobj, seg, offset, fba)
AttributeBuffer *buffer;
Widget inputobj;
ICString *seg;
int offset;
FeedbackAttributes *fba;
{
    int nchars = seg->nchars - offset;
    unsigned long fb;
    int i;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (buffer->attrs_length + nchars > buffer->attrs_limit) {
	unsigned long *new_attrs_buffer;
	int new_limit = buffer->attrs_limit * 2;
	if (new_limit < buffer->attrs_length + nchars)
	    new_limit = buffer->attrs_length + nchars + 8;
	new_attrs_buffer = (unsigned long *)XtMalloc(new_limit * sizeof(unsigned long));
	bcopy(buffer->attrs_buffer, new_attrs_buffer,
	      buffer->attrs_length * sizeof(unsigned long));
	XtFree((char *)buffer->attrs_buffer);
	buffer->attrs_limit = new_limit;
	buffer->attrs_buffer = new_attrs_buffer;
    }
    fb = attrToFeedback(fba, seg->attr);
    for (i = 0; i < nchars; i++) {
	buffer->attrs_buffer[buffer->attrs_length + i] = fb;
    }
    buffer->attrs_length += nchars;
}

/*- attrToFeedback: ICString attribute -> XIMFeedback attribute converter -*/
static unsigned long
attrToFeedback(fba, attr)
FeedbackAttributes *fba;
int attr;
{
    int idx;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (attr == ICAttrNormalString) return 0;

    if (!(attr & ICAttrConverted)) {
#ifdef USE_WINIME
TRACE(("    FEEDBACK_NOCONV\n"));
#endif
	/* Not yet converted */
	idx = FEEDBACK_NOCONV;
    } else if (attr & ICAttrCurrentSegment) {
#ifdef USE_WINIME
TRACE(("    FEEDBACK_CURRENT\n"));
#endif
	/* it's converted and the current segment */
	idx = FEEDBACK_CURRENT;
    } else if (attr & ICAttrCurrentSubSegment) {
#ifdef USE_WINIME
TRACE(("    FEEDBACK_CURRENTSUB\n"));
#endif
	/* it's converted and the current sub segment */
	idx = FEEDBACK_CURRENTSUB;
    } else {
#ifdef USE_WINIME
TRACE(("    FEEDBACK_CONV\n"));
#endif
	/* converted, not current */
	idx = FEEDBACK_CONV;
    }
    return fba->feedbacks[idx];
}

/*- CBPreeditStart: callback preedit start -*/
static void
CBPreeditStart(ocw)
OnTheSpotConversionWidget ocw;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.preeditstartcallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNpreeditStartCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBPreeditStart()\n"));

    XtCallCallbackList((Widget)ocw, ocw->onthespot.preeditstartcallback,
		       (XtPointer)NULL);
}

/*- CBPreeditDone: callback preedit done -*/
static void
CBPreeditDone(ocw)
OnTheSpotConversionWidget ocw;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.preeditdonecallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNpreeditDoneCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBPreeditDone()\n"));

    XtCallCallbackList((Widget)ocw, ocw->onthespot.preeditdonecallback,
		       (XtPointer)NULL);
}

/*- CBPreeditDraw: callback preedit draw -*/
static void
CBPreeditDraw(ocw, caret, chg_first, chg_length, encoding, format, text_length, text, attrs_length, attrs)
OnTheSpotConversionWidget ocw;
int caret;
int chg_first;
int chg_length;
Atom encoding;
int format;
int text_length;
XtPointer text;
int attrs_length;
unsigned long *attrs;
{
    OCCPreeditDrawArg arg;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.preeditdrawcallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNpreeditDrawCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

#ifndef USE_WINIME
    TRACE(("OnTheSpot:CBPreeditDraw()\n"));
#endif

    arg.caret = caret;
    arg.chg_first = chg_first;
    arg.chg_length = chg_length;
    arg.encoding = encoding;
    arg.format = format;
    arg.text_length = text_length;
    arg.text = text;
    arg.attrs_length = attrs_length;
    arg.attrs = attrs;
#ifdef USE_WINIME
TRACE(("  caret = %d\n", caret));
TRACE(("  chg_first = %d\n", chg_first));
TRACE(("  chg_length = %d\n", chg_length));
//TRACE(("  encoding = %s\n", encoding));
TRACE(("  format = %d\n", format));
TRACE(("  text_length = %d\n", text_length));
//TRACE(("  text = %s\n", text));
TRACE(("  attrs_length = %d\n", attrs_length));
TRACE(("  attrs = %ld\n", *attrs));
#endif
    XtCallCallbackList((Widget)ocw, ocw->onthespot.preeditdrawcallback,
		       (XtPointer)&arg);
}

/*- CBPreeditCaret: callback preedit caret -*/
static void
CBPreeditCaret(ocw, caret)
OnTheSpotConversionWidget ocw;
int caret;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.preeditcaretcallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNpreeditCaretCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBPreeditCaret()\n"));

    XtCallCallbackList((Widget)ocw, ocw->onthespot.preeditcaretcallback,
		       (XtPointer)caret);
}

/*- CBStatusStart: callback status start -*/
static void
CBStatusStart(ocw)
OnTheSpotConversionWidget ocw;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.statusstartcallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNstatusStartCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBStatusStart()\n"));

    XtCallCallbackList((Widget)ocw, ocw->onthespot.statusstartcallback,
		       (XtPointer)NULL);
}

/*- CBStatusDone: callback status done -*/
static void
CBStatusDone(ocw)
OnTheSpotConversionWidget ocw;
{
#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.statusdonecallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNstatusDoneCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBStatusDone()\n"));

    XtCallCallbackList((Widget)ocw, ocw->onthespot.statusdonecallback,
		       (XtPointer)NULL);
}

/*- CBStatusDraw: callback status draw -*/
static void
CBStatusDraw(ocw, encoding, format, length, text, nchars)
OnTheSpotConversionWidget ocw;
Atom encoding;
int format;
int length;
XtPointer text;
int nchars;
{
    OCCPreeditDrawArg arg;

#ifdef USE_WINIME
TRACE(("OnTheSpot:%s()\n", __FUNCTION__));
#endif
    if (ocw->onthespot.statusdrawcallback == NULL ||
	XtHasCallbacks((Widget)ocw, XtNstatusDrawCallback) != XtCallbackHasSome)
    {
	/* no callback */
	return;
    }

    TRACE(("OnTheSpot:CBStatusDraw()\n"));

    arg.caret = 0;	/* ignored */
    arg.chg_first = 0;	/* ignored */
    arg.chg_length = 0;	/* ignored */
    arg.encoding = encoding;
    arg.format = format;
    arg.text_length = length;
    arg.text = text;
    arg.attrs_length = nchars;
    arg.attrs = NULL;	/* ignored */
    XtCallCallbackList((Widget)ocw, ocw->onthespot.statusdrawcallback,
		       (XtPointer)&arg);
}

