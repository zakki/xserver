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

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author: Akira Kon, NEC Corporation.  (kon@d1.bs2.mt.nec.co.jp)
 */

/* 
 * Copyright 1999 Justsystem Corporation, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Justsystem Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Justsystem 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * Author: Atsushi Irisawa
 */

#ifndef lint
static char *rcsid = "$Id: Atok.c,v 1.3 1999/08/24 08:59:35 ishisone Exp $";
#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#define XK_KATAKANA
#include <X11/keysym.h>
#if XtSpecificationRelease > 4
#include <X11/Xfuncs.h>
#endif
#include "AtokP.h"
#include "key_def.h"
#include "DebugPrint.h"

#define _WCHAR_T
#define wchar_t wchar

#ifdef SVR4
#define bzero(p, l)	memset(p, 0, l)
#else
#if defined(SYSV) || defined(USG)
#define OVERLAP_BCOPY
extern char *memset();
#define bzero(p, l)	memset(p, 0, l)
#endif
#endif

static XtResource resources[] = {
#define offset(field) XtOffset(AtokObject, atok.field)
    { XtNAtokServer, XtCAtokServer, XtRString, sizeof(String),
	offset(atokserver), XtRString, NULL },
    { XtNConfFile, XtCConfFile, XtRString, sizeof(String),
	offset(conffile), XtRString, NULL },
    { XtNStyleFile, XtCStyleFile, XtRString, sizeof(String),
	offset(stylefile), XtRString, NULL },
    { XtNPort, XtCPort, XtRString, sizeof(String),
	offset(port), XtRString, NULL },
#undef offset
};

static void ClassInitialize();
static void Initialize();
static void Destroy();
static Boolean SetValues();
static int InputEvent();
static ICString *GetMode();
static int CursorPos();
static int NumSegments();
static ICString *GetSegment();
static int CompareSegment();
static ICString *GetItemList();
static int SelectItem();
static int ConvertedString();
static int ClearConversion();
static ICString *GetAuxSegments();
static int PreeditString();
static int StatusString();

AtokClassRec atokClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass) &inputConvClassRec,
    /* class_name		*/	"Atok",
    /* widget_size		*/	sizeof(AtokRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1			*/	NULL,
    /* obj2			*/	NULL,
    /* obj3			*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* obj4			*/	FALSE,
    /* obj5			*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7			*/	FALSE,
    /* destroy			*/	Destroy,
    /* obj8			*/	NULL,
    /* obj9			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11			*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* obj12			*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
  { /* inputConv fields */
    /* InputEvent		*/	InputEvent,
    /* GetMode			*/	GetMode,
    /* CursorPos		*/	CursorPos,
    /* NumSegments		*/	NumSegments,
    /* GetSegment		*/	GetSegment,
    /* CompareSegment		*/	CompareSegment,
    /* GetItemList		*/	GetItemList,
    /* SelectItem		*/	SelectItem,
    /* GetConvertedString	*/	ConvertedString,
    /* ClearConversion		*/	ClearConversion,
    /* GetAuxSegments		*/	GetAuxSegments,
    /* SupportMultipleObjects	*/	True,
    /* GetTriggerKeys		*/	XtInheritGetTriggerKeys,
    /* num_trigger_keys		*/	0,
    /* trigger_keys		*/	NULL,
    /* GetPreeditString		*/	PreeditString,
    /* GetStatusString		*/	StatusString,
    /* NoMoreObjects		*/	False,
  },
  { /* atok fields */
    /* foo			*/	0,
  }
};

WidgetClass atokObjectClass = (WidgetClass)&atokClassRec;

static int bell();
static void fix();
static void fixProcForAtok();
static void convend();

static void addObject();
static void deleteObject();

static void atokDisplay();
static int atokCandDisplay();
static int atokAuxDisplay();

static void moveLeft();
static void moveRight();
static void setSelection();
static void moveSelection();
static void querySelection();
static void endSelection();
static void insertSelection();
static int allocCandList();
static int changeTextForAtok();
static int changeTextForAtokAUX();
static void startSelection();
static int makeCandList();

static void copyInWchar();

static Display *displaybell = (Display *)0;

static int clientID = -1;
static int nAtokContexts = 0;
static int ceSock = -1;


static void
ClassInitialize()
{
    DPRINT(( "ATOK Class Initialize\n" ));
    TRACE(("AtokObjectClass initialized\n"));
    /* $B2?$b$7$J$$(B */
}

/* ARGSUSED */
static void
Initialize(req, new, args, num_args)
Widget req;
Widget new;
ArgList args;
Cardinal *num_args;
{
    AtokObject obj = (AtokObject)new;
    AtokObject objX = (AtokObject)req;
    int sock;
    menuAux *orgMenuAux;
    char buf[256];
    int isConnect = 1;
    int i;

    bzero((char *)&obj->atok, sizeof(obj->atok));

    obj->atok.textchanged = False;
    obj->atok.nbr_of_cand = 0;
    obj->atok.cand_lists_ics = NULL;
    obj->atok.cur_cand = 0;

    for (i = 0; i < NBR_OF_PART; i++) {
	obj->atok.aux_ics[i].data = (char *)NULL;
	obj->atok.aux_ics[i].nchars = 0;
	obj->atok.aux_ics[i].nbytes = 0;
	obj->atok.aux_length[i] = 0;
	obj->atok.aux_size[i] = 0;
    }

    if (objX->atok.atokserver)
	obj->atok.atokserver = objX->atok.atokserver;
    if (objX->atok.port)
	obj->atok.port = objX->atok.port;
    if (objX->atok.conffile)
	obj->atok.conffile = objX->atok.conffile;

    /* $BF~NO%P%C%U%!$r:n@.$9$k!#(B*/
    obj->atok.comm = (_XatokRecPtr)XtMalloc(sizeof(_XatokRec));
    bzero((char *)obj->atok.comm, sizeof(_XatokRec));
    obj->atok.comm->echoLen = 0;
    /* obj->atok.sock = ceSock; */

    if (!nAtokContexts) {
	ceSock = atokConnect(obj->atok.atokserver,
			     obj->atok.port,
			     obj->atok.conffile,
			     obj->atok.stylefile,
			     /*
			     obj->atok.conffile ?
				obj->atok.conffile : "atok12.conf",
			     obj->atok.stylefile ?
				obj->atok.stylefile : "atok12.sty",
			     */
			     obj->atok.comm);
	if (ceSock < 0) {
	    XtAppWarning(XtWidgetToApplicationContext((Widget)obj),
			 "Cannot connect to ATOK server");
	    /* exit(0); */
	    XtFree((char *)obj->atok.comm);
	    obj->atok.comm = (_XatokRecPtr)NULL;
	    return;
	}
	isConnect = 0;
	obj->atok.comm->sock = ceSock;
	clientID = obj->atok.comm->NETrqst.clntid;
    }

    obj->atok.comm->sock = ceSock;

    addObject(obj, ceSock, clientID, isConnect);

    orgMenuAux = getMenuInstance();
    obj->atok.comm->menuAux = auxMenuCopy(orgMenuAux);
    obj->atok.comm->curAux = obj->atok.comm->menuAux;
    XtCallCallbackList((Widget) obj, obj->inputConv.modechangecallback,
		       (XtPointer)NULL);
    nAtokContexts++;
}

