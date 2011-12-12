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
    /* 何もしない */
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

    /* 入力バッファを作成する。*/
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
    wchar wbuf[BUFSIZ*4];	/* 確定文字列のバッファ */
    int wlen;			/* 確定文字列の長さ */
    unsigned char kbuf[BUFSIZ];	/* キー入力データ */
    int nbytes;			/* キー入力の長さ  */
    KeySym ks;			/* Xのキーシンボル */
    XComposeStatus comp_status;		
    WORD aajcode;		/* AAJCODE */
    int kanjiStatus;
    int enableStatus;
    int changeStatus;
    int elseEvent;
    int status;
    int isThrue = 0;

    DPRINT(( "\n### NEW EVENT \n" ));

    /* KeyPress以外は捨てる */
    if (event->type != KeyPress /*&& event->type != KeyRelease*/) return 0;

    obj->atok.textchanged = False;

    /*
     * AtokServer が 再起動されたばあいなど、コネクションがきれている場合が
     * あるので 再度コネクションを張ってみる。
     */
    if (obj->atok.comm == NULL) {
	/* return -1; /* 接続できていないときは、エラーを返す */
	return 1; /* 接続できていないときは、無視する */
    }

    /* X のキーイベントを取得する。*/
    kbuf[0] = '\0';
    nbytes = XKanaLookup((XKeyEvent*)event, kbuf, sizeof(kbuf), &ks,
			 &comp_status);

    wbuf[0] = (wchar)kbuf[0]; /* きたない */

    DPRINT(("nbytes %d\n", nbytes));

    /* 要らない場合は無視する */
    if (nbytes == 0 && ks & 0xff00 == 0) return 1;

    /* ベルを鳴らすディスプレイの設定 */
    displaybell = XtDisplayOfObject((Widget)obj);

    /*
     * 候補ウィンドウがACTIVE になっていた場合の候補の移動は
     * Kinput2 より Callback が呼び出されず、選択されている番号が
     * 取得できないので、ここで一度 選択されている番号を取得する。
     */
    if (obj->atok.comm->convStatus & M_CAND) {
	querySelection(obj);
    }

    /*
     * キー入力を元に AAJCODEに変換し、CEへのリクエストを行う。
     * リクエスト結果に基づいて、表示やモードを変更する。
     * 確定文字列があった場合には、リターンとして、その文字列の長さが
     * 返ってくる。
     * これ以降の処理は 画面に表示するための物である。
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
     * ここから下は kanjiStatus で指定されているモードの処理を
     * 行い、画面に表示する。
     * 候補一覧、、辞書、確定などに解れる。
     *
     * 通常のキー入力の場合
     * 候補一覧表示中の場合
     * 辞書ユーティリティー表示中の場合
     */
    switch(kanjiStatus) {
    case KRS_UNCHANGED: /* スルー確定 */
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
	/* モードの変更があった */
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
	/* 次候補ウィンドウ更新 */
	if (changeStatus &
	    (ATCHANGEB_RECTOPEN | ATCHANGEB_RECTCLOSE | ATCHANGEB_RECTCHANGE))
	{
	    /* do nothing */
	}
	if (changeStatus & (ATCHANGEB_SYSLSTR | ATCHANGEB_SYSLTOSYSL)) {
	    /*
	     * auxSyslineCtrl() の return が０以外の時は、
	     * システム行の表示が終了の時であり、
	     * 確定文字列が存在する可能性があるので
	     * システム行を消してから
	     * AuxDisplay()を呼び出す必要がある。
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
     * ここから下は 表示のコントロール
     * 入力されたキーにより処理を分けている。
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
	/* 辞書ユーティリティー表示時のキー入力 */
    }
    /* その他の場合のキー入力 */
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
    *n = (CAND_ROWS * CAND_COLS) + 1; /* +1 は 候補数 */
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

    /* ATOK の確定FUNCを呼び出す。 */
    if (changed) {
	XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
			   (XtPointer)NULL);
	/* ATOK のバッファもFLUSHする。 */
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
     * Atok オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
     * COMPOUND_TEXT に変換する
     */
    *encoding = XA_COMPOUND_TEXT( XtDisplayOfObject(( Widget )obj ));
    *format = 8;

    /* COMPOUND_TEXT は \r が送れないので \n に変換しておく */
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
	/* セグメントの最後 */
	++segn;
	offset = 0;
    }
    if (segn >= acomm->nbr_of_seg || offset >= acomm->segments[segn].length) {
	/* 削除された */
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
     * Atok オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
     * COMPOUND_TEXT に変換する
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

    /* COMPOUND_TEXT は \r が送れないので \n に変換しておく */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = (XtPointer)XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    /* wbuf を free しておく */
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
     * data に入っている変換テキストは null ターミネートされていないかも
     * しれないので、まずコピーして null ターミネートする
     */
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy(seg->data, (char *)wbuf, sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * Canna オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
     * COMPOUND_TEXT に変換する
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject(w));
    *format = 8;

    /* COMPOUND_TEXT は \r が送れないので \n に変換しておく */
    for (wp = wbuf; *wp != 0; wp++) {
	if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);
    *nchars = seg->nchars;

    /* wbuf を free しておく */
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
    /* 確定の処理 */
    XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
		       (XtPointer)NULL);	/* ？？？ */
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
 * 変換終了
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

    acomm->convStatus = M_KEYCONV;	/* 漢字変換 ON のモードに設定 */
    /* acomm->mode.dsp = 1;	/* サーバから取得する事ひらがな モードに設定 */

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
     * len > 0 の時は、 atokClntEvent で変換を行った結果、
     * 確定文字列がある時である。
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
     * 表示する内容(テキスト)が変わっていた場合には、表示しなおす。
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
     * 候補ウインドウ表示時のキー入力
     * キーは key.c で定義しているものを使用する事。
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
 * 候補一覧表示の関数郡
 */