static void
Destroy(w)
Widget w;
{
    AtokObject obj = (AtokObject)w;

    if (obj->atok.comm) XtFree((char *)obj->atok.comm);
    deleteObject(obj);
}

/* ARGSUSED */
static Boolean SetValues(cur, req, wid, args, num_args)
Widget cur;
Widget req;
Widget wid;
ArgList args;
Cardinal *num_args;
{
    AtokObject old = (AtokObject)cur;
    AtokObject new = (AtokObject)wid;

    return False;
}


static int
XKanaLookup(event_struct, buffer_return, bytes_buffer,
	    keysym, status_return)
XKeyEvent *event_struct;
char *buffer_return;
int bytes_buffer;
KeySym *keysym;
XComposeStatus *status_return;
{
  int res;
  res = XLookupString(event_struct, buffer_return, bytes_buffer,
		      keysym, status_return);
  if (!res && XK_overline <= *keysym && *keysym <= XK_semivoicedsound) {
    buffer_return[0] = (unsigned long)(*keysym) & 0xff;
    res = 1;
  }
  return res;
}


/*
 * Kinput2 API Functions
 */

static int
InputEvent(w, event)
Widget w;
XEvent *event;
{
    AtokObject obj = (AtokObject)w;
    wchar wbuf[BUFSIZ*4];	/* $B3NDjJ8;zNs$N%P%C%U%!(B */
    int wlen;			/* $B3NDjJ8;zNs$ND9$5(B */
    unsigned char kbuf[BUFSIZ];	/* $B%-!<F~NO%G!<%?(B */
    int nbytes;			/* $B%-!<F~NO$ND9$5(B  */
    KeySym ks;			/* X$B$N%-!<%7%s%\%k(B */
    XComposeStatus comp_status;		
    WORD aajcode;		/* AAJCODE */
    int kanjiStatus;
    int enableStatus;
    int changeStatus;
    int elseEvent;
    int status;
    int isThrue = 0;

    DPRINT(( "\n### NEW EVENT \n" ));

    /* KeyPress$B0J30$O<N$F$k(B */
    if (event->type != KeyPress /*&& event->type != KeyRelease*/) return 0;

    obj->atok.textchanged = False;

    /*
     * AtokServer $B$,(B $B:F5/F0$5$l$?$P$"$$$J$I!"%3%M%/%7%g%s$,$-$l$F$$$k>l9g$,(B
     * $B$"$k$N$G(B $B:FEY%3%M%/%7%g%s$rD%$C$F$_$k!#(B
     */
    if (obj->atok.comm == NULL) {
	/* return -1; /* $B@\B3$G$-$F$$$J$$$H$-$O!"%(%i!<$rJV$9(B */
	return 1; /* $B@\B3$G$-$F$$$J$$$H$-$O!"L5;k$9$k(B */
    }

    /* X $B$N%-!<%$%Y%s%H$r<hF@$9$k!#(B*/
    kbuf[0] = '\0';
    nbytes = XKanaLookup((XKeyEvent*)event, kbuf, sizeof(kbuf), &ks,
			 &comp_status);

    wbuf[0] = (wchar)kbuf[0]; /* $B$-$?$J$$(B */

    DPRINT(("nbytes %d\n", nbytes));

    /* $BMW$i$J$$>l9g$OL5;k$9$k(B */
    if (nbytes == 0 && ks & 0xff00 == 0) return 1;

    /* $B%Y%k$rLD$i$9%G%#%9%W%l%$$N@_Dj(B */
    displaybell = XtDisplayOfObject((Widget)obj);

    /*
     * $B8uJd%&%#%s%I%&$,(BACTIVE $B$K$J$C$F$$$?>l9g$N8uJd$N0\F0$O(B
     * Kinput2 $B$h$j(B Callback $B$,8F$S=P$5$l$:!"A*Br$5$l$F$$$kHV9f$,(B
     * $B<hF@$G$-$J$$$N$G!"$3$3$G0lEY(B $BA*Br$5$l$F$$$kHV9f$r<hF@$9$k!#(B
     */
    if (obj->atok.comm->convStatus & M_CAND) {
	querySelection(obj);
    }

    /*
     * $B%-!<F~NO$r85$K(B AAJCODE$B$KJQ49$7!"(BCE$B$X$N%j%/%(%9%H$r9T$&!#(B
     * $B%j%/%(%9%H7k2L$K4p$E$$$F!"I=<($d%b!<%I$rJQ99$9$k!#(B
     * $B3NDjJ8;zNs$,$"$C$?>l9g$K$O!"%j%?!<%s$H$7$F!"$=$NJ8;zNs$ND9$5$,(B
     * $BJV$C$F$/$k!#(B
     * $B$3$l0J9_$N=hM}$O(B $B2hLL$KI=<($9$k$?$a$NJ*$G$"$k!#(B
     */
    wlen = XatokEventControl(obj->atok.comm, event, ks, kbuf, nbytes,
			     &aajcode, &kanjiStatus, &enableStatus,
			     &changeStatus, &elseEvent);

    if (!aajcode) {
	/* Can't convertsion to AAJ CODE */
	return 0;
    }

    if (obj->atok.comm->convStatus == M_NOKEYCONV) {
	convend(obj);
	return 0;
    }
		
    /*
     * $B$3$3$+$i2<$O(B kanjiStatus $B$G;XDj$5$l$F$$$k%b!<%I$N=hM}$r(B
     * $B9T$$!"2hLL$KI=<($9$k!#(B
     * $B8uJd0lMw!"!"<-=q!"3NDj$J$I$K2r$l$k!#(B
     *
     * $BDL>o$N%-!<F~NO$N>l9g(B
     * $B8uJd0lMwI=<(Cf$N>l9g(B
     * $B<-=q%f!<%F%#%j%F%#!<I=<(Cf$N>l9g(B
     */
    switch(kanjiStatus) {
    case KRS_UNCHANGED: /* $B%9%k!<3NDj(B */
	isThrue = 1 ;
	obj->atok.comm->wlen = 0 ;
	obj->atok.comm->wbuf[0] = aajcode ;
	break;
    case KRS_BACKSPACE:
    case KRS_SOFTBACKSPACE:
    case KRS_SOFTCARRIAGERETURN:
    case KRS_EXTTANGO_OPEN:
    default:
	break;
    }
    status = 0;
    if (enableStatus) {
	if (elseEvent & EVENTB_BEEP) {
	    bell();
	}
	/* $B%b!<%I$NJQ99$,$"$C$?(B */
	if (changeStatus & ATCHANGEB_MODESTR) {
	    XtCallCallbackList((Widget)obj, obj->inputConv.modechangecallback,
			       (XtPointer)NULL);
	}
	if (changeStatus & ATCHANGEB_RESULTSTR) {
	    /* do nothing */
	}
	if (changeStatus & ATCHANGEB_COMPSTR) {
	    /* do nothing */
	}
	/* $B<!8uJd%&%#%s%I%&99?7(B */
	if (changeStatus &
	    (ATCHANGEB_RECTOPEN | ATCHANGEB_RECTCLOSE | ATCHANGEB_RECTCHANGE))
	{
	    /* do nothing */
	}
	if (changeStatus & (ATCHANGEB_SYSLSTR | ATCHANGEB_SYSLTOSYSL)) {
	    /*
	     * auxSyslineCtrl() $B$N(B return $B$,#00J30$N;~$O!"(B
	     * $B%7%9%F%`9T$NI=<($,=*N;$N;~$G$"$j!"(B
	     * $B3NDjJ8;zNs$,B8:_$9$k2DG=@-$,$"$k$N$G(B
	     * $B%7%9%F%`9T$r>C$7$F$+$i(B
	     * AuxDisplay()$B$r8F$S=P$9I,MW$,$"$k!#(B
	     */
	    status = auxSyslineCtrl(obj->atok.comm,
				    changeStatus & ATCHANGEB_RESULTSTR);
	    atokAuxDisplay(obj, M_SYSLINE);
	    /*
	    if (!status) {
		DPRINT(("SYSLINE END\n"));
		return 0;
	    }
	    */
	}
    }
    /*
     * $B$3$3$+$i2<$O(B $BI=<($N%3%s%H%m!<%k(B
     * $BF~NO$5$l$?%-!<$K$h$j=hM}$rJ,$1$F$$$k!#(B
     */
    if (obj->atok.comm->convStatus & M_CAND) {
	status = atokCandDisplay(obj, (int)aajcode);
	if (status < 1) {
	    return 0;
	}
    }
    /* else */
    if (obj->atok.comm->convStatus & M_AUX) {
	status = atokAuxDisplay(obj, obj->atok.comm->convStatus);
	if (status < 1) {
	    return;
	}
    }
    else if (obj->atok.comm->convStatus & M_SYSLINE && !status) {
	status = auxSyslineCtrl(obj->atok.comm,
				changeStatus & ATCHANGEB_RESULTSTR);
	atokAuxDisplay(obj, obj->atok.comm->convStatus);
	if (!status) {
	    return 0;
	}
    }
    else if (kanjiStatus == XKEY_DICT) {
	/* $B<-=q%f!<%F%#%j%F%#!<I=<(;~$N%-!<F~NO(B */
    }
    /* $B$=$NB>$N>l9g$N%-!<F~NO(B */
    if (obj->atok.comm->convStatus & M_KEYCONV) {
	atokDisplay(obj, &kanjiStatus);
    }

    return isThrue;
}