/*
 * 完
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
	 *  最後のページを表示している。
	 */
	if (comm->kohoCurPage + 1 == comm->kohoPageNbr) {
		/*
		 * 最後のページだが、最初のページである
		 * したがって変更するページはない。
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
    arg.u.current_item = kohoNum;	/* 候補NOを設定する事 */
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
     * ATOK CE に選択されたことを通知する必要あり
     * APIが解り次第 コーディング
     */
    return;
}

/*
 * 候補リストのためのバッファを用意する。
 */
static int
allocCandList(obj, nbr, cur)
AtokObject obj;
int nbr;
int *cur;
{
    ICString *p;

    /*
     * 既に必要なサイズ以上 確保されている場合には何もしない。
     */
    if (nbr <= obj->atok.cand_list_size) {
	return nbr;
    }

    /*
     * まだ 確保されていない時には、新規に、既に
     * 確保されている領域を広げたい時には realloc する。
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
 * キー入力後、CEによって変換されたデータを画面に表示するための
 * コントロールを行う処理のメインである。
 * 表示セグメントの計算、ガイドラインなどの表示文字列の作成を行っている。
 *
 * ここで行うのは大きく分けて３つの処理である。
 *	1 入力途中の文字列のECHO BACK
 *	2 入力モードが変換された時の 入力モード表示文字列の設定
 *	3 一覧表示している時の
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
	/* キー入力があった場合 */
	acomm->segments[offset+1].length
		= acomm->segments[offset+2].length = 0;
	if (acomm->revLen > 0) {
	    if ( acomm->revPos == 0 ) {
		remain = acomm->echoLen - acomm->revLen;
		/*
		 * REVERSE :
		 * REVERSE : NORMAL
		 * 反転表示が１セグメント目の場合には
		 * str[0] に文字を足す
		 */
		copyInWchar(acomm->echoStr, acomm->revLen,
			    &(acomm->segments[offset+0].string),
			    &(acomm->segments[offset+0].size),
			    &(acomm->segments[offset+0].length));
		acomm->cur_seg = offset;
		acomm->nbr_of_seg = offset + 1;
		/*
		 * ２セグメント目が存在する場合には、
		 * ２セグメント目も設定する。
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
		 * ここにくるときは ２番目以降なので str0,
		 * str1  は絶対に存在する。
		 * 反転表示があり、且つ１セグメントがノーマル
		 * 表示という事は２セグメント目が反転表示で
		 * ある。
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
		 * NORMAL : REVERSE : NORMAL の場合
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
	     * NORMAL 表示ONLY
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
     * ここで AUX に表示する文字を作成する。
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
     * 表示する文字がない場合はリターンする。
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
	     * ２セグメント目が存在する場合には、
	     * ２セグメント目も設定する。
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
	     * ここにくるときは ２番目以降なので str0,
	     * str1  は絶対に存在する。
	     * 反転表示があり、且つ１セグメントがノーマル
	     * 表示という事は２セグメント目が反転表示で
	     * ある。
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
	     * NORMAL : REVERSE : NORMAL の場合
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
	 * NORMAL 表示ONLY
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
 * 候補文字列関係のソース
 */

/* Page */
/*
 * [関数名]
 *		()
 * [表題]
 *		
 * [呼出形式]
 *		
 * [引数]
 *		型        : 名       称   : IO : 説      明
 *	
 *	
 * [返り値]
 *		
 * [使用関数]
 *	
 * [機能]
 *	Event が発生して、 CEより 候補リストが取得した後に表示を
 *	 開始する時に呼び出される。
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
    comm->kohoCurPage = 0;		/* ページは０から始まる。*/
    /* comm->kohoPos = 1;		/* ATOKCE のリターンを設定 */

    /*
     * まずは 候補を設定するICSString のメモリーを確保して、
     * 初期化する。
     */
    oldSize = obj->atok.cand_list_size;
    if (cand_page) {
	/*
	 * 候補表示の最大数が決まっている場合は、
	 * 候補のためのICS が取得されているか、いないかの場合しかない
	 */
	n = pgMax + 1;		/* +1 は候補番号表示用 */
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
     * 取得されている候補を設定する。
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
ICString *icsp;	/* 表示用データ */
int pgMax;	/* １ページの最大数 */
int icsNum;
int page;	/* 表示するページ */
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
    ICString *ticsp = icsp;	/* 表示用データ */

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
    arg.u.current_item = kohoNum;	/* 候補NOを設定する事 */
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    return page;
}


/*
   copyInWchar -- wchar をコピーする。

	ws, wlen で示された wchar 文字列を wsbuf のポイント先のバッファに格
	納する。wsbuf のサイズは wssize のポイント先に格納されている値で指
	定されるが、それでは小さい時は copyInWchar 内で XtRealloc され、新
	たにアロケートされたバッファが wsbuf のポイント先に格納される。また、
	バッファの新たなサイズが wssize のポイント先に格納される。得られた
	文字数がwslen のポイント先に格納される。
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