static ICString	*
GetMode(w)
Widget w;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;
    int len;
    static ICString icstr;

    if (acomm == NULL) return NULL; /* Not connected */

    icstr.data = (char *)XatokGetModeStr(acomm, &len);
    icstr.nchars = len;
    icstr.nbytes = icstr.nchars * sizeof(wchar);
    icstr.attr = ICAttrNormalString;

    return &icstr;
}

static int
CursorPos(w, nsegp, ncharp)
Widget w;
Cardinal *nsegp;
Cardinal *ncharp;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;

    if (acomm == NULL) return 0; /* Not connected */

    return XatokGetSegmentPosition(acomm, nsegp, ncharp);
}

static int
NumSegments(w)
Widget w;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;

    if (acomm == NULL) return 0; /* Not connected */

    return XatokGetSegmentNumber(acomm);
}

static ICString *
GetSegment(w, n)
Widget w;
Cardinal n;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;
    int len;
    int attr;
    static ICString seg;

    if (acomm == NULL) return NULL; /* Not connected */

    seg.data = (char *)XatokGetSegmentRec(acomm, n, &len, &attr);
    seg.nchars = len;
    seg.nbytes = seg.nchars * sizeof(wchar);
    seg.attr = attr;

    return &seg;
}

/* ARGSUSED */
static int
CompareSegment(w, seg1, seg2, n)
Widget w;
ICString *seg1;
ICString *seg2;
Cardinal *n;
{
    wchar *p, *q;
    int len, nsame;
    int result = 0;

    if (seg1->attr != seg2->attr) result |= ICAttrChanged;

    len = seg1->nchars > seg2->nchars ? seg2->nchars : seg1->nchars;
    nsame = 0;
    p = (wchar *)seg1->data;
    q = (wchar *)seg2->data;
    while (nsame < len && *p++ == *q++) nsame++;

    if (nsame != len || len != seg1->nchars || len != seg2->nchars)
	result |= ICStringChanged;

    if (n) *n = nsame;

    return result;
}

static ICString *
GetItemList(w, n)
Widget w;
Cardinal *n;
{
    AtokObject obj = (AtokObject)w;

#ifdef CAND_PAGE
    *n = (CAND_ROWS * CAND_COLS) + 1; /* +1 $B$O(B $B8uJd?t(B */
#else
    *n = obj->atok.nbr_of_cand;
#endif /* CAND_PAGE */

    return obj->atok.cand_lists_ics;
}

static int
SelectItem(w, n)
Widget w;
int n;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;
    int changed = FALSE;
    int status = 0;

    if (acomm == NULL) return -1; /* Not connected */

    if (n >= 0) obj->atok.selected_cand = n;

    /* ATOK $B$N3NDj(BFUNC$B$r8F$S=P$9!#(B */
    if (changed) {
	XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
			   (XtPointer)NULL);
	/* ATOK $B$N%P%C%U%!$b(BFLUSH$B$9$k!#(B */
	XtCallCallbackList((Widget)obj, obj->inputConv.textchangecallback,
			   (XtPointer)NULL);
    }

    return status;
}

static int
ConvertedString(w, encoding, format, length, string)
Widget w;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

    if (acomm == NULL || acomm->nbr_of_seg == 0 || acomm->offset == 0)
	return -1;

    wlen = acomm->segments[acomm->offset -1].length;
    wbuf = acomm->segments[acomm->offset -1].string;

    /*
     * Atok $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT( XtDisplayOfObject(( Widget )obj ));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++ ) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    XatokShiftLeftAll(acomm);

    return 0;
}

static int
ClearConversion(w)
Widget w;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;

    if (acomm == NULL) return -1; /* Not connected */

    XatokClearBuffer(acomm);
    XtCallCallbackList((Widget)obj, obj->inputConv.textchangecallback,
		       (XtPointer)NULL);
    return 0;
}

static ICString *
GetAuxSegments(w, n, ns, nc)
Widget w;
Cardinal *n, *ns, *nc;
{
    AtokObject obj = (AtokObject)w;
    Cardinal nseg, nchar;

    if (obj->atok.comm == NULL) return NULL; /* Not connected */

    if (n) {
	*n = obj->atok.aux_nseg;
    }

    if (obj->atok.aux_curseg < obj->atok.aux_nseg) {
	nseg = obj->atok.aux_curseg;
	nchar = 0;
    }
    else {
	nseg = 0;
	nchar = obj->atok.aux_length[0];
    }
    if (ns) {
	*ns = nseg;
    }
    if (nc) {
	*nc = nchar;
    }

    return obj->atok.aux_ics;
}

/* ARGSUSED */
static int
PreeditString(w, segn, offset, encoding, format, length, string)
Widget w;
int segn;
int offset;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
{
    AtokObject obj = (AtokObject)w;
    _XatokRecPtr acomm = obj->atok.comm;
    int i;
    wchar *wbuf, *wp;
    int	 len, wlen;
    extern int convJWStoCT();

    if (acomm == NULL) return -1;
    if (segn < acomm->nbr_of_seg && offset >= acomm->segments[segn].length) {
	/* $B%;%0%a%s%H$N:G8e(B */
	++segn;
	offset = 0;
    }
    if (segn >= acomm->nbr_of_seg || offset >= acomm->segments[segn].length) {
	/* $B:o=|$5$l$?(B */
	*encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
	*format = 8;
	*length = 0;
	*string = (XtPointer)XtMalloc(1);
	return 0;
    }

    wlen = 0;
    for (i = segn; i < acomm->nbr_of_seg; i++) {
	wlen += acomm->segments[i].length;
    }
    wlen -= offset;

    wp = wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    len = acomm->segments[segn].length - offset;
    (void)bcopy((char *)(acomm->segments[segn].string + offset),
		(char *)wp,
		sizeof(wchar) * len);
    wp += len;
    for (i = segn + 1; i < acomm->nbr_of_seg; i++) {
	len = acomm->segments[i].length;
	(void)bcopy((char *)acomm->segments[i].string,
		    (char *)wp,
		    sizeof(wchar) * len);
	wp += len;
    }
    wbuf[wlen] = 0;

    /*
     * Atok $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = (XtPointer)XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

/* ARGSUSED */
static int
StatusString(w, encoding, format, length, string, nchars)
Widget w;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
int *nchars;
{
    ICString *seg;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();

    seg = GetMode(w);
    if (seg == NULL) {
	*length = *nchars = 0;
	return -1;
    }

    wlen = seg->nchars;
    if (wlen <= 0) {
	*length = *nchars = 0;
	return -1;
    }

    /*
     * data $B$KF~$C$F$$$kJQ49%F%-%9%H$O(B null $B%?!<%_%M!<%H$5$l$F$$$J$$$+$b(B
     * $B$7$l$J$$$N$G!"$^$:%3%T!<$7$F(B null $B%?!<%_%M!<%H$9$k(B
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy(seg->data, (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * Canna $B%*%V%8%'%/%H$O(B COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$7$+%5%]!<%H$7$J$$(B
     * COMPOUND_TEXT $B$KJQ49$9$k(B
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT $B$O(B \r $B$,Aw$l$J$$$N$G(B \n $B$KJQ49$7$F$*$/(B */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);
    *nchars = seg->nchars;

    /* wbuf $B$r(B free $B$7$F$*$/(B */
    XtFree((char *)wbuf);

    return 0;
}

/*
 * Private FUNCTIONS
 */

static int
bell()
{
    if (displaybell) {
	XBell(displaybell, 0);
    }
    return 0;
}

static void
fix(obj)
AtokObject obj;
{
    /* $B3NDj$N=hM}(B */
    XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
		       (XtPointer)NULL);	/* $B!)!)!)(B */
}

static void
fixProcForAtok(obj, fixedstr, fixedlen)
AtokObject obj;
wchar *fixedstr;
int fixedlen;
{
    int offset;
    int i;

    offset = obj->atok.comm->offset;

    if (offset < ATOK_NSEG) {
	XatokShiftRight(obj->atok.comm);
	offset = obj->atok.comm->offset;
    }
    else {
	XatokShiftLeft(obj->atok.comm);
    }
    copyInWchar(fixedstr, fixedlen,
		&(obj->atok.comm->segments[offset-1].string),
		&(obj->atok.comm->segments[offset-1].size),
		&(obj->atok.comm->segments[offset-1].length));
}

/*
 * $BJQ49=*N;(B
 */
static void
convend(obj)
AtokObject obj;
{
    XtCallCallbackList((Widget)obj, obj->inputConv.endcallback,
		       (XtPointer)NULL);
}

/*
 * keeping list of objects
 */
typedef struct _oblist_ {
    AtokObject obj;
    struct _oblist_ *next;
} ObjRec;

static ObjRec *ObjList = NULL;

static void
addObject(obj, sock, clntid, isConnect)
AtokObject obj;
int sock;
int clntid;
int isConnect;
{
    ObjRec *objp = XtNew(ObjRec);
    _XatokRecPtr acomm;
    char buf[256];

    objp->obj = obj;
    objp->next = ObjList;
    ObjList = objp;

    acomm = objp->obj->atok.comm;
    /* bzero(acomm, sizeof(_XatokRecPtr)); */

    acomm->convStatus = M_KEYCONV;	/* $B4A;zJQ49(B ON $B$N%b!<%I$K@_Dj(B */
    /* acomm->mode.dsp = 1;	/* $B%5!<%P$+$i<hF@$9$k;v$R$i$,$J(B $B%b!<%I$K@_Dj(B */

    if (isConnect) {
	atokCEOpen(acomm->sock,
		   clntid, objp->obj->atok.comm
		   /*
		   &objp->obj->atok.comm->NETrqst,
		   &objp->obj->atok.comm->NETrslt);
		   */
		   );
    }
}

static void
deleteObject(obj)
AtokObject obj;
{
    ObjRec *objp, *objp0;

    for (objp0 = NULL, objp = ObjList;
	 objp != NULL;
	 objp0 = objp, objp = objp->next) {
	if (objp->obj == obj) {
	    if (objp0 == NULL) {
		ObjList = objp->next;
	    } else {
		objp0->next = objp->next;
	    }
	    XtFree((char *)objp);
	    return;
	}
    }
}

static void
atokDisplay(obj, kstat)
AtokObject obj;
int *kstat;
{
    wchar *wbuf = obj->atok.comm->wbuf;
    int len = obj->atok.comm->wlen;

    /*
     * len > 0 $B$N;~$O!"(B atokClntEvent $B$GJQ49$r9T$C$?7k2L!"(B
     * $B3NDjJ8;zNs$,$"$k;~$G$"$k!#(B
     */
    if (len > 0) {
	/*
	XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
			   (XtPointer)NULL);
	*/
	fixProcForAtok(obj, wbuf, len);
	fix(obj);
    }
    changeTextForAtok(obj, kstat);

    /*
     * $BI=<($9$kFbMF(B($B%F%-%9%H(B)$B$,JQ$o$C$F$$$?>l9g$K$O!"I=<($7$J$*$9!#(B
     */
    if ( obj->atok.textchanged ) {
	XtCallCallbackList((Widget)obj, obj->inputConv.textchangecallback,
			   (XtPointer)NULL);
	obj->atok.textchanged = False;
    }
}

static int
atokCandDisplay(obj, aajcode)
AtokObject obj;
int aajcode;
{
    int	status = 0;

    /*
     * $B8uJd%&%$%s%I%&I=<(;~$N%-!<F~NO(B
     * $B%-!<$O(B key.c $B$GDj5A$7$F$$$k$b$N$r;HMQ$9$k;v!#(B
     */
    switch( aajcode ) {
    case XFER:
#ifdef CAND_PAGE
	startSelection(obj, 1);
#else
	startSelection(obj, 0);
#endif
	break;
    case SPACE:
    case RIGHT:
    case CTR_F:
#ifdef	CAND_PAGE
	moveRight(obj, 1);
#else
	moveRight(obj, 0);
#endif	
	break;
    case LEFT:
    case CTR_B:
#ifdef	CAND_PAGE
	moveLeft(obj, 1);
#else
	moveLeft(obj, 0);
#endif
	break;
    case UP:
    case CTR_P:
	moveSelection(obj, ICMoveUp);
	break;
    case DOWN:
    case CTR_N:
	moveSelection(obj, ICMoveDown);
	break;
    case CTR_E:
	moveSelection(obj, ICMoveLast);
	break;
    case CTR_A:
	moveSelection(obj, ICMoveFirst);
	break;
    case CR:
    case VKEY|FUNC_KAKU_BUBUN:
	endSelection(obj, FALSE, TRUE);
	status = 1;
	break;
    case CTR_G:
	endSelection(obj, TRUE, TRUE);
	obj->atok.cur_cand = -1;
	break;
    case CTR_DEL:	/* 616 */
    case EESC:
    case FUNC_CNV_CANCL:	/* 1999/06/30 */
	endSelection(obj, TRUE, TRUE);
	obj->atok.cur_cand = -1;
	status = 1;
	break;
    case 0:
    default:
	break;
    }
    return status;
}
	
static int
atokAuxDisplay(obj, op)
AtokObject obj;
opMode op;
{
    ICAuxControlArg arg;
    int status = 1;

    switch (obj->atok.comm->menuStatus) {
    case ICAuxEnd :
	if (obj->atok.comm->convStatus & op) {
	    obj->atok.comm->convStatus ^= op;
	}
	obj->atok.comm->convStatus |= M_KEYCONV;
	status = 0;
    case ICAuxStart :
    case ICAuxChange :
	arg.command = obj->atok.comm->menuStatus;
	break;
    default :
	return -1;
    }
    if (arg.command != ICAuxEnd) {
	changeTextForAtokAUX(&obj->atok);
    }

    XtCallCallbackList((Widget)obj, obj->inputConv.auxcallback,
		       (XtPointer)&arg);
	
    return status;
}

/*
 * Utils
 */
/*
 * $B8uJd0lMwI=<($N4X?t74(B
 */
/*
 * $B40(B
 */
static void
moveLeft(obj, cand_page)
AtokObject obj;
int cand_page;
{
    _XatokRecPtr comm;
    int pgMax;
    int move = TRUE;
    ICSelectionControlArg arg;
    int kohoNum;

    if (!cand_page) {
	moveSelection(obj, ICMoveLeft);
	return;
    }
    comm = obj->atok.comm;

    querySelection(obj);
    pgMax = comm->kohoMax;

    if (comm->kohoPos == 0) {
	/*
	 *  $B:G8e$N%Z!<%8$rI=<($7$F$$$k!#(B
	 */
	if (comm->kohoCurPage + 1 == comm->kohoPageNbr) {
		/*
		 * $B:G8e$N%Z!<%8$@$,!":G=i$N%Z!<%8$G$"$k(B
		 * $B$7$?$,$C$FJQ99$9$k%Z!<%8$O$J$$!#(B
		 */
		if (comm->kohoCurPage == 0) {
		    move = TRUE;
		}
		else {
		    comm->kohoCurPage--;
		    move = FALSE;
		}
	}
	else if (comm->kohoCurPage == 0) {
	    comm->kohoCurPage = comm->kohoPageNbr - 1;
	    move = FALSE;
	}
	else  {
	    comm->kohoCurPage--;
	    move = FALSE;
	}
    }

    if (move == FALSE) {
	endSelection(obj, FALSE, FALSE);
	kohoNum = comm->kohoNum2 > (comm->kohoCurPage + 1) * pgMax ?
		pgMax - 1 : comm->kohoNum2 - (comm->kohoCurPage * pgMax)  - 1;
	makeCandList(obj, obj->atok.cand_lists_ics, pgMax, pgMax + 1,
		     comm->kohoCurPage, kohoNum, 1);
	return;
    }

    moveSelection(obj, ICMoveLeft);
}
	
static void
moveRight(obj, cand_page)
AtokObject obj;
int cand_page;
{
    _XatokRecPtr comm;
    int pgMax;
    int move = TRUE;
    int kohoNum;
    ICSelectionControlArg arg;

    if (!cand_page) {
	moveSelection(obj, ICMoveRight);
	return;
    }
    comm = obj->atok.comm;

    querySelection(obj);
    /* pgMax = CAND_COLS * CAND_ROWS; */
    pgMax = comm->kohoMax;

    if (comm->kohoPos == pgMax - 1) {
	if (comm->kohoNum2 > (comm->kohoCurPage + 1) * pgMax) {
	    comm->kohoCurPage++;
	}
	else {
	    comm->kohoCurPage = 0;
	}
	move = FALSE;
	comm->kohoPos = 0;
    }
    else if (comm->kohoPos == comm->kohoNum2 - (comm->kohoCurPage * pgMax) - 1)
    {
	/* if (comm->kohoCurPage != 0) move = FALSE; */
	comm->kohoCurPage = 0;
	move = FALSE;
	comm->kohoPos = 0;
    }
    if (move == FALSE) {
	endSelection(obj, FALSE, FALSE);
	makeCandList(obj, obj->atok.cand_lists_ics, pgMax, pgMax + 1,
		     comm->kohoCurPage, 0, 1);
	comm->kohoPos = 0;
	return;

    }

    moveSelection( obj, ICMoveRight );
}
		
static void
setSelection(obj, kohoNum)
AtokObject obj;
int kohoNum;
{
    ICSelectionControlArg arg;

    /* Set current candidate */
    arg.command = ICSelectionSet;
    arg.u.current_item = kohoNum;	/* $B8uJd(BNO$B$r@_Dj$9$k;v(B */
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
}

static void
moveSelection(obj, dir)
AtokObject obj;
int dir;
{
    ICSelectionControlArg arg;

    arg.command = ICSelectionMove;
    arg.u.dir = dir;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
    querySelection(obj);
}

static void
querySelection(obj)
AtokObject obj;
{
    ICSelectionControlArg arg;
    arg.command = ICSelectionGet;
    arg.u.current_item = -1;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    obj->atok.selected_cand = arg.u.current_item;
    obj->atok.comm->kohoPos = arg.u.current_item;
}

static void
endSelection(obj, isabort, modeChange)
AtokObject obj;
int isabort;
int modeChange;
{
    ICSelectionControlArg arg;
    int selected;

    if (modeChange) {
	obj->atok.comm->convStatus ^= M_CAND;
    }
    arg.command = ICSelectionEnd;
    arg.u.current_item = -1;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    if (!isabort && (selected = arg.u.current_item) >= 0) {
	insertSelection(obj, selected);
    }
}

static void
insertSelection(obj, selected)
AtokObject obj;
int selected;
{
    obj->atok.cur_cand = selected;
    obj->atok.comm->kohoPos = selected;

    /*
     * ATOK CE $B$KA*Br$5$l$?$3$H$rDLCN$9$kI,MW$"$j(B
     * API$B$,2r$j<!Bh(B $B%3!<%G%#%s%0(B
     */
    return;
}

/*
 * $B8uJd%j%9%H$N$?$a$N%P%C%U%!$rMQ0U$9$k!#(B
 */
static int
allocCandList(obj, nbr, cur)
AtokObject obj;
int nbr;
int *cur;
{
    ICString *p;

    /*
     * $B4{$KI,MW$J%5%$%:0J>e(B $B3NJ]$5$l$F$$$k>l9g$K$O2?$b$7$J$$!#(B
     */
    if (nbr <= obj->atok.cand_list_size) {
	return nbr;
    }

    /*
     * $B$^$@(B $B3NJ]$5$l$F$$$J$$;~$K$O!"?75,$K!"4{$K(B
     * $B3NJ]$5$l$F$$$kNN0h$r9-$2$?$$;~$K$O(B realloc $B$9$k!#(B
     */
    if (obj->atok.cand_list_size == 0) {
	p = (ICString *)XtMalloc(nbr * sizeof(ICString));
    }
    else {
	p = (ICString *)XtRealloc((char *)obj->atok.cand_lists_ics,
				  nbr * sizeof(ICString));
    }
    obj->atok.cand_lists_ics = p;
    obj->atok.cand_list_size = nbr;
    *cur = 0;

    return nbr;
}

/*
 * $B%-!<F~NO8e!"(BCE$B$K$h$C$FJQ49$5$l$?%G!<%?$r2hLL$KI=<($9$k$?$a$N(B
 * $B%3%s%H%m!<%k$r9T$&=hM}$N%a%$%s$G$"$k!#(B
 * $BI=<(%;%0%a%s%H$N7W;;!"%,%$%I%i%$%s$J$I$NI=<(J8;zNs$N:n@.$r9T$C$F$$$k!#(B
 *
 * $B$3$3$G9T$&$N$OBg$-$/J,$1$F#3$D$N=hM}$G$"$k!#(B
 *	1 $BF~NOESCf$NJ8;zNs$N(BECHO BACK
 *	2 $BF~NO%b!<%I$,JQ49$5$l$?;~$N(B $BF~NO%b!<%II=<(J8;zNs$N@_Dj(B
 *	3 $B0lMwI=<($7$F$$$k;~$N(B
 */
static int
changeTextForAtok(obj, kstat)
AtokObject obj;
int *kstat;
{
    _XatokRecPtr acomm = obj->atok.comm;
    int remain;
    int offset = acomm->offset;
    int len = acomm->echoLen;
    int i;
    wchar *wp;

    if (len == 0) {
	acomm->cur_seg = offset;
	acomm->nbr_of_seg = offset;
	acomm->segments[offset+0].length
		= acomm->segments[offset+1].length
		= acomm->segments[offset+2].length = 0;
	if (*kstat == XKEY_FIX) {
	    obj->atok.textchanged = TRUE;
	}
	obj->atok.textchanged = TRUE;
    }
    else if (len > 0) {
	/* $B%-!<F~NO$,$"$C$?>l9g(B */
	acomm->segments[offset+1].length
		= acomm->segments[offset+2].length = 0;
	if (acomm->revLen > 0) {
	    if ( acomm->revPos == 0 ) {
		remain = acomm->echoLen - acomm->revLen;
		/*
		 * REVERSE :
		 * REVERSE : NORMAL
		 * $BH?E>I=<($,#1%;%0%a%s%HL\$N>l9g$K$O(B
		 * str[0] $B$KJ8;z$rB-$9(B
		 */
		copyInWchar(acomm->echoStr, acomm->revLen,
			    &(acomm->segments[offset+0].string),
			    &(acomm->segments[offset+0].size),
			    &(acomm->segments[offset+0].length));
		acomm->cur_seg = offset;
		acomm->nbr_of_seg = offset + 1;
		/*
		 * $B#2%;%0%a%s%HL\$,B8:_$9$k>l9g$K$O!"(B
		 * $B#2%;%0%a%s%HL\$b@_Dj$9$k!#(B
		 */
		if (remain) {
		    copyInWchar(acomm->echoStr + acomm->revLen, remain,
				&(acomm->segments[offset+1].string),
				&(acomm->segments[offset+1].size),
				&(acomm->segments[offset+1].length));
		    acomm->nbr_of_seg = offset+2;
		}
	    }
	    else {
		/*
		 * NORMAL : REVERSE
		 * NORAML : REVERSE : NORMAL
		 *
		 * $B$3$3$K$/$k$H$-$O(B $B#2HVL\0J9_$J$N$G(B str0,
		 * str1  $B$O@dBP$KB8:_$9$k!#(B
		 * $BH?E>I=<($,$"$j!"3n$D#1%;%0%a%s%H$,%N!<%^%k(B
		 * $BI=<($H$$$&;v$O#2%;%0%a%s%HL\$,H?E>I=<($G(B
		 * $B$"$k!#(B
		 */
		remain = acomm->echoLen - acomm->revPos - acomm->revLen;
		copyInWchar(acomm->echoStr, acomm->revPos,
			    &(acomm->segments[offset+0].string),
			    &(acomm->segments[offset+0].size),
			    &(acomm->segments[offset+0].length));
		copyInWchar(acomm->echoStr + acomm->revPos, acomm->revLen,
			    &(acomm->segments[offset+1].string),
			    &(acomm->segments[offset+1].size),
			    &(acomm->segments[offset+1].length));
		acomm->cur_seg = offset+1;
		acomm->nbr_of_seg = offset+2;
		/*
		 * NORMAL : REVERSE : NORMAL $B$N>l9g(B
		 */
		if (remain){
		    copyInWchar(acomm->echoStr + acomm->revPos + acomm->revLen,
				remain,
				&(acomm->segments[offset+2].string),
				&(acomm->segments[offset+2].size),
				&(acomm->segments[offset+2].length));
		    acomm->nbr_of_seg = offset+3;
		}
	    }
	}
	else {
	    /*
	     * NORMAL $BI=<((BONLY
	     */
	    copyInWchar(acomm->echoStr, acomm->echoLen,
			&(acomm->segments[offset+0].string),
			&(acomm->segments[offset+0].size),
			&(acomm->segments[offset+0].length));
	    acomm->segments[1].length = acomm->segments[2].length = 0;
	    acomm->nbr_of_seg = offset+1;
	    acomm->cur_seg = offset+1;
	}
	obj->atok.textchanged = TRUE;
    }
}

static int
changeTextForAtokAUX(obj)
AtokPart *obj;
{
    int remain;
    int len;
    int i;
    _XatokRecPtr acomm = obj->comm;

    /*
     * $B$3$3$G(B AUX $B$KI=<($9$kJ8;z$r:n@.$9$k!#(B
     */
    if (obj->comm->convStatus & M_SYSLINE) {
	auxSyslineString(acomm, acomm->curAux,
			 acomm->aux_echoStr, &acomm->aux_echoLen,
			 &acomm->aux_revLen, &acomm->aux_revPos);
    }
    else {
	auxMenuString(acomm, acomm->curAux,
		      acomm->aux_echoStr, &acomm->aux_echoLen,
		      &acomm->aux_revLen, &acomm->aux_revPos);
    }
    /*
     * $BI=<($9$kJ8;z$,$J$$>l9g$O%j%?!<%s$9$k!#(B
     */
    if ( acomm->aux_echoLen == 0 ) {
	obj->aux_curseg = 0;
	obj->aux_nseg = 0;
	obj->aux_length[0] = obj->aux_length[1] = obj->aux_length[2] = 0;
	return 0;
    }
    obj->aux_length[1] = obj->aux_length[2] = 0;
    if (acomm->aux_revLen > 0) {
	if (acomm->aux_revPos == 0) {
	    remain = acomm->aux_echoLen - acomm->aux_revLen;
	    copyInWchar(acomm->aux_echoStr, acomm->aux_revLen,
			&obj->aux_string[0],
			&obj->aux_size[0],
			&obj->aux_length[0]);
	    obj->aux_curseg = 0;
	    obj->aux_nseg = 1;
	    /*
	     * $B#2%;%0%a%s%HL\$,B8:_$9$k>l9g$K$O!"(B
	     * $B#2%;%0%a%s%HL\$b@_Dj$9$k!#(B
	     */
	    if (remain) {
		copyInWchar(acomm->aux_echoStr + acomm->aux_revLen, remain,
			    &obj->aux_string[1],
			    &obj->aux_size[1],
			    &obj->aux_length[1]);
		obj->aux_nseg = 2;
	    }
	}
	else {
	    /*
	     * NORMAL : REVERSE
	     * NORAML : REVERSE : NORMAL
	     *
	     * $B$3$3$K$/$k$H$-$O(B $B#2HVL\0J9_$J$N$G(B str0,
	     * str1  $B$O@dBP$KB8:_$9$k!#(B
	     * $BH?E>I=<($,$"$j!"3n$D#1%;%0%a%s%H$,%N!<%^%k(B
	     * $BI=<($H$$$&;v$O#2%;%0%a%s%HL\$,H?E>I=<($G(B
	     * $B$"$k!#(B
	     */
	    remain = acomm->aux_echoLen - acomm->aux_revPos - acomm->aux_revLen;
	    copyInWchar(acomm->aux_echoStr, acomm->aux_revPos,
			&obj->aux_string[0],
			&obj->aux_size[0],
			&obj->aux_length[0]);
	    copyInWchar(acomm->aux_echoStr + acomm->aux_revPos,
			acomm->aux_revLen,
			&obj->aux_string[1],
			&obj->aux_size[1],
			&obj->aux_length[1]);
	    obj->aux_curseg = 1;
	    obj->aux_nseg = 2;
	    /*
	     * NORMAL : REVERSE : NORMAL $B$N>l9g(B
	     */
	    if (remain) {
		copyInWchar(acomm->aux_echoStr + acomm->aux_revPos
							+ acomm->aux_revLen,
			    remain,
			    &obj->aux_string[2],
			    &obj->aux_size[2],
			    &obj->aux_length[2] );
		obj->aux_nseg = 3;
	    }
	}
    }
    else {
	/*
	 * NORMAL $BI=<((BONLY
	 */
	copyInWchar(acomm->aux_echoStr, acomm->aux_echoLen,
		    &obj->aux_string[0],
		    &obj->aux_size[0],
		    &obj->aux_length[0]);
	obj->aux_length[1] = obj->aux_length[1] = 0;
	obj->aux_nseg = 1;
	obj->aux_curseg = 1;
    }

    for (i = 0; i < obj->aux_nseg; i++) {
	obj->aux_ics[i].data = (char *)obj->aux_string[i];
	obj->aux_ics[i].nchars = obj->aux_length[i];
	obj->aux_ics[i].nbytes = obj->aux_length[i] * sizeof(wchar);
	obj->aux_ics[i].attr = ICAttrConverted;
    }
    if (obj->aux_curseg < obj->aux_nseg) {
	obj->aux_ics[obj->aux_curseg].attr |= ICAttrCurrentSegment;
    }
    return obj->aux_nseg;
}

/*
 * $B8uJdJ8;zNs4X78$N%=!<%9(B
 */

/* Page */
/*
 * [$B4X?tL>(B]
 *		()
 * [$BI=Bj(B]
 *		
 * [$B8F=P7A<0(B]
 *		
 * [$B0z?t(B]
 *		$B7?(B        : $BL>(B       $B>N(B   : IO : $B@b(B      $BL@(B
 *	
 *	
 * [$BJV$jCM(B]
 *		
 * [$B;HMQ4X?t(B]
 *	
 * [$B5!G=(B]
 *	Event $B$,H/@8$7$F!"(B CE$B$h$j(B $B8uJd%j%9%H$,<hF@$7$?8e$KI=<($r(B
 *	 $B3+;O$9$k;~$K8F$S=P$5$l$k!#(B
 *
 */
static void
startSelection(obj, cand_page)
AtokObject obj;
int cand_page;
{
    _XatokRecPtr comm = obj->atok.comm;
    ICString *icsp;
    int i, n;
    int pgMax;
    int oldSize;

    n = obj->atok.nbr_of_cand = comm->kohoNum2;
    if (cand_page) {
	pgMax = CAND_COLS * CAND_ROWS;
    }
    else {
	pgMax = n;
    }
    comm->kohoMax = pgMax;

    comm->kohoPageNbr = (comm->kohoNum2 + pgMax - 1) / pgMax;
    comm->kohoCurPage = 0;		/* $B%Z!<%8$O#0$+$i;O$^$k!#(B*/
    /* comm->kohoPos = 1;		/* ATOKCE $B$N%j%?!<%s$r@_Dj(B */

    /*
     * $B$^$:$O(B $B8uJd$r@_Dj$9$k(BICSString $B$N%a%b%j!<$r3NJ]$7$F!"(B
     * $B=i4|2=$9$k!#(B
     */
    oldSize = obj->atok.cand_list_size;
    if (cand_page) {
	/*
	 * $B8uJdI=<($N:GBg?t$,7h$^$C$F$$$k>l9g$O!"(B
	 * $B8uJd$N$?$a$N(BICS $B$,<hF@$5$l$F$$$k$+!"$$$J$$$+$N>l9g$7$+$J$$(B
	 */
	n = pgMax + 1;		/* +1 $B$O8uJdHV9fI=<(MQ(B */
	if (obj->atok.cand_list_size == 0) {
	    icsp = (ICString *)XtMalloc(n * sizeof(ICString));
	    obj->atok.cand_list_size = n;
	    obj->atok.cand_lists_ics = icsp;
	}
    }
    else {
	if (obj->atok.cand_list_size == 0) {
	    icsp = (ICString *)XtMalloc(n * sizeof(ICString));
	    obj->atok.cand_list_size = n;
	    obj->atok.cand_lists_ics = icsp;
	}
	else if (obj->atok.cand_list_size < n) {
	    icsp = (ICString *)XtRealloc((char *)obj->atok.cand_lists_ics,
					 n * sizeof(ICString));
	    obj->atok.cand_list_size = n;
	    obj->atok.cand_lists_ics = icsp;
	}
	else {
	    icsp = obj->atok.cand_lists_ics;
	}
    }

    /*
     * $B<hF@$5$l$F$$$k8uJd$r@_Dj$9$k!#(B
     */
    icsp = obj->atok.cand_lists_ics;
#ifdef CAND_PAGE
    makeCandList(obj, icsp, pgMax, n, comm->kohoPos / pgMax, comm->kohoPos, 1);
#else
    makeCandList(obj, icsp, pgMax, n, comm->kohoPos / pgMax, comm->kohoPos, 0);
#endif
}

static int
makeCandList(obj, icsp, pgMax, icsNum, page, kohoNum, cand_page)
AtokObject obj;
ICString *icsp;	/* $BI=<(MQ%G!<%?(B */
int pgMax;	/* $B#1%Z!<%8$N:GBg?t(B */
int icsNum;
int page;	/* $BI=<($9$k%Z!<%8(B */
int kohoNum;
int cand_page;
{
    _XatokRecPtr comm = obj->atok.comm;
    int i, j, n;
    int len, ksize, klen;
    int bytes, chars;
    int es;
    unsigned char *ep;
    wchar wbuf[BUFSIZ];
    char euc[BUFSIZ];
    int stNbr;
    int maxCand = 0;
    ICSelectionControlArg arg;
    ICString *ticsp = icsp;	/* $BI=<(MQ%G!<%?(B */

    for (i = 0; i < icsNum; i++, ticsp++) {
	/* if (i < oldSize) continue; */
	ticsp->nbytes = (unsigned short)0;
	ticsp->nchars = (unsigned short)0;
	ticsp->data = (char *)0;
	ticsp->attr = 0;
    }
    stNbr = page * pgMax;

    ksize = 0;
    for(i = 0; i < stNbr; i++) {
	ksize += comm->kohoLenPtr[i];
    }
    if (cand_page) {
	n = comm->kohoNum2 - stNbr > pgMax ? pgMax : comm->kohoNum2 - stNbr;
    }
    else {
	n = pgMax;
    }
    for (j = 0; j < n; j++) {
	bzero(euc, sizeof(euc));
	klen = comm->kohoLenPtr[i];
	es = klen * 3;
	ep = (unsigned char *)XtMalloc(es);
	bzero(ep, es);
	ucs2euc(&comm->kohoStrPtr[ksize], klen, ep, es, 0x0000a2ae);
	es = strlen(ep);
	ksize += klen;
#ifdef CAND_PAGE
	sprintf(euc, "%2d %s", j + 1, ep);
#else
	strcpy(euc, ep);
#endif
	es = strlen(euc);
	es = euc2wcs(euc, es, wbuf);
	XtFree((char *)ep);

	bytes = (int)icsp->nbytes;
	chars = (int)icsp->nchars;
	copyInWchar(wbuf, es, (wchar **)&(icsp->data), &bytes, &chars);
	icsp->nbytes = (unsigned short)bytes;
	icsp->nchars = (unsigned short)chars;
	if (chars > maxCand) {
	    maxCand = chars;
	}
	icsp->attr = ICAttrNormalString;
	icsp++;
	i++;
    }
    for(; j < pgMax; j++) {
	icsp->nbytes = (unsigned short)0;
	icsp->nchars = (unsigned short)0;
	icsp->attr = ICAttrNormalString;
	icsp->data = (char *)0;
	icsp++;
    }
    if (cand_page) {
#if SHOW_PAGE
	sprintf(euc, "%d/%d", page * pgMax + 1, comm->kohoNum2);
#else
	sprintf(euc, "Page %d/%d", page + 1,
				   (comm->kohoNum2 + pgMax - 1) / pgMax);
#endif
	es = strlen(euc);
	es = euc2wcs(euc, es, wbuf);
	bytes = (int)icsp->nbytes;
	chars = (int)icsp->nchars;
	copyInWchar(wbuf, es, ( wchar ** )&( icsp->data ), &bytes, &chars);

	icsp->nbytes = (unsigned short)bytes;
	icsp->nchars = (unsigned short)chars;
	icsp->attr = ICAttrNormalString;
    }

    comm->kohoCurPage = page;

    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionCandidates;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    /* Set current candidate */
    arg.command = ICSelectionSet;
    arg.u.current_item = kohoNum;	/* $B8uJd(BNO$B$r@_Dj$9$k;v(B */
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    return page;
}


/*
   copyInWchar -- wchar $B$r%3%T!<$9$k!#(B

	ws, wlen $B$G<($5$l$?(B wchar $BJ8;zNs$r(B wsbuf $B$N%]%$%s%H@h$N%P%C%U%!$K3J(B
	$BG<$9$k!#(Bwsbuf $B$N%5%$%:$O(B wssize $B$N%]%$%s%H@h$K3JG<$5$l$F$$$kCM$G;X(B
	$BDj$5$l$k$,!"$=$l$G$O>.$5$$;~$O(B copyInWchar $BFb$G(B XtRealloc $B$5$l!"?7(B
	$B$?$K%"%m%1!<%H$5$l$?%P%C%U%!$,(B wsbuf $B$N%]%$%s%H@h$K3JG<$5$l$k!#$^$?!"(B
	$B%P%C%U%!$N?7$?$J%5%$%:$,(B wssize $B$N%]%$%s%H@h$K3JG<$5$l$k!#F@$i$l$?(B
	$BJ8;z?t$,(Bwslen $B$N%]%$%s%H@h$K3JG<$5$l$k!#(B
*/
static void
copyInWchar(ws, wlen, wsbuf, wssize, wslen)
wchar *ws;
int wlen;
wchar **wsbuf;
int *wssize;
int *wslen;
{
    int i;

    if (*wssize == 0) {
	*wsbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
	*wssize = wlen + 1;
    }
    if (wlen + 1 > *wssize) {
	*wsbuf = (wchar *)XtRealloc((char *)*wsbuf, (wlen + 1) * sizeof(wchar));
	*wssize = wlen + 1;
    }
    *wslen = wlen;
    (void)bcopy(ws, *wsbuf, wlen * sizeof(wchar));
    *(*wsbuf + wlen) = (wchar)0;
}
/* Atok.c */
